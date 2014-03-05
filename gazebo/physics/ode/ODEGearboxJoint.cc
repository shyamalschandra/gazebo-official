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

#include <boost/bind.hpp>

#include "gazebo/gazebo_config.h"
#include "gazebo/common/Console.hh"

#include "gazebo/physics/Model.hh"
#include "gazebo/physics/Link.hh"
#include "gazebo/physics/ode/ODELink.hh"
#include "gazebo/physics/ode/ODEGearboxJoint.hh"

using namespace gazebo;
using namespace physics;

//////////////////////////////////////////////////
ODEGearboxJoint::ODEGearboxJoint(dWorldID _worldId, BasePtr _parent)
    : GearboxJoint<ODEJoint>(_parent)
{
  this->jointId = dJointCreateGearbox(_worldId, NULL);
}

//////////////////////////////////////////////////
ODEGearboxJoint::~ODEGearboxJoint()
{
}

//////////////////////////////////////////////////
void ODEGearboxJoint::Init()
{
  Joint::Init();
  LinkPtr link = this->model->GetLink(this->referenceBody);
  if (link)
    this->SetReferenceBody(link);
  else
    gzerr << "reference Link has not been set yet.\n";
}

//////////////////////////////////////////////////
void ODEGearboxJoint::Load(sdf::ElementPtr _sdf)
{
  GearboxJoint<ODEJoint>::Load(_sdf);

  this->SetGearRatio(this->gearRatio);
}

//////////////////////////////////////////////////
void ODEGearboxJoint::SetReferenceBody(LinkPtr _body)
{
  ODELinkPtr odelink = boost::dynamic_pointer_cast<ODELink>(_body);

  if (odelink == NULL)
    gzwarn << "Reference body not valid, using inertial frame.\n";
  else
    dJointSetGearboxReferenceBody(this->jointId, odelink->GetODEId());
}

//////////////////////////////////////////////////
void ODEGearboxJoint::SetGearRatio(double _gearRatio)
{
  this->gearRatio = _gearRatio;
  dJointSetGearboxRatio(this->jointId, _gearRatio);
}

//////////////////////////////////////////////////
math::Vector3 ODEGearboxJoint::GetAnchor(unsigned int /*_index*/) const
{
/*
  dVector3 result;
  if (_index == 0)
    dJointGetGearboxAnchor1(this->jointId, result);
  else if (_index == 1)
    dJointGetGearboxAnchor2(this->jointId, result);
  else
    gzerr << "requesting GetAnchor axis [" << _index << "] out of range\n";
  return math::Vector3(result[0], result[1], result[2]);
*/

  gzerr << "GetAnchor not implmented.\n";
  return math::Vector3();
}

//////////////////////////////////////////////////
void ODEGearboxJoint::SetAnchor(unsigned int /*_index*/,
                                const math::Vector3 &/*_anchor*/)
{
/* anchor not used/needed in gearbox
  if (this->childLink)
    this->childLink->SetEnabled(true);
  if (this->parentLink)
    this->parentLink->SetEnabled(true);

  if (_index == 0)
    dJointSetGearboxAnchor1(this->jointId, _anchor.x, _anchor.y, _anchor.z);
  else if (_index == 1)
    dJointSetGearboxAnchor2(this->jointId, _anchor.x, _anchor.y, _anchor.z);
  else
    gzerr << "requesting SetAnchor axis [" << _index << "] out of range\n";
*/
}


//////////////////////////////////////////////////
math::Vector3 ODEGearboxJoint::GetGlobalAxis(unsigned int _index) const
{
  dVector3 result;

  if (_index == 0)
    dJointGetGearboxAxis1(this->jointId, result);
  else if (_index == 1)
    dJointGetGearboxAxis2(this->jointId, result);
  else
    gzerr << "requesting GetAnchor axis [" << _index << "] out of range\n";

  return math::Vector3(result[0], result[1], result[2]);
}

//////////////////////////////////////////////////
void ODEGearboxJoint::SetAxis(unsigned int _index, const math::Vector3 &_axis)
{
  ODEJoint::SetAxis(_index, _axis);

  if (this->childLink)
    this->childLink->SetEnabled(true);
  if (this->parentLink)
    this->parentLink->SetEnabled(true);

  if (_index == 0)
  {
    /// ODE needs global axis
    math::Quaternion axisFrame = this->GetAxisFrame(0);
    math::Vector3 globalAxis = axisFrame.RotateVector(_axis);
    dJointSetGearboxAxis1(this->jointId, globalAxis.x, globalAxis.y,
      globalAxis.z);
  }
  else if (_index == 1)
  {
    /// ODE needs global axis
    math::Quaternion axisFrame = this->GetAxisFrame(0);
    math::Vector3 globalAxis = axisFrame.RotateVector(_axis);
    dJointSetGearboxAxis2(this->jointId, globalAxis.x, globalAxis.y,
      globalAxis.z);
  }
  else
    gzerr << "requesting SetAnchor axis [" << _index << "] out of range\n";
}

//////////////////////////////////////////////////
math::Angle ODEGearboxJoint::GetAngleImpl(unsigned int /*index*/) const
{
  gzlog << "GetAngle not implemented for gearbox\n";
  return math::Angle(0);
}

//////////////////////////////////////////////////
double ODEGearboxJoint::GetVelocity(unsigned int /*index*/) const
{
  gzlog << "GetVelocity not implemented for gearbox\n";
  return 0;
}

//////////////////////////////////////////////////
void ODEGearboxJoint::SetVelocity(unsigned int /*index*/, double /*_angle*/)
{
  gzlog << "SetVelocity not implemented for gearbox\n";
  return;
}

//////////////////////////////////////////////////
void ODEGearboxJoint::SetMaxForce(unsigned int /*index*/, double /*_t*/)
{
  gzlog << "SetMaxForce not implemented for gearbox\n";
  return;
}

//////////////////////////////////////////////////
double ODEGearboxJoint::GetMaxForce(unsigned int /*index*/)
{
  gzlog << "GetMaxForce not implemented for gearbox\n";
  return 0;
}

//////////////////////////////////////////////////
void ODEGearboxJoint::SetForceImpl(unsigned int /*_index*/, double /*_effort*/)
{
  if (this->jointId)
    gzlog << "SetForce not implemented for gearbox\n";
  else
    gzerr << "ODE Joint ID is invalid\n";
}

//////////////////////////////////////////////////
double ODEGearboxJoint::GetParam(unsigned int /*_parameter*/) const
{
  gzlog << "GetParam not implemented for gearbox\n";
  return 0;
}

//////////////////////////////////////////////////
void ODEGearboxJoint::SetParam(unsigned int /*_parameter*/, double /*_value*/)
{
  gzlog << "SetParam not implemented for gearbox\n";
  return;
}
