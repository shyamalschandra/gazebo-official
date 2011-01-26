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
/* Desc: A hinge joint with 2 degrees of freedom
 * Author: Nate Keonig, Andrew Howard
 * Date: 21 May 2003
 * CVS: $Id: ODEHinge2Joint.cc 7129 2008-11-12 19:38:15Z natepak $
 */

#include "gazebo_config.h"
#include "GazeboMessage.hh"
#include "Body.hh"
#include "Global.hh"
#include "XMLConfig.hh"
#include "ODEHinge2Joint.hh"

using namespace gazebo;

//////////////////////////////////////////////////////////////////////////////
// Constructor
ODEHinge2Joint::ODEHinge2Joint( dWorldID worldId )
    : Hinge2Joint<ODEJoint>()
{
  this->jointId = dJointCreateHinge2( worldId, NULL );

  Param::Begin(&this->parameters);
  this->suspensionCfmP = new ParamT<double>("suspensionCfm",0.0,0);
  Param::End();
}

//////////////////////////////////////////////////////////////////////////////
// Destructor
ODEHinge2Joint::~ODEHinge2Joint()
{
  delete this->suspensionCfmP;
}

//////////////////////////////////////////////////////////////////////////////
///  Load the joint
void ODEHinge2Joint::Load(XMLConfigNode *node)
{
  Hinge2Joint<ODEJoint>::Load(node);

  this->suspensionCfmP->Load(node);

  // Suspension CFM is only valid for Hinge2 joints
  this->SetParam(dParamSuspensionCFM, **(this->suspensionCfmP));
}

//////////////////////////////////////////////////////////////////////////////
/// Save a joint to a stream in XML format
void ODEHinge2Joint::SaveJoint(std::string &prefix, std::ostream &stream)
{
  Hinge2Joint<ODEJoint>::SaveJoint(prefix, stream);
  stream << prefix << "  " << *(this->suspensionCfmP) << "\n";
}

//////////////////////////////////////////////////////////////////////////////
// Get anchor point
Vector3 ODEHinge2Joint::GetAnchor(int index) const
{
  dVector3 result;

  // NATY
  //this->physics->LockMutex();
  if (index == 0)
    dJointGetHinge2Anchor( this->jointId, result );
  else
    dJointGetHinge2Anchor2( this->jointId, result );
  // NATY
  //this->physics->UnlockMutex();

  return Vector3(result[0], result[1], result[2]);
}

//////////////////////////////////////////////////////////////////////////////
// Set the anchor point
void ODEHinge2Joint::SetAnchor( int /*index*/, const Vector3 &anchor )
{
  // NATY
  //this->physics->LockMutex();
  if (this->body1) this->body1->SetEnabled(true);
  if (this->body2) this->body2->SetEnabled(true);
  dJointSetHinge2Anchor( this->jointId, anchor.x, anchor.y, anchor.z );
  // NATY
  //this->physics->UnlockMutex();
}

//////////////////////////////////////////////////////////////////////////////
// Set the first axis of rotation
void ODEHinge2Joint::SetAxis( int index, const Vector3 &axis )
{
  // NATY
  // this->physics->LockMutex();
  if (this->body1) this->body1->SetEnabled(true);
  if (this->body2) this->body2->SetEnabled(true);

  if (index == 0)
    dJointSetHinge2Axis1( this->jointId, axis.x, axis.y, axis.z );
  else
    dJointSetHinge2Axis2( this->jointId, axis.x, axis.y, axis.z );
  // NATY
  // this->physics->UnlockMutex();
}

//////////////////////////////////////////////////////////////////////////////
// Set the joint damping
void ODEHinge2Joint::SetDamping( int /*index*/, const double damping )
{
#ifdef INCLUDE_ODE_JOINT_DAMPING
  // NATY
  // this->physics->LockMutex();
  // ode does not yet support Hinge2 joint damping
  dJointSetDamping( this->jointId, damping);
  // NATY
  // this->physics->UnlockMutex();
#else
  gzerr(0) << "joint damping not implemented in ODE hinge2 joint\n";
#endif
}

//////////////////////////////////////////////////////////////////////////////
// Get first axis of rotation
Vector3 ODEHinge2Joint::GetAxis(int index) const
{
  dVector3 result;

  // NATY 
  // this->physics->LockMutex();
  if (index == 0)
    dJointGetHinge2Axis1( this->jointId, result );
  else
    dJointGetHinge2Axis2( this->jointId, result );
  // NATY
  // this->physics->UnlockMutex();

  return Vector3(result[0], result[1], result[2]);
}

//////////////////////////////////////////////////////////////////////////////
// Get angle of rotation about first axis
Angle ODEHinge2Joint::GetAngle(int index) const
{
  // NATY
  // this->physics->LockMutex();
  if (index == 0)
    return dJointGetHinge2Angle1( this->jointId );
  else
    gzerr(0) << "ODE has not function to get the second angle in a hinge2 joint";
  // NATY 
  // this->physics->UnlockMutex();

  return Angle(0);
}

//////////////////////////////////////////////////////////////////////////////
// Get rate of rotation about first axis
double ODEHinge2Joint::GetVelocity(int index) const
{
  double result;

  // NATY
  // this->physics->LockMutex();
  if (index == 0)
    result = dJointGetHinge2Angle1Rate( this->jointId );
  else
    result = dJointGetHinge2Angle2Rate( this->jointId );
  // NATY
  // this->physics->UnlockMutex();
}

//////////////////////////////////////////////////////////////////////////////
/// Set the velocity of an axis(index).
void ODEHinge2Joint::SetVelocity(int index, double angle)
{
  if (index == 0)
    this->SetParam(dParamVel, angle );
  else
    this->SetParam(dParamVel2, angle );
}

//////////////////////////////////////////////////////////////////////////////
/// Get the max allowed force of an axis(index).
double ODEHinge2Joint::GetMaxForce(int index)
{
  if (index == 0)
    return this->GetParam(dParamFMax);
  else
    return this->GetParam(dParamFMax2);
}


//////////////////////////////////////////////////////////////////////////////
/// Set the max allowed force of an axis(index).
void ODEHinge2Joint::SetMaxForce(int index, double t)
{
  if (index == 0)
    this->SetParam(dParamFMax, t );
  else
    this->SetParam(dParamFMax2, t );
}


//////////////////////////////////////////////////////////////////////////////
// Set _parameter with _value
void ODEHinge2Joint::SetForce(int index, double torque)
{
  // NATY
  // this->physics->LockMutex();
  if (this->body1) this->body1->SetEnabled(true);
  if (this->body2) this->body2->SetEnabled(true);

  if (index == 0)
    dJointAddHinge2Torques(this->jointId, torque, 0);
  else
    dJointAddHinge2Torques(this->jointId, 0, torque);
  // NATY
  // this->physics->UnlockMutex();
}

//////////////////////////////////////////////////////////////////////////////
// Get the specified parameter
double ODEHinge2Joint::GetParam( int parameter ) const
{
  // NATY
  // this->physics->LockMutex();
  double result = dJointGetHinge2Param( this->jointId, parameter );
  // NATY
  // this->physics->UnlockMutex();

  return result;
}

//////////////////////////////////////////////////////////////////////////////
// Set _parameter with _value
void ODEHinge2Joint::SetParam( int parameter, double value)
{
  // NATY
  // this->physics->LockMutex();
  ODEJoint::SetParam(parameter, value);
  dJointSetHinge2Param( this->jointId, parameter, value );
  // NATY
  // this->physics->UnlockMutex();
}
