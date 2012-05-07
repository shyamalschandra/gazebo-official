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
/* Desc: A 4-wheeled vehicle
 * Author: Nate Koenig
 */

#ifndef __GAZEBO_VEHICLE_PLUGIN_HH__
#define __GAZEBO_VEHICLE_PLUGIN_HH__

#include <string>

#include "common/common.h"
#include "physics/physics.h"
#include "gazebo.hh"

namespace gazebo
{
  class VehiclePlugin : public ModelPlugin
  {
    /// \brief Constructor
    public: VehiclePlugin();

    public: virtual void Load(physics::ModelPtr _model, sdf::ElementPtr _sdf);
    public: virtual void Init();

    private: void OnUpdate();
    private: std::vector<event::ConnectionPtr> connections;

    private: LinkPtr chassis;
    private: std::vector<LinkPtr> wheels;
  };
}
#endif
