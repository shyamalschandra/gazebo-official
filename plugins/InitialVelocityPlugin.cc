/*
 * Copyright (C) 2012-2014 Open Source Robotics Foundation
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

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include "gazebo/common/Assert.hh"
#include "gazebo/physics/physics.hh"
#include "gazebo/sensors/SensorManager.hh"
#include "gazebo/sensors/ContactSensor.hh"
#include "gazebo/transport/transport.hh"
#include "plugins/InitialVelocityPlugin.hh"

using namespace gazebo;

GZ_REGISTER_MODEL_PLUGIN(InitialVelocityPlugin)

/////////////////////////////////////////////////
InitialVelocityPlugin::InitialVelocityPlugin()
{
}

/////////////////////////////////////////////////
InitialVelocityPlugin::~InitialVelocityPlugin()
{
}

/////////////////////////////////////////////////
void InitialVelocityPlugin::Load(physics::ModelPtr _model,
                     sdf::ElementPtr _sdf)
{
  GZ_ASSERT(_model, "_model pointer is NULL");
  this->model = _model;
  this->sdf = _sdf;
  this->Reset();
}

/////////////////////////////////////////////////
void InitialVelocityPlugin::Reset()
{
  if (this->sdf->HasElement("linear"))
  {
    math::Vector3 linear = this->sdf->Get<math::Vector3>("linear");
    this->model->SetLinearVel(linear);
  }
  if (this->sdf->HasElement("angular"))
  {
    math::Vector3 angular = this->sdf->Get<math::Vector3>("angular");
    this->model->SetAngularVel(angular);
  }
}
