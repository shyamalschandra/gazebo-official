/*
 * Copyright (C) 2014-2015 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include <errno.h>
#include <libusb-1.0/libusb.h>
#include <linux/hidraw.h>
#include <linux/input.h>
#include <linux/types.h>
#include <cstring>
#include <mutex>
#include <thread>
#include "gazebo/physics/physics.hh"
#include "gazebo/transport/transport.hh"
#include "plugins/HydraPlugin.hh"

// Loosely adapted from the following
// https://github.com/ros-drivers/razer_hydra/blob/groovy-devel/src/hydra.cpp

// Ugly hack to work around failing compilation on systems that don't
// yet populate new version of hidraw.h to userspace.
//
// If you need this, please have your distro update the kernel headers.

#ifndef HIDIOCSFEATURE
#define HIDIOCSFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x06, len)
#define HIDIOCGFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x07, len)
#endif

// Eventually crawl hidraw file system using this:
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
  : pollThread(nullptr),
    stop(false)
{
}

/////////////////////////////////////////////////
RazerHydra::~RazerHydra()
{
  event::Events::DisconnectWorldUpdateBegin(this->updateConnection);

  this->stop = true;
  if (this->pollThread->joinable())
    this->pollThread->join();
}

/////////////////////////////////////////////////
void RazerHydra::Load(physics::WorldPtr _world, sdf::ElementPtr /*_sdf*/)
{
  // Find the Razer device.
  std::vector<std::string> devices;
  for (int i = 4; i < 5; ++i)
  {
    std::ostringstream stream;
    stream << "/sys/class/hidraw/hidraw" << i << "/device/uevent";
    std::ifstream fileIn(stream.str().c_str());
    if (fileIn.is_open())
    {
      std::string line;
      std::string device;
      while (std::getline(fileIn, line) && device.empty())
      {
        if (line.find("HID_NAME=Razer Razer Hydra") != std::string::npos)
        {
          device = "/dev/hidraw" + boost::lexical_cast<std::string>(i);
          devices.push_back(device);
          std::cout << "Found in " << i << std::endl;
        }
      }
      if (!device.empty())
        break;
    }
  }

  for (int i = 6; i < 7; ++i)
  {
    std::ostringstream stream;
    stream << "/sys/class/hidraw/hidraw" << i << "/device/uevent";
    std::ifstream fileIn(stream.str().c_str());
    if (fileIn.is_open())
    {
      std::string line;
      std::string device;
      while (std::getline(fileIn, line) && device.empty())
      {
        if (line.find("HID_NAME=Razer Razer Hydra") != std::string::npos)
        {
          device = "/dev/hidraw" + boost::lexical_cast<std::string>(i);
          devices.push_back(device);
          std::cout << "Found in " << i << std::endl;
        }
      }
      if (!device.empty())
        break;
    }
  }

  if (devices.empty())
  {
    gzerr << "Unable to find Razer device\n";
    return;
  }

  int counter = 0;
  for (const auto &device : devices)
  {
    int fd = open(device.c_str(), O_RDWR | O_NONBLOCK);
    if (fd < 0)
    {
      gzerr << "couldn't open hidraw device[" << device << "]\n";
      return;
    }

    this->controllers.push_back(std::unique_ptr<HydraController>(
      new HydraController(fd, _world->GetName(),
        "~/hydra" + std::to_string(counter++))));
  }

  std::cout << "Num Hydra's found: " << this->controllers.size() << std::endl;

  this->updateConnection = event::Events::ConnectWorldUpdateBegin(
      boost::bind(&RazerHydra::Update, this, _1));

  this->pollThread = new std::thread(&RazerHydra::Run, this);
}

/////////////////////////////////////////////////
void RazerHydra::Update(const common::UpdateInfo & /*_info*/)
{
  for (auto &controller : this->controllers)
    controller->Publish();
}

/////////////////////////////////////////////////
void RazerHydra::Run()
{
  float cornerHz = 2.5f;

  while (!this->stop)
  {
    for (auto &controller : this->controllers)
    {
      if (!controller->Poll(cornerHz))
        common::Time::NSleep(250000);
    }
  }
}

/////////////////////////////////////////////////
HydraController::HydraController(const int _fd, const std::string &_worldName,
  const std::string &_topic)
  : lastCycleStart(common::Time::GetWallTime()),
    fd(_fd),
    worldName(_worldName),
    topic(_topic)
{
  for (auto &v: this->analog)
    v = 0;

  for (auto &v: this->rawAnalog)
    v = 0;

  for (auto &v: this->rawButtons)
    v = 0;

  for (auto &v: this->rawQuat)
    v = 0;

  for (auto &v: this->rawPos)
    v = 0;

  for (auto &v: this->buttons)
    v = 0;

  // magic number for 50% mix at each step
  this->periodEstimate.SetFc(0.11, 1.0);

  this->periodEstimate.SetValue(0.004);

  uint8_t buf[256];
  struct hidraw_report_descriptor rptDesc;
  struct hidraw_devinfo info;

  memset(&rptDesc, 0x0, sizeof(rptDesc));
  memset(&info, 0x0, sizeof(info));
  memset(buf, 0x0, sizeof(buf));

  // Get Raw Name
  int res = ioctl(this->fd, HIDIOCGRAWNAME(256), buf);
  if (res < 0)
    gzerr << "Hydro ioctl error HIDIOCGRAWNAME: " << strerror(errno) << "\n";

  // set feature to start it streaming
  memset(buf, 0x0, sizeof(buf));
  buf[6] = 1;
  buf[8] = 4;
  buf[9] = 3;
  buf[89] = 6;

  int attempt;
  for (attempt = 0; attempt < 50; ++attempt)
  {
    res = ioctl(this->fd, HIDIOCSFEATURE(91), buf);
    if (res < 0)
    {
      gzerr << "Unable to start streaming. HIDIOCSFEATURE: "
            << strerror(errno) << "\n";
      common::Time::MSleep(500);
    }
    else
    {
      break;
    }
  }

  if (attempt >= 50)
  {
    gzerr << "Failed to load hydra\n";
    return;
  }

  this->node = transport::NodePtr(new transport::Node());
  this->node->Init(this->worldName);
  this->pub = this->node->Advertise<msgs::Hydra>(this->topic);
}

/////////////////////////////////////////////////
HydraController::~HydraController()
{
  if (this->fd >= 0)
  {
    uint8_t buf[256];
    memset(buf, 0, sizeof(buf));
    buf[6] = 1;
    buf[8] = 4;
    buf[89] = 5;

    if (ioctl(this->fd, HIDIOCSFEATURE(91), buf) < 0)
    {
      gzerr << "Unable to stop streaming. HIDIOCSFEATURE: "
            << strerror(errno) << "\n";
    }

    close(this->fd);
  }
}

/////////////////////////////////////////////////
bool HydraController::Poll(float _lowPassCornerHz)
{
  if (this->fd < 0)
  {
    gzerr << "hidraw device is not open, couldn't poll.\n";
    return false;
  }

  if (_lowPassCornerHz <= std::numeric_limits<float>::epsilon())
  {
    gzerr << "Corner frequency for low-pass filter must be greater than 0."
      << "Using a default value of 2.5Hz.\n";
    // Set a default value if the value is incorrect.
    _lowPassCornerHz = 2.5;
  }

  uint8_t buf[64];
  ssize_t nread = read(this->fd, buf, sizeof(buf));

  // No updates.
  if (nread <= 0)
    return false;


  static bool firstTime = true;

  // Update average read period
  if (!firstTime)
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
  this->rawPos[0] = *(reinterpret_cast<int16_t *>(buf+8));
  this->rawPos[1] = *(reinterpret_cast<int16_t *>(buf+10));
  this->rawPos[2] = *(reinterpret_cast<int16_t *>(buf+12));
  this->rawQuat[0] = *(reinterpret_cast<int16_t *>(buf+14));
  this->rawQuat[1] = *(reinterpret_cast<int16_t *>(buf+16));
  this->rawQuat[2] = *(reinterpret_cast<int16_t *>(buf+18));
  this->rawQuat[3] = *(reinterpret_cast<int16_t *>(buf+20));
  this->rawButtons[0] = buf[22] & 0x7f;
  this->rawAnalog[0] = *(reinterpret_cast<int16_t *>(buf+23));
  this->rawAnalog[1] = *(reinterpret_cast<int16_t *>(buf+25));
  this->rawAnalog[2] = buf[27];

  this->rawPos[3] = *(reinterpret_cast<int16_t *>(buf+30));
  this->rawPos[4] = *(reinterpret_cast<int16_t *>(buf+32));
  this->rawPos[5] = *(reinterpret_cast<int16_t *>(buf+34));
  this->rawQuat[4] = *(reinterpret_cast<int16_t *>(buf+36));
  this->rawQuat[5] = *(reinterpret_cast<int16_t *>(buf+38));
  this->rawQuat[6] = *(reinterpret_cast<int16_t *>(buf+40));
  this->rawQuat[7] = *(reinterpret_cast<int16_t *>(buf+42));
  this->rawButtons[1] = buf[44] & 0x7f;
  this->rawAnalog[3] = *(reinterpret_cast<int16_t *>(buf+45));
  this->rawAnalog[4] = *(reinterpret_cast<int16_t *>(buf+47));
  this->rawAnalog[5] = buf[49];

  std::lock_guard<std::mutex> lock(this->mutex);
  // Put the raw position and orientation into Gazebo coordinate frame
  for (int i = 0; i < 2; ++i)
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
  for (int i = 0; i < 2; ++i)
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

/////////////////////////////////////////////////
std::array<math::Vector3, 2> HydraController::GetPos()
{
  std::lock_guard<std::mutex> lock(this->mutex);
  return this->pos;
}

/////////////////////////////////////////////////
std::array<math::Quaternion, 2> HydraController::GetQuat()
{
  std::lock_guard<std::mutex> lock(this->mutex);
  return this->quat;
}

/////////////////////////////////////////////////
std::array<float, 6> HydraController::GetAnalog()
{
  std::lock_guard<std::mutex> lock(this->mutex);
  return this->analog;
}

/////////////////////////////////////////////////
std::array<uint8_t, 14> HydraController::GetButtons()
{
  std::lock_guard<std::mutex> lock(this->mutex);
  return this->buttons;
}

/////////////////////////////////////////////////
void HydraController::Publish()
{
  std::lock_guard<std::mutex> lock(this->mutex);

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