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
/* Desc: Bullet motion state class.
 * Author: Nate Koenig
 * Date: 25 May 2009
 */

#include "gazebo/physics/World.hh"
#include "gazebo/physics/Link.hh"
#include "gazebo/physics/bullet/BulletPhysics.hh"
#include "gazebo/physics/bullet/BulletMotionState.hh"
#include "gazebo/physics/bullet/BulletTypes.hh"

using namespace gazebo;
using namespace physics;

//////////////////////////////////////////////////
BulletMotionState::BulletMotionState(LinkPtr _link)
  : btMotionState()
{
  this->link = _link;
}

//////////////////////////////////////////////////
BulletMotionState::~BulletMotionState()
{
}

//////////////////////////////////////////////////
void BulletMotionState::getWorldTransform(btTransform &_cogWorldTrans) const
{
  _cogWorldTrans =
    BulletTypes::ConvertPose(this->link->GetWorldInertialPose());
}

//////////////////////////////////////////////////
void BulletMotionState::setWorldTransform(const btTransform &_cogWorldTrans)
{
  math::Pose pose = BulletTypes::ConvertPose(_cogWorldTrans);

  // transform pose from cg location to link location
  // cg: pose of cg in link frame, so -cg is transform from cg to
  //     link defined in cg frame.
  // pose: transform from world origin to cg in inertial frame.
  // -cg + pose:  transform from world origin to link frame in inertial frame.
  math::Pose cg = this->link->GetInertial()->GetPose();
  pose = -cg + pose;

  // The second argument is set to false to prevent Entity.cc from propagating
  // the pose change all the way back to bullet.
  // Using the dirtyPose mechanism employed by ODE.
  // \TODO: In attempt to ensure locking between Joint::SetAngle(SetPosition)
  // and physics updates, use the same dirtyPose mechanism as ODE. Unfortunately
  // Joint::SetAngle(SetPosition) is still broken, probably due to issue #1194,
  // as indicated in set_pose_loop.cc integration test.
  // \TODO: this is an ugly line of code. It's like this for speed.
  this->link->SetDirtyPose(pose);
  this->link->GetWorld()->dirtyPoses.push_back(this->link.get());

  // below is inefficient as we end up double caching for some joints
  // should consider adding a "dirty" flag.
  // or trying doing this during BulletPhysics::InternalTickCallback(...)
  Joint_V parentJoints = this->link->GetParentJoints();
  for (unsigned int j = 0; j < parentJoints.size(); ++j)
  {
    JointPtr joint = parentJoints[j];
    joint->CacheForceTorque();
  }
  Joint_V childJoints = this->link->GetChildJoints();
  for (unsigned int j = 0; j < childJoints.size(); ++j)
  {
    JointPtr joint = childJoints[j];
    joint->CacheForceTorque();
  }
}
