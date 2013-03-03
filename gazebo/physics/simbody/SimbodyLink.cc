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
/* Desc: Link class
 * Author: Nate Koenig
 * Date: 13 Feb 2006
 */

#include "common/Console.hh"
#include "common/Exception.hh"

#include "physics/World.hh"

#include "physics/simbody/simbody_inc.h"
#include "physics/simbody/SimbodyCollision.hh"
#include "physics/simbody/SimbodyPhysics.hh"
#include "physics/simbody/SimbodyLink.hh"

using namespace gazebo;
using namespace physics;

//////////////////////////////////////////////////
SimbodyLink::SimbodyLink(EntityPtr _parent)
    : Link(_parent)
{
  this->mustBeBaseLink = false;
}

//////////////////////////////////////////////////
SimbodyLink::~SimbodyLink()
{
}

//////////////////////////////////////////////////
void SimbodyLink::Load(sdf::ElementPtr _sdf)
{
  this->simbodyPhysics = boost::shared_dynamic_cast<SimbodyPhysics>(
      this->GetWorld()->GetPhysicsEngine());

  if (this->simbodyPhysics == NULL)
    gzthrow("Not using the simbody physics engine");

  if (_sdf->HasElement("must_be_base_link"))
    this->mustBeBaseLink = _sdf->GetValueBool("must_be_base_link");

  Link::Load(_sdf);
}

//////////////////////////////////////////////////
void SimbodyLink::Init()
{
  Link::Init();

  math::Vector3 cogVec = this->inertial->GetCoG();

  // Set the initial pose of the body

  for (Base_V::iterator iter = this->children.begin();
       iter != this->children.end(); ++iter)
  {
    if ((*iter)->HasType(Base::COLLISION))
    {
      SimbodyCollisionPtr collision;
      collision = boost::shared_static_cast<SimbodyCollision>(*iter);

      math::Pose relativePose = collision->GetRelativePose();
      relativePose.pos -= cogVec;

    }
  }


  // Create a construction info object
  // Create the new rigid body
}

//////////////////////////////////////////////////
void SimbodyLink::Fini()
{
  Link::Fini();
}

//////////////////////////////////////////////////
void SimbodyLink::Update()
{
  Link::Update();
}

//////////////////////////////////////////////////
void SimbodyLink::SetGravityMode(bool /*_mode*/)
{
}

//////////////////////////////////////////////////
bool SimbodyLink::GetGravityMode() const
{
  bool result = false;
  return result;
}

//////////////////////////////////////////////////
void SimbodyLink::SetSelfCollide(bool /*_collide*/)
{
}

//////////////////////////////////////////////////
/*void SimbodyLink::AttachCollision(Collision *_collision)
{
  Link::AttachCollision(_collision);

  SimbodyCollision *bcollision = dynamic_cast<SimbodyCollision*>(_collision);

  if (_collision == NULL)
    gzthrow("requires SimbodyCollision");

  math::Pose relativePose = _collision->GetRelativePose();
}
  */

//////////////////////////////////////////////////
/// changed
void SimbodyLink::OnPoseChange()
{
  /*
  math::Pose pose = this->GetWorldPose();

  */
}

//////////////////////////////////////////////////
void SimbodyLink::SetEnabled(bool /*_enable*/) const
{
}

//////////////////////////////////////////////////
void SimbodyLink::SetLinearVel(const math::Vector3 & /*_vel*/)
{
}

//////////////////////////////////////////////////
math::Vector3 SimbodyLink::GetWorldLinearVel(
  const math::Vector3& /*_vector3*/) const
{
  return math::Vector3();
}

//////////////////////////////////////////////////
math::Vector3 SimbodyLink::GetWorldLinearVel(
  const math::Pose &/*_pose*/) const
{
  return math::Vector3();
}

//////////////////////////////////////////////////
math::Vector3 SimbodyLink::GetWorldCoGLinearVel() const
{
  return math::Vector3();
}

//////////////////////////////////////////////////
void SimbodyLink::SetAngularVel(const math::Vector3 &/*_vel*/)
{
}

//////////////////////////////////////////////////
math::Vector3 SimbodyLink::GetWorldAngularVel() const
{
  return math::Vector3();
}

//////////////////////////////////////////////////
void SimbodyLink::SetForce(const math::Vector3 &/*_force*/)
{
}

//////////////////////////////////////////////////
math::Vector3 SimbodyLink::GetWorldForce() const
{
  return math::Vector3();
}

//////////////////////////////////////////////////
void SimbodyLink::SetTorque(const math::Vector3 &/*_torque*/)
{
}

//////////////////////////////////////////////////
math::Vector3 SimbodyLink::GetWorldTorque() const
{
  return math::Vector3();
}

//////////////////////////////////////////////////
void SimbodyLink::SetLinearDamping(double /*_damping*/)
{
}

//////////////////////////////////////////////////
void SimbodyLink::SetAngularDamping(double /*_damping*/)
{
}

/////////////////////////////////////////////////
void SimbodyLink::AddForce(const math::Vector3 &/*_force*/)
{
}

/////////////////////////////////////////////////
void SimbodyLink::AddRelativeForce(const math::Vector3 &/*_force*/)
{
}

/////////////////////////////////////////////////
void SimbodyLink::AddForceAtWorldPosition(const math::Vector3 &/*_force*/,
                                         const math::Vector3 &/*_pos*/)
{
}

/////////////////////////////////////////////////
void SimbodyLink::AddForceAtRelativePosition(const math::Vector3 &/*_force*/,
                  const math::Vector3 &/*_relpos*/)
{
}

/////////////////////////////////////////////////
void SimbodyLink::AddTorque(const math::Vector3 &/*_torque*/)
{
}

/////////////////////////////////////////////////
void SimbodyLink::AddRelativeTorque(const math::Vector3 &/*_torque*/)
{
}

/////////////////////////////////////////////////
void SimbodyLink::SetAutoDisable(bool /*_disable*/)
{
}

/////////////////////////////////////////////////
SimTK::MassProperties SimbodyLink::GetMassProperties() const
{
  if (!this->IsStatic())
  {
    const SimTK::Real mass = this->inertial->GetMass();
    SimTK::Transform X_LI = physics::SimbodyPhysics::Pose2Transform(
      this->inertial->GetPose());
    const SimTK::Vec3 &com_L = X_LI.p(); // vector from Lo to com, exp. in L

    if (math::equal(mass, 0.0))
      return SimTK::MassProperties(mass,com_L,SimTK::UnitInertia(1,1,1));

    // Get mass-weighted central inertia, expressed in I frame.
    SimTK::Inertia Ic_I(this->inertial->GetIXX(),
                 this->inertial->GetIYY(),
                 this->inertial->GetIZZ(),
                 this->inertial->GetIXY(),
                 this->inertial->GetIXZ(),
                 this->inertial->GetIYZ());
    // Re-express the central inertia from the I frame to the L frame.
    SimTK::Inertia Ic_L = Ic_I.reexpress(~X_LI.R()); // Ic_L=R_LI*Ic_I*R_IL
    // Shift to L frame origin.
    SimTK::Inertia Io_L = Ic_L.shiftFromMassCenter(-com_L, mass);
    return SimTK::MassProperties(mass, com_L, Io_L); // converts to unit inertia
  }
  else
  {
    gzerr << "inertial block no specified, using unit mass properties\n";
    return SimTK::MassProperties(1,SimTK::Vec3(0),SimTK::UnitInertia(1,1,1));
  }
}

// When a link is broken into several fragments (master and slaves), they
// share the mass equally. Given the number of fragments, this returns the
// appropriate mass properties to use for each fragment. Per Simbody's
// convention, COM is measured from, and inertia taken about, the link 
// origin and both are expressed in the link frame.
SimTK::MassProperties SimbodyLink::GetEffectiveMassProps(int _numFragments) const
{
    SimTK::MassProperties massProps = this->GetMassProperties();
    assert(_numFragments > 0); // must be at least 1 for the master
    return SimTK::MassProperties(massProps.getMass()/_numFragments,
                          massProps.getMassCenter(),
                          massProps.getUnitInertia());
}

