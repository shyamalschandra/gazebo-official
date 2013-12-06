/*
 * Copyright (C) 2012-2013 Open Source Robotics Foundation
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

#ifndef _GAZEBO_ATLAS_PLUGIN_HH_
#define _GAZEBO_ATLAS_PLUGIN_HH_

#include <kdl/chain.hpp>
#include <kdl/chainfksolver.hpp>
#include <kdl/chainfksolverpos_recursive.hpp>
#include <kdl/frames_io.hpp>


#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

#include "gazebo/physics/physics.hh"
#include "gazebo/msgs/msgs.hh"
#include "gazebo/transport/TransportTypes.hh"
#include "gazebo/common/Time.hh"
#include "gazebo/common/Plugin.hh"
#include "gazebo/common/Events.hh"

namespace gazebo
{
  class AtlasPlugin : public ModelPlugin
  {
    /// \brief Constructor
    public: AtlasPlugin();

    /// \brief Destructor
    public: virtual ~AtlasPlugin();

    /// \brief Load the controller
    public: void Load(physics::ModelPtr _parent, sdf::ElementPtr _sdf);

    /// \brief Update the controller
    /// \param[in] _info Update information provided by the server.
    private: void Update(const common::UpdateInfo &_info);

    private: void OnHydra(ConstHydraPtr &_msg);

    private: void SetRightFingers(double _angle);
    private: void SetLeftFingers(double _angle);

    private: void Restart();

    private: physics::WorldPtr world;
    private: physics::ModelPtr model;
    private: physics::ModelPtr boxModel;

    private: transport::NodePtr node;
    private: transport::SubscriberPtr hydraSub;

    private: boost::mutex update_mutex;

    // Pointer to the update event connection
    private: event::ConnectionPtr updateConnection;

    private: physics::JointPtr pinJoint;

    private: physics::LinkPtr rightHand;

    private: physics::JointControllerPtr jointController;

    private: math::Vector3 goalPos;

    private: KDL::ChainFkSolverPos_recursive *rFkSolver;
    private: KDL::Chain rChain;
    private: KDL::JntArray *rJointpositions;

    private: bool rightBumper, leftBumper;

    private: physics::ModelPtr rightModel, leftModel;

    private: math::Pose rightStartPose;
    private: math::Pose leftStartPose;

    private: math::Pose basePoseRight;
    private: math::Pose basePoseLeft;
    private: math::Pose resetPoseRight;
    private: math::Pose resetPoseLeft;
  };
}
#endif
