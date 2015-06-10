/*
 * Copyright (C) 2015 Open Source Robotics Foundation
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

#ifndef _GAZEBO_TRANSPORTER_PLUGIN_HH_
#define _GAZEBO_TRANSPORTER_PLUGIN_HH_

#include <sdf/sdf.hh>

#include <gazebo/transport/Node.hh>
#include <gazebo/transport/Subscriber.hh>

#include <gazebo/common/Plugin.hh>
#include <gazebo/util/system.hh>
#include <gazebo/msgs/msgs.hh>

namespace gazebo
{
  class GAZEBO_VISIBLE TransporterPlugin : public WorldPlugin
  {
    /// \brief Constructor.
    public: TransporterPlugin();

    /// \brief Destructor.
    public: ~TransporterPlugin();

    /// \brief Load the plugin.
    /// \param[in] _world Pointer to world
    /// \param[in] _sdf Pointer to the SDF configuration.
    public: virtual void Load(physics::WorldPtr _world, sdf::ElementPtr _sdf);

    private: void Update();

    private: void OnActivation(ConstGzStringPtr &_msg);

    /// \brief World pointer.
    private: physics::WorldPtr world;

    /// \brief SDF pointer.
    private: sdf::ElementPtr sdf;

    private: class Pad
             {
               public: std::string name;
               public: std::string dest;

               public: math::Pose incomingPose;
               public: math::Pose outgoingPose;

               public: math::Vector3 incomingBox;
               public: math::Vector3 outgoingBox;

               /// \brief True if the pad should automatically teleport.
               /// False will cause the pad to wait for an activation
               /// signal. See this plugin's <activation_topic> xml element.
               public: bool autoActivation;

               /// \brief This flag is used for manual activation of a pad.
               /// It is set to true when a string message that contains
               /// the name of the pad is sent over the activation topic.
               public: bool activated;
             };


    private: std::map<std::string, Pad*> pads;

    /// \brief Pointer to the update event connection
    private: event::ConnectionPtr updateConnection;

    private: transport::NodePtr node;
    private: transport::SubscriberPtr activationSub;

    private: boost::mutex padMutex;
  };
}
#endif
