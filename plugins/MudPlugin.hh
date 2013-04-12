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
#ifndef _GAZEBO_MODEL_TRAJECTORY_TEST_PLUGIN_HH_
#define _GAZEBO_MODEL_TRAJECTORY_TEST_PLUGIN_HH_

#include "gazebo/common/common.hh"
#include "gazebo/physics/physics.hh"
#include "gazebo/transport/TransportTypes.hh"
#include "gazebo/gazebo.hh"

namespace gazebo
{
  class MudPlugin : public ModelPlugin
  {
    public: MudPlugin();
    public: virtual void Load(physics::ModelPtr _model, sdf::ElementPtr _sdf);
    public: virtual void Init();

    private: void OnContact(ConstContactsPtr &_msg);
    private: void OnUpdate();

    private: transport::NodePtr node;
    private: transport::SubscriberPtr contactSub;

    private: event::ConnectionPtr updateConnection;

    private: physics::WorldPtr world;
    private: physics::ModelPtr model;
    private: std::string modelName;
    private: physics::LinkPtr link;

    /// \brief Name of contact sensor relative to model, scoped with '/',
    private: std::string contactSensorName;

    /// \brief Flag that indicates whether mud is enabled or disabled.
    private: bool mudEnabled;

    /// \brief Hysteresis parameter used for switching mudEnabled,
    ///        defaults to 0.2.
    private: double hysteresis;

    /// \brief Mutex to protect reads and writes.
    private: mutable boost::mutex mutex;

    /// \brief Store newest contacts message.
    private: msgs::Contacts newestContactsMsg;

    /// \brief Flag to indicate new contact message.
    private: bool newMsg;

    /// \brief Dynamically created joint for simulating mud forces.
    private: physics::JointPtr joint;

    /// \brief Pointer to link currently targeted by mud joint.
    private: physics::LinkPtr targetLink;
  };
}
#endif
