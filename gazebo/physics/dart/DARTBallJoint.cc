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

#include "gazebo/gazebo_config.h"
#include "gazebo/common/Console.hh"
#include "gazebo/physics/dart/DARTBallJoint.hh"

using namespace gazebo;
using namespace physics;

//////////////////////////////////////////////////
DARTBallJoint::DARTBallJoint(BasePtr _parent)
: BallJoint<DARTJoint>(_parent)
{
}

//////////////////////////////////////////////////
DARTBallJoint::~DARTBallJoint()
{
}

//////////////////////////////////////////////////
math::Vector3 DARTBallJoint::GetAnchor(int /*_index*/) const
{
  gzwarn << "Not implemented!\n";

  return math::Vector3(0, 0, 0);
}


//////////////////////////////////////////////////
void DARTBallJoint::SetAnchor(int /*_index*/, const math::Vector3& /*_anchor*/)
{
  gzwarn << "Not implemented!\n";
}

//////////////////////////////////////////////////
void DARTBallJoint::SetDamping(int /*_index*/, double /*_damping*/)
{
  gzwarn << "Not implemented!\n";
}
