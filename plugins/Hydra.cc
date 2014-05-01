#include <errno.h>
#include <libusb-1.0/libusb.h>
#include <linux/types.h>
#include <linux/input.h>
#include <linux/hidraw.h>
#include <cstring>

#include "gazebo/transport/transport.hh"
#include "gazebo/physics/physics.hh"

#include "plugins/Hydra.hh"

// loosely adapted from the following
// https://github.com/ros-drivers/razer_hydra/blob/groovy-devel/src/hydra.cpp

// Ugly hack to work around failing compilation on systems that don't
// yet populate new version of hidraw.h to userspace.
//
// If you need this, please have your distro update the kernel headers.

#ifndef HIDIOCSFEATURE
#define HIDIOCSFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x06, len)
#define HIDIOCGFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x07, len)
#endif

// eventually crawl hidraw file system using this:
// http://www.signal11.us/oss/udev/

using namespace gazebo;
GZ_REGISTER_WORLD_PLUGIN(RazerHydra)

#define HYDRA_RIGHT_BUMPER 7
#define HYDRA_RIGHT_1 8
#define HYDRA_RIGHT_2 9
#define HYDRA_RIGHT_3 10
#define HYDRA_RIGHT_4 11
#define HYDRA_RIGHT_CENTER 12
#define HYDRA_RIGHT_JOY 13

#define HYDRA_LEFT_LB 0
#define HYDRA_LEFT_1 1
#define HYDRA_LEFT_2 2
#define HYDRA_LEFT_3 3
#define HYDRA_LEFT_4 4
#define HYDRA_LEFT_CENTER 5
#define HYDRA_LEFT_JOY 6

/////////////////////////////////////////////////
RazerHydra::RazerHydra()
: hidrawFd(0)
{
  this->stop = false;
  this->lastCycleStart = common::Time::GetWallTime();

  // magic number for 50% mix at each step
  this->periodEstimate.SetFc(0.11, 1.0);

  this->periodEstimate.SetValue(0.004);
}

/////////////////////////////////////////////////
RazerHydra::~RazerHydra()
{
  event::Events::DisconnectWorldUpdateBegin(this->updateConnection);

  this->stop = true;
  this->pollThread->join();
}

/////////////////////////////////////////////////
void RazerHydra::Load(physics::WorldPtr _world, sdf::ElementPtr /*_sdf*/)
{
  int res;
  uint8_t buf[256];
  struct hidraw_report_descriptor rptDesc;
  struct hidraw_devinfo info;

  // Find the Razer device.
  std::string device;
  for (int i = 0; i < 6 && device.empty(); ++i)
  {
    std::ostringstream stream;
    stream << "/sys/class/hidraw/hidraw" << i << "/device/uevent";
    std::ifstream fileIn(stream.str().c_str());
    if (fileIn.is_open())
    {
      std::string line;
      while (std::getline(fileIn, line) && device.empty())
        if (line.find("HID_NAME=Razer Razer Hydra") != std::string::npos)
          device = "/dev/hidraw" + boost::lexical_cast<std::string>(i);
    }
  }

  if (device.empty())
  {
    gzerr << "Unable to find Razer device\n";
    return;
  }

  this->hidrawFd = open(device.c_str(), O_RDWR | O_NONBLOCK);
  if (this->hidrawFd < 0)
  {
    gzerr << "couldn't open hidraw device[" << device << "]\n";
    return;
  }

  memset(&rptDesc, 0x0, sizeof(rptDesc));
  memset(&info, 0x0, sizeof(info));
  memset(buf, 0x0, sizeof(buf));

  // Get Raw Name
  res = ioctl(this->hidrawFd, HIDIOCGRAWNAME(256), buf);
  if (res < 0)
    perror("HIDIOCGRAWNAME");

  // set feature to start it streaming
  memset(buf, 0x0, sizeof(buf));
  buf[6] = 1;
  buf[8] = 4;
  buf[9] = 3;
  buf[89] = 6;

  int attempt = 0;
  for (attempt = 0; attempt < 50; ++attempt)
  {
    res = ioctl(this->hidrawFd, HIDIOCSFEATURE(91), buf);
    if (res < 0)
    {
      gzerr << "unable to start streaming\n";
      perror("HIDIOCSFEATURE");
      common::Time::MSleep(500);
    }
    else
    {
      break;
    }
  }

  if (attempt >= 60)
  {
    gzerr << "Failed to load hydra\n";
    return;
  }


  this->updateConnection = event::Events::ConnectWorldUpdateBegin(
      boost::bind(&RazerHydra::Update, this, _1));

  this->pollThread = new boost::thread(boost::bind(&RazerHydra::Run, this));

  this->node = transport::NodePtr(new transport::Node());
  this->node->Init(_world->GetName());
  this->pub = this->node->Advertise<msgs::Hydra>("~/hydra");
}

/////////////////////////////////////////////////
void RazerHydra::Update(const common::UpdateInfo & /*_info*/)
{
  boost::mutex::scoped_lock lock(this->mutex);
  math::Pose origRight(this->pos[1], this->quat[1]);

  math::Pose pivotRight = origRight;
  math::Pose grabRight = origRight;

  pivotRight.pos += origRight.rot * math::Vector3(-0.04, 0, 0);
  grabRight.pos += origRight.rot * math::Vector3(-0.12, 0, 0);

  math::Pose origLeft(this->pos[0], this->quat[0]);

  math::Pose pivotLeft = origLeft;
  math::Pose grabLeft = origLeft;

  pivotLeft.pos += origLeft.rot.RotateVector(math::Vector3(-0.04, 0, 0));
  grabLeft.pos += origLeft.rot.RotateVector(math::Vector3(-0.12, 0, 0));

  msgs::Hydra msg;
  msgs::Hydra::Paddle *rightPaddle = msg.mutable_right();
  msgs::Hydra::Paddle *leftPaddle = msg.mutable_left();

  // Analog 0: Left right(+) left(-)
  // Analog 1: Left forward(+) back(-)
  // Analog 2: Left trigger(0-1)
  // Analog 3: Right right(+) left(-)
  // Analog 4: Right forward(+) back(-)
  // Analog 5: Right trigger(0-1)
  rightPaddle->set_joy_y(this->analog[3]);
  rightPaddle->set_joy_x(this->analog[4]);
  rightPaddle->set_trigger(this->analog[5]);

  leftPaddle->set_joy_y(this->analog[0]);
  leftPaddle->set_joy_x(this->analog[1]);
  leftPaddle->set_trigger(this->analog[2]);

  leftPaddle->set_button_bumper(this->buttons[0]);
  leftPaddle->set_button_1(this->buttons[1]);
  leftPaddle->set_button_2(this->buttons[2]);
  leftPaddle->set_button_3(this->buttons[3]);
  leftPaddle->set_button_4(this->buttons[4]);

  if (this->buttons[1] || this->buttons[2] || this->buttons[3] || this->buttons[4])
  	std::cout << "Button pressed" << std::endl;

  leftPaddle->set_button_center(this->buttons[5]);
  leftPaddle->set_button_joy(this->buttons[6]);

  rightPaddle->set_button_bumper(this->buttons[7]);
  rightPaddle->set_button_1(this->buttons[8]);
  rightPaddle->set_button_2(this->buttons[9]);
  rightPaddle->set_button_3(this->buttons[10]);
  rightPaddle->set_button_4(this->buttons[11]);
  rightPaddle->set_button_center(this->buttons[12]);
  rightPaddle->set_button_joy(this->buttons[13]);

  msgs::Set(rightPaddle->mutable_pose(), grabRight);
  msgs::Set(leftPaddle->mutable_pose(), grabLeft);

  this->pub->Publish(msg);
}

/////////////////////////////////////////////////
void RazerHydra::Run()
{
  common::Time pollTime(0, 5000);
  double cornerHz = 2.5;

  while(!this->stop)
  {
    this->Poll(pollTime, cornerHz);
  }

  if (this->hidrawFd >= 0)
  {
    uint8_t buf[256];
    memset(buf, 0, sizeof(buf));
    buf[6] = 1;
    buf[8] = 4;
    buf[89] = 5;

    if (ioctl(this->hidrawFd, HIDIOCSFEATURE(91), buf) < 0)
    {
      gzerr << "unable to stop streaming\n";
      perror("HIDIOCSFEATURE");
    }

    close(this->hidrawFd);
  }
}

/////////////////////////////////////////////////
bool RazerHydra::Poll(const common::Time &_timeToWait, float _lowPassCornerHz)
{
  if (this->hidrawFd < 0)
  {
    gzerr << "hidraw device is not open, couldn't poll.\n";
    return false;
  }

  if(_timeToWait == common::Time::Zero)
  {
    gzerr << "_msToWait must be at least 1.\n";
    return false;
  }

  if(_lowPassCornerHz <= std::numeric_limits<float>::epsilon())
  {
    gzerr << "Corner frequency for low-pass filter must be greater than 0."
      << "Aborting.\n";
    return false;
  }

  common::Time deadline = common::Time::GetWallTime() + _timeToWait;

  uint8_t buf[64];
  //while (common::Time::GetWallTime() < deadline)
  //{
    ssize_t nread = read(this->hidrawFd, buf, sizeof(buf));

    if (nread > 0)
    {
      static bool firstTime = true;

      // Update average read period
      if(!firstTime)
      {
        this->periodEstimate.Process(
            (common::Time::GetWallTime() - this->lastCycleStart).Double());
      }

      this->lastCycleStart = common::Time::GetWallTime();

      if (firstTime)
        firstTime = false;

      // Update filter frequencies
      float fs = 1.0 / this->periodEstimate.GetValue();
      float fc = _lowPassCornerHz;

      for (int i = 0; i < 2; ++i)
      {
        this->filterPos[i].SetFc(fc, fs);
        this->filterQuat[i].SetFc(fc, fs);
      }

      // Read data
      this->rawPos[0] = *((int16_t *)(buf+8));
      this->rawPos[1] = *((int16_t *)(buf+10));
      this->rawPos[2] = *((int16_t *)(buf+12));
      this->rawQuat[0] = *((int16_t *)(buf+14));
      this->rawQuat[1] = *((int16_t *)(buf+16));
      this->rawQuat[2] = *((int16_t *)(buf+18));
      this->rawQuat[3] = *((int16_t *)(buf+20));
      this->rawButtons[0] = buf[22] & 0x7f;
      this->rawAnalog[0] = *((int16_t *)(buf+23));
      this->rawAnalog[1] = *((int16_t *)(buf+25));
      this->rawAnalog[2] = buf[27];

      this->rawPos[3] = *((int16_t *)(buf+30));
      this->rawPos[4] = *((int16_t *)(buf+32));
      this->rawPos[5] = *((int16_t *)(buf+34));
      this->rawQuat[4] = *((int16_t *)(buf+36));
      this->rawQuat[5] = *((int16_t *)(buf+38));
      this->rawQuat[6] = *((int16_t *)(buf+40));
      this->rawQuat[7] = *((int16_t *)(buf+42));
      this->rawButtons[1] = buf[44] & 0x7f;
      this->rawAnalog[3] = *((int16_t *)(buf+45));
      this->rawAnalog[4] = *((int16_t *)(buf+47));
      this->rawAnalog[5] = buf[49];

      boost::mutex::scoped_lock lock(this->mutex);
      // Put the raw position and orientation into Gazebo coordinate frame
      for (int i = 0; i < 2; i++)
      {
        this->pos[i].x = -this->rawPos[3*i+1] * 0.001;
        this->pos[i].y = -this->rawPos[3*i+0] * 0.001;
        this->pos[i].z = -this->rawPos[3*i+2] * 0.001;

        this->quat[i].w = this->rawQuat[i*4+0] / 32768.0;
        this->quat[i].x = -this->rawQuat[i*4+2] / 32768.0;
        this->quat[i].y = -this->rawQuat[i*4+1] / 32768.0;
        this->quat[i].z = -this->rawQuat[i*4+3] / 32768.0;
      }


      // Apply filters
      for (int i = 0; i < 2; i++)
      {
        this->quat[i] = this->filterQuat[i].Process(this->quat[i]);
        this->pos[i] = this->filterPos[i].Process(this->pos[i]);
      }

      this->analog[0] = this->rawAnalog[0] / 32768.0;
      this->analog[1] = this->rawAnalog[1] / 32768.0;
      this->analog[2] = this->rawAnalog[2] / 255.0;
      this->analog[3] = this->rawAnalog[3] / 32768.0;
      this->analog[4] = this->rawAnalog[4] / 32768.0;
      this->analog[5] = this->rawAnalog[5] / 255.0;

      for (int i = 0; i < 2; ++i)
      {
        this->buttons[i*7  ] = (this->rawButtons[i] & 0x01) ? 1 : 0;
        this->buttons[i*7+1] = (this->rawButtons[i] & 0x04) ? 1 : 0;
        this->buttons[i*7+2] = (this->rawButtons[i] & 0x08) ? 1 : 0;
        this->buttons[i*7+3] = (this->rawButtons[i] & 0x02) ? 1 : 0;
        this->buttons[i*7+4] = (this->rawButtons[i] & 0x10) ? 1 : 0;
        this->buttons[i*7+5] = (this->rawButtons[i] & 0x20) ? 1 : 0;
        this->buttons[i*7+6] = (this->rawButtons[i] & 0x40) ? 1 : 0;
      }

      return true;
    }
    else
    {
      common::Time::NSleep(250000);
      // gzerr << "Read failed\n";
    }
    /*else
    {
      common::Time toSleep =
        this->lastCycleStart +
        common::Time(this->periodEstimate.GetValue() * 0.95);

      common::Time sleepDuration = toSleep - common::Time::GetWallTime();

      if(sleepDuration > 0)
        common::Time::Sleep(sleepDuration);
      else
        common::Time::NSleep(250000);
    }*/
  //}

  return false;
}