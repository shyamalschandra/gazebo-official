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

#include "gazebo/physics/Link.hh"
#include "gazebo/physics/dart/DARTHinge2Joint.hh"

using namespace gazebo;
using namespace physics;

//////////////////////////////////////////////////
DARTHinge2Joint::DARTHinge2Joint(BasePtr _parent)
    : Hinge2Joint<DARTJoint>(_parent),
      dartUniveralJoint(new dart::dynamics::UniversalJoint())
{
  this->dartJoint = dartUniveralJoint;
}

//////////////////////////////////////////////////
DARTHinge2Joint::~DARTHinge2Joint()
{
  delete dartUniveralJoint;
  this->dartJoint = NULL;
}

//////////////////////////////////////////////////
void DARTHinge2Joint::Load(sdf::ElementPtr _sdf)
{
  Hinge2Joint<DARTJoint>::Load(_sdf);
}

//////////////////////////////////////////////////
void DARTHinge2Joint::Init()
{
  Hinge2Joint<DARTJoint>::Init();
}

//////////////////////////////////////////////////
math::Vector3 DARTHinge2Joint::GetAnchor(int /*_index*/) const
{
  Eigen::Isometry3d T = this->dartChildBodyNode->getWorldTransform() *
                        this->dartJoint->getTransformFromChildBodyNode();
  Eigen::Vector3d worldOrigin = T.translation();

  return DARTTypes::ConvVec3(worldOrigin);
}

//////////////////////////////////////////////////
void DARTHinge2Joint::SetAxis(int _index, const math::Vector3& _axis)
{
  Eigen::Vector3d dartAxis = DARTTypes::ConvVec3(_axis);

  if (_index == 0)
  {
    //----------------------------------------------------------------------------
    // TODO: Issue #494
    // See: https://bitbucket.org/osrf/gazebo/issue/494/joint-axis-reference-frame-doesnt-match
    Eigen::Isometry3d dartTransfJointLeftToParentLink
        = this->dartJoint->getTransformFromParentBodyNode().inverse();
    dartAxis = dartTransfJointLeftToParentLink.linear() * dartAxis;
    //----------------------------------------------------------------------------

    this->dartUniveralJoint->setAxis1(dartAxis);
  }
  else if (_index == 1)
  {
    //----------------------------------------------------------------------------
    // TODO: Issue #494
    // See: https://bitbucket.org/osrf/gazebo/issue/494/joint-axis-reference-frame-doesnt-match
    Eigen::Isometry3d dartTransfJointLeftToParentLink
        = this->dartJoint->getTransformFromParentBodyNode().inverse();
    dartAxis = dartTransfJointLeftToParentLink.linear() * dartAxis;
    //----------------------------------------------------------------------------

    this->dartUniveralJoint->setAxis2(dartAxis);
  }
  else
  {
    gzerr << "Invalid index[" << _index << "]\n";
  }
}

//////////////////////////////////////////////////
math::Vector3 DARTHinge2Joint::GetGlobalAxis(int _index) const
{
  Eigen::Vector3d globalAxis = Eigen::Vector3d::UnitX();

  if (_index == 0)
  {
    Eigen::Isometry3d T = this->dartChildBodyNode->getWorldTransform() *
                          this->dartJoint->getLocalTransform().inverse() *
                          this->dartJoint->getTransformFromParentBodyNode();
    Eigen::Vector3d axis = this->dartUniveralJoint->getAxis1();

    globalAxis = T.linear() * axis;
  }
  else if (_index == 1)
  {
    Eigen::Isometry3d T = this->dartChildBodyNode->getWorldTransform() *
                          this->dartJoint->getTransformFromChildBodyNode();
    Eigen::Vector3d axis = this->dartUniveralJoint->getAxis2();

    globalAxis = T.linear() * axis;
  }
  else
  {
    gzerr << "Invalid index[" << _index << "]\n";
  }

  // TODO: Issue #494
  // See: https://bitbucket.org/osrf/gazebo/issue/494/joint-axis-reference-frame-doesnt-match
  return DARTTypes::ConvVec3(globalAxis);
}

//////////////////////////////////////////////////
math::Angle DARTHinge2Joint::GetAngleImpl(int _index) const
{
  math::Angle result;

  if (_index == 0)
  {
    double radianAngle = this->dartJoint->getGenCoord(0)->get_q();
    result.SetFromRadian(radianAngle);
  }
  else if (_index == 1)
  {
    double radianAngle = this->dartJoint->getGenCoord(1)->get_q();
    result.SetFromRadian(radianAngle);
  }
  else
  {
    gzerr << "Invalid index[" << _index << "]\n";
  }

  return result;
}

//////////////////////////////////////////////////
double DARTHinge2Joint::GetVelocity(int _index) const
{
  double result = 0.0;

  if (_index == 0)
    result = this->dartJoint->getGenCoord(0)->get_dq();
  else if (_index == 1)
    result = this->dartJoint->getGenCoord(1)->get_dq();
  else
    gzerr << "Invalid index[" << _index << "]\n";

  return result;
}

//////////////////////////////////////////////////
void DARTHinge2Joint::SetVelocity(int _index, double _vel)
{
  if (_index == 0)
    this->dartJoint->getGenCoord(0)->set_dq(_vel);
  else if (_index == 1)
    this->dartJoint->getGenCoord(1)->set_dq(_vel);
  else
    gzerr << "Invalid index[" << _index << "]\n";
}

//////////////////////////////////////////////////
double DARTHinge2Joint::GetMaxForce(int _index)
{
  double result = 0.0;

  if (_index == 0)
    result = this->dartJoint->getGenCoord(0)->get_tauMax();
  else if (_index == 1)
    result = this->dartJoint->getGenCoord(1)->get_tauMax();
  else
    gzerr << "Invalid index[" << _index << "]\n";

  return result;
}

//////////////////////////////////////////////////
void DARTHinge2Joint::SetMaxForce(int _index, double _force)
{
  if (_index == 0)
    this->dartJoint->getGenCoord(0)->set_tauMax(_force);
  else if (_index == 1)
    this->dartJoint->getGenCoord(1)->set_tauMax(_force);
  else
    gzerr << "Invalid index[" << _index << "]\n";
}

//////////////////////////////////////////////////
void DARTHinge2Joint::SetForceImpl(int _index, double _effort)
{
  if (_index == 0)
    this->dartJoint->getGenCoord(0)->set_tau(_effort);
  else if (_index == 1)
    this->dartJoint->getGenCoord(1)->set_tau(_effort);
  else
    gzerr << "Invalid index[" << _index << "]\n";
}
