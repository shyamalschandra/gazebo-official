/*
 * Copyright 2012 Open Source Robotics Foundation
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

#include "gazebo/common/Exception.hh"
#include "gazebo/common/Image.hh"

#include "gazebo/physics/World.hh"

#include "gazebo/transport/transport.hh"
#include "gazebo/msgs/msgs.hh"

#include "gazebo/rendering/RenderEngine.hh"
#include "gazebo/rendering/Camera.hh"
#include "gazebo/rendering/Scene.hh"
#include "gazebo/rendering/Rendering.hh"

#include "gazebo/sensors/SensorFactory.hh"
#include "gazebo/sensors/MultiCameraSensor.hh"

using namespace gazebo;
using namespace sensors;

GZ_REGISTER_STATIC_SENSOR("multicamera", MultiCameraSensor)

//////////////////////////////////////////////////
MultiCameraSensor::MultiCameraSensor()
    : Sensor(sensors::IMAGE)
{
}

//////////////////////////////////////////////////
MultiCameraSensor::~MultiCameraSensor()
{
}

//////////////////////////////////////////////////
std::string MultiCameraSensor::GetTopic() const
{
  std::string topic = Sensor::GetTopic();

  // Create a topic name if one has not been specified.
  if (topic.empty())
  {
    topic = "~/";
    topic += this->parentName + "/" + this->GetName() + "/images";
    boost::replace_all(topic, "::", "/");
  }

  return topic;
}

//////////////////////////////////////////////////
void MultiCameraSensor::Load(const std::string &_worldName)
{
  Sensor::Load(_worldName);

  // Create the publisher of image data.
  this->imagePub = this->node->Advertise<msgs::ImagesStamped>(this->GetTopic());
}

//////////////////////////////////////////////////
void MultiCameraSensor::Init()
{
  if (rendering::RenderEngine::Instance()->GetRenderPathType() ==
      rendering::RenderEngine::NONE)
  {
    gzerr << "Unable to create MultiCameraSensor. Rendering is disabled.\n";
    return;
  }

  std::string worldName = this->world->GetName();

  if (worldName.empty())
  {
    gzerr << "No world name\n";
    return;
  }

  rendering::ScenePtr scene = rendering::get_scene(worldName);

  if (!scene)
  {
    scene = rendering::create_scene(worldName, false);

    // This usually means rendering is not available
    if (!scene)
    {
      gzerr << "Unable to create MultiCameraSensor.\n";
      return;
    }
  }

  // Create and initialize all the cameras
  sdf::ElementPtr cameraSdf = this->sdf->GetElement("camera");
  while (cameraSdf)
  {
    rendering::CameraPtr camera = scene->CreateCamera(
          cameraSdf->GetValueString("name"), false);

    if (!camera)
    {
      gzthrow("Unable to create multicamera sensor[" +
              cameraSdf->GetValueString("name"));
      return;
    }

    camera->SetCaptureData(true);
    camera->Load(cameraSdf);

    // Do some sanity checks
    if (camera->GetImageWidth() == 0 || camera->GetImageHeight() == 0)
      gzthrow("Image has zero size");

    camera->Init();
    camera->CreateRenderTexture(camera->GetName() + "_RttTex");

    math::Pose cameraPose = this->pose;
    if (cameraSdf->HasElement("pose"))
      cameraPose += cameraSdf->GetValuePose("pose");
    camera->SetWorldPose(cameraPose);
    camera->AttachToVisual(this->parentName, true);

    this->cameras.push_back(camera);

    cameraSdf = cameraSdf->GetNextElement("camera");
  }

  Sensor::Init();
}

//////////////////////////////////////////////////
void MultiCameraSensor::Fini()
{
  Sensor::Fini();

  for (std::vector<rendering::CameraPtr>::iterator iter =
      this->cameras.begin(); iter != this->cameras.end(); ++iter)
  {
    (*iter)->Fini();
  }
  this->cameras.clear();
}

//////////////////////////////////////////////////
rendering::CameraPtr MultiCameraSensor::GetCamera(unsigned int _index) const
{
  if (_index < this->cameras.size())
    return this->cameras[_index];
  else
    gzthrow("camera index out of range. Valid range[0.." +
        boost::lexical_cast<std::string>(this->cameras.size()-1));
}

//////////////////////////////////////////////////
void MultiCameraSensor::UpdateImpl(bool /*_force*/)
{
  if (this->cameras.size() == 0)
    return;

  bool publish = this->imagePub->HasConnections();

  this->lastMeasurementTime = this->world->GetSimTime();

  msgs::ImagesStamped msg;
  msgs::Set(msg.mutable_time(), this->lastMeasurementTime);

  // Update all the cameras
  for (std::vector<rendering::CameraPtr>::iterator iter = this->cameras.begin();
       iter != this->cameras.end(); ++iter)
  {
    (*iter)->Render();
    (*iter)->PostRender();

    if (publish)
    {
      msgs::Image *image = msg.add_image();
      image->set_width((*iter)->GetImageWidth());
      image->set_height((*iter)->GetImageHeight());
      image->set_pixel_format(common::Image::ConvertPixelFormat(
            (*iter)->GetImageFormat()));
      image->set_step((*iter)->GetImageWidth() * (*iter)->GetImageDepth());
      image->set_data((*iter)->GetImageData(0),
          image->width() * (*iter)->GetImageDepth() * image->height());
    }
  }

  if (publish)
    this->imagePub->Publish(msg);
}

//////////////////////////////////////////////////
unsigned int MultiCameraSensor::GetImageWidth(unsigned int _index) const
{
  return this->GetCamera(_index)->GetImageWidth();
}

//////////////////////////////////////////////////
unsigned int MultiCameraSensor::GetImageHeight(unsigned int _index) const
{
  return this->GetCamera(_index)->GetImageHeight();
}

//////////////////////////////////////////////////
const unsigned char *MultiCameraSensor::GetImageData(unsigned int _index)
{
  return this->GetCamera(_index)->GetImageData(0);
}

//////////////////////////////////////////////////
bool MultiCameraSensor::SaveFrame(const std::vector<std::string> &_filenames)
{
  this->SetActive(true);

  if (_filenames.size() != this->cameras.size())
  {
    gzerr << "Filename count[" << _filenames.size() << "] does not match "
          << "camera count[" << this->cameras.size() << "]\n";
    return false;
  }

  bool result = true;

  std::vector<rendering::CameraPtr>::iterator citer = this->cameras.begin();
  for (std::vector<std::string>::const_iterator fiter = _filenames.begin();
       fiter != _filenames.end(); ++fiter, ++citer)
  {
    result &= (*citer)->SaveFrame(*fiter);
  }

  return result;
}
