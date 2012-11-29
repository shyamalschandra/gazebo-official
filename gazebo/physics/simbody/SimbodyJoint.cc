/*
 * Copyright 2011 Nate Koenig
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
/* Desc: The base Simbody joint class
 * Author: Nate Koenig, Andrew Howard
 * Date: 15 May 2009
 */

#include "common/Exception.hh"
#include "common/Console.hh"

#include "physics/simbody/simbody_inc.h"
#include "physics/simbody/SimbodyLink.hh"
#include "physics/simbody/SimbodyJoint.hh"

using namespace gazebo;
using namespace physics;

//////////////////////////////////////////////////
SimbodyJoint::SimbodyJoint(BasePtr _parent)
  : Joint(_parent)
{
  this->constraint = NULL;
  this->world = NULL;
}

//////////////////////////////////////////////////
SimbodyJoint::~SimbodyJoint()
{
  delete this->constraint;
  this->world = NULL;
}

//////////////////////////////////////////////////
void SimbodyJoint::Load(sdf::ElementPtr _sdf)
{
  Joint::Load(_sdf);
}

//////////////////////////////////////////////////
void SimbodyJoint::Reset()
{
  Joint::Reset();
}

//////////////////////////////////////////////////
LinkPtr SimbodyJoint::GetJointLink(int _index) const
{
  LinkPtr result;

  if (this->constraint == NULL)
    gzthrow("Attach bodies to the joint first");

  if (_index == 0 || _index == 1)
  {
    SimbodyLinkPtr simbodyLink1 =
      boost::shared_static_cast<SimbodyLink>(this->childLink);

    SimbodyLinkPtr simbodyLink2 =
      boost::shared_static_cast<SimbodyLink>(this->parentLink);

    btRigidBody rigidLink = this->constraint->getRigidBodyA();

    if (simbodyLink1 && rigidLink.getUserPointer() == simbodyLink1.get())
      result = this->childLink;
    else if (simbodyLink2)
      result = this->parentLink;
  }

  return result;
}

//////////////////////////////////////////////////
bool SimbodyJoint::AreConnected(LinkPtr _one, LinkPtr _two) const
{
  return this->constraint && ((this->childLink.get() == _one.get() &&
                               this->parentLink.get() == _two.get()) ||
                              (this->childLink.get() == _two.get() &&
                               this->parentLink.get() == _one.get()));
}

//////////////////////////////////////////////////
void SimbodyJoint::Detach()
{
  this->childLink.reset();
  this->parentLink.reset();

  delete this->constraint;
}
