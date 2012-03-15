/*
 * Copyright 2011 Nate Koenig & Andrew Howard
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
/* Desc: Ray proximity sensor
 * Author: Carle Cote
 * Date: 23 february 2004
*/

#include "physics/World.hh"

#include "common/Exception.hh"
#include "common/Events.hh"

#include "transport/transport.h"

#include "rendering/Scene.hh"
#include "rendering/Rendering.hh"

#include "sensors/SensorFactory.hh"
#include "sensors/VisualLaserSensor.hh"
#include "rendering/VisualLaser.hh"

using namespace gazebo;
using namespace sensors;

GZ_REGISTER_STATIC_SENSOR("visual_laser", VisualLaserSensor)

//////////////////////////////////////////////////
VisualLaserSensor::VisualLaserSensor()
    : Sensor()
{
  this->mutex = new boost::mutex();
  this->active = false;
  this->node = transport::NodePtr(new transport::Node());
}

//////////////////////////////////////////////////
VisualLaserSensor::~VisualLaserSensor()
{
  delete this->mutex;
}

//////////////////////////////////////////////////
void VisualLaserSensor::Load(const std::string &_worldName, sdf::ElementPtr &_sdf)
{
  Sensor::Load(_worldName, _sdf);
}

//////////////////////////////////////////////////
void VisualLaserSensor::Load(const std::string &_worldName)
{
  Sensor::Load(_worldName);
  this->poseSub = this->node->Subscribe("~/pose",
                                        &VisualLaserSensor::OnPose, this);
  
  this->rayElem = this->sdf->GetElement("ray");
  this->scanElem = this->rayElem->GetElement("scan");
  this->horzElem = this->scanElem->GetElement("horizontal");
  this->rangeElem = this->rayElem->GetElement("range");

  if (this->scanElem->HasElement("vertical"))
    this->vertElem = this->scanElem->GetElement("vertical");

  width_1st = this->GetRayCount();
  height_1st = this->GetVerticalRayCount();

  if (width_1st == 0 || height_1st == 0)
  {
    gzthrow("VisualLaserSensor: Image has 0 size!");
  }
  
  width_2nd = this->GetRangeCount();
  height_2nd = this->GetVerticalRangeCount();

  ratio_2nd = width_2nd / height_2nd;


  near = this->GetRangeMin();
  far = this->GetRangeMax();

  hfov = this->GetAngleMax().GetAsRadian() - this->GetAngleMin().GetAsRadian();
  vfov = this->GetVerticalAngleMax().GetAsRadian() - this->GetVerticalAngleMin().GetAsRadian();

  ratio_1st = tan(hfov/2.0) / tan(vfov/2.0);

  if (height_1st < width_1st) 
    height_1st = width_1st / ratio_1st;
  else
    width_1st = height_1st * ratio_1st;

  this->cameraElem.reset(new sdf::Element);
  sdf::initFile("sdf/camera.sdf", this->cameraElem);

  sdf::ElementPtr ptr = this->cameraElem->GetOrCreateElement("horizontal_fov");
  ptr->GetAttribute("angle")->Set(hfov);

  ptr = this->cameraElem->GetOrCreateElement("image");
  ptr->GetAttribute("width")->Set(width_1st);
  ptr->GetAttribute("height")->Set(height_1st);
  ptr->GetAttribute("format")->Set("R8G8B8");
  
  ptr = this->cameraElem->GetOrCreateElement("clip");
  ptr->GetAttribute("near")->Set(near);
  ptr->GetAttribute("far")->Set(far);
}

//////////////////////////////////////////////////
void VisualLaserSensor::Init()
{
  std::string worldName = this->world->GetName();

  if (!worldName.empty())
  {
    this->scene = rendering::get_scene(worldName);

    if (!this->scene)
      this->scene = rendering::create_scene(worldName, false);

    this->laserCam = this->scene->CreateVisualLaser(
        this->sdf->GetValueString("name"), false);

    if (!this->laserCam)
    {
      gzerr << "Unable to create visual laser sensor\n";
      return;
    }
    this->laserCam->SetCaptureData(true);

    this->laserCam->Load(this->cameraElem);

    this->laserCam->Init();
    this->laserCam->SetRangeCount(width_2nd, height_2nd);
    this->laserCam->SetParentSensor(this);
    this->laserCam->CreateLaserTexture(this->GetName() + "_RttTex_Laser");
    this->laserCam->CreateRenderTexture(this->GetName() + "_RttTex_Image");
    this->laserCam->SetWorldPose(this->pose);
    this->laserCam->AttachToVisual(this->parentName, true);
  }
  else
    gzerr << "No world name\n";

  Sensor::Init();
}

//////////////////////////////////////////////////
void VisualLaserSensor::Fini()
{
  Sensor::Fini();
  this->laserCam->Fini();
  this->laserCam.reset();
  this->scene.reset();
}

//////////////////////////////////////////////////
double VisualLaserSensor::GetHFOV()
{
  return this->hfov;
}

//////////////////////////////////////////////////
double VisualLaserSensor::GetVFOV()
{
  return this->vfov;
}

//////////////////////////////////////////////////
double VisualLaserSensor::Get1stRatio()
{
  return this->ratio_1st;
}

//////////////////////////////////////////////////
double VisualLaserSensor::Get2ndRatio()
{
  return this->ratio_2nd;
}

//////////////////////////////////////////////////
math::Angle VisualLaserSensor::GetAngleMin() const
{
  return this->horzElem->GetValueDouble("min_angle");
}

//////////////////////////////////////////////////
math::Angle VisualLaserSensor::GetAngleMax() const
{
  return this->horzElem->GetValueDouble("max_angle");
}

//////////////////////////////////////////////////
double VisualLaserSensor::GetRangeMin() const
{
  return this->rangeElem->GetValueDouble("min");
}

//////////////////////////////////////////////////
double VisualLaserSensor::GetRangeMax() const
{
  return this->rangeElem->GetValueDouble("max");
}

/////////////////////////////////////////////////
double VisualLaserSensor::GetAngleResolution() const
{
  return (this->GetAngleMax() - this->GetAngleMin()).GetAsRadian() /
   (this->GetRangeCount()-1);
}

//////////////////////////////////////////////////
double VisualLaserSensor::GetRangeResolution() const
{
  return this->rangeElem->GetValueDouble("resolution");
}

//////////////////////////////////////////////////
int VisualLaserSensor::GetRayCount() const
{
  return this->horzElem->GetValueUInt("samples");
}

//////////////////////////////////////////////////
int VisualLaserSensor::GetRangeCount() const
{
  return this->GetRayCount() *
        this->horzElem->GetValueDouble("resolution");
}

//////////////////////////////////////////////////
int VisualLaserSensor::GetVerticalRayCount() const
{
  if (this->scanElem->HasElement("vertical"))
    return this->vertElem->GetValueUInt("samples");
  else
    return 1;
}

//////////////////////////////////////////////////
int VisualLaserSensor::GetVerticalRangeCount() const
{
  if (this->scanElem->HasElement("vertical"))
  {
    int rows =  (this->GetVerticalRayCount() *
          this->vertElem->GetValueDouble("resolution"));
    if (rows > 1)
      return rows;
    else
      return 1;
  }
  else
    return 1;
}

//////////////////////////////////////////////////
math::Angle VisualLaserSensor::GetVerticalAngleMin() const
{
  if (this->scanElem->HasElement("vertical"))
    return this->vertElem->GetValueDouble("min_angle");
  else
    return math::Angle(0);
}

//////////////////////////////////////////////////
math::Angle VisualLaserSensor::GetVerticalAngleMax() const
{
  if (this->scanElem->HasElement("vertical"))
    return this->vertElem->GetValueDouble("max_angle");
  else
    return math::Angle(0);
}

//////////////////////////////////////////////////
void VisualLaserSensor::GetRanges(std::vector<double> &_ranges)
{
  /*boost::mutex::scoped_lock(*this->mutex);
  _ranges.resize(this->laserMsg.ranges_size());
  memcpy(&_ranges[0], this->laserMsg.ranges().data(),
         sizeof(_ranges[0]) * this->laserMsg.ranges_size());*/
}

//////////////////////////////////////////////////
double VisualLaserSensor::GetRange(int _index)
{
  if (_index < 0 || _index > this->laserMsg.ranges_size())
  {
    gzerr << "Invalid range index[" << _index << "]\n";
    return 0.0;
  }

  boost::mutex::scoped_lock(*this->mutex);
  return this->laserMsg.ranges(_index);
}

//////////////////////////////////////////////////
double VisualLaserSensor::GetRetro(int index)
{
  //boost::mutex::scoped_lock(*this->mutex);
  //return this->laserShape->GetRetro(index);
}

//////////////////////////////////////////////////
int VisualLaserSensor::GetFiducial(int index)
{
  //boost::mutex::scoped_lock(*this->mutex);
  //return this->laserShape->GetFiducial(index);
}

//////////////////////////////////////////////////
void VisualLaserSensor::UpdateImpl(bool /*_force*/)
{
  //this->mutex->lock();

  // Sensor::Update(force);
  if (this->laserCam)\
  {
    this->laserCam->Render();
    this->laserCam->PostRender();
    this->lastUpdateTime = this->world->GetSimTime();
  }
  
  //this->mutex->unlock();

  //if (this->scanPub)
  //  this->scanPub->Publish(this->laserMsg);
}

void VisualLaserSensor::OnPose(ConstPosePtr &/*_msg*/)
{
}


