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
/* Desc: ODEGeom class
 * Author: Nate Koenig
 * Date: 13 Feb 2006
 */

#include <sstream>

#include "common/Console.hh"
#include "math/Box.hh"

#include "physics/Mass.hh"
#include "physics/SurfaceParams.hh"
#include "physics/ode/ODEPhysics.hh"
#include "physics/ode/ODEBody.hh"
#include "physics/ode/ODEGeom.hh"

using namespace gazebo;
using namespace physics;

////////////////////////////////////////////////////////////////////////////////
// Constructor
ODEGeom::ODEGeom( BodyPtr _body )
    : Geom(_body)
{
  this->SetName("ODE_Geom");
  this->geomId = NULL;
}

////////////////////////////////////////////////////////////////////////////////
// Destructor
ODEGeom::~ODEGeom()
{
  if (this->geomId)
    dGeomDestroy(this->geomId);
}

////////////////////////////////////////////////////////////////////////////////
/// Load the geom
void ODEGeom::Load(common::XMLConfigNode *node)
{
  Geom::Load(node);

  this->SetSpaceId( boost::shared_static_cast<ODEBody>(this->body)->GetSpaceId() );

  /*if (this->geomId && this->placeable)
  {
    math::Pose localPose;
    dQuaternion q;

    // Transform into CoM relative Pose
    localPose = this->GetRelativePose();

    q[0] = localPose.rot.u;
    q[1] = localPose.rot.x;
    q[2] = localPose.rot.y;
    q[3] = localPose.rot.z;

    // Set the pose of the encapsulated geom; this is always relative
    // to the CoM
    dGeomSetOffsetPosition(this->geomId, localPose.pos.x, localPose.pos.y, 
        localPose.pos.z);
    dGeomSetOffsetQuaternion(this->geomId, q);
  }*/


  if (this->IsStatic())
  {
    this->SetCategoryBits(GZ_FIXED_COLLIDE);
    this->SetCollideBits(~GZ_FIXED_COLLIDE);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Pose change callback
void ODEGeom::OnPoseChange()
{
  math::Pose localPose;
  dQuaternion q;

  if (this->IsStatic() && this->geomId && this->placeable)
  {
    // Transform into global pose since a static geom does not have a body 
    localPose = this->GetWorldPose();

    q[0] = localPose.rot.w;
    q[1] = localPose.rot.x;
    q[2] = localPose.rot.y;
    q[3] = localPose.rot.z;

    dGeomSetPosition(this->geomId, localPose.pos.x, localPose.pos.y, 
                     localPose.pos.z);
    dGeomSetQuaternion(this->geomId, q);
  }
  else if (this->geomId && this->placeable)
  {
    // Transform into CoM relative Pose
    localPose = this->GetRelativePose();

    q[0] = localPose.rot.w;
    q[1] = localPose.rot.x;
    q[2] = localPose.rot.y;
    q[3] = localPose.rot.z;

    // Set the pose of the encapsulated geom; this is always relative
    // to the CoM
    dGeomSetOffsetPosition(this->geomId, localPose.pos.x, localPose.pos.y, 
        localPose.pos.z);
    dGeomSetOffsetQuaternion(this->geomId, q);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Save the body based on our common::XMLConfig node
void ODEGeom::Save(std::string &prefix, std::ostream &stream)
{
  if (this->GetShapeType() == RAY_SHAPE)
    return;

  Geom::Save(prefix, stream);
}

////////////////////////////////////////////////////////////////////////////////
// Set the encapsulated geometry object
void ODEGeom::SetGeom(dGeomID geomId, bool placeable)
{
  // Must go first in this function
  this->geomId = geomId;

  Geom::SetGeom(placeable);

  if ( dGeomGetSpace(this->geomId) == 0 )
  {
    dSpaceAdd(this->spaceId, this->geomId);
    assert(dGeomGetSpace(this->geomId) != 0);
  }

  dGeomSetData(this->geomId, this);
}

////////////////////////////////////////////////////////////////////////////////
// Return the geom id
dGeomID ODEGeom::GetGeomId() const
{
  return this->geomId;
}

////////////////////////////////////////////////////////////////////////////////
/// Get the ODE geom class
int ODEGeom::GetGeomClass() const
{
  int result = 0;

  if (this->geomId)
  {
    // NATY
    //this->physicsEngine->LockMutex();
    result = dGeomGetClass( this->geomId );

    // NATY
    //this->physicsEngine->UnlockMutex();
  }

  return result;
}
 

////////////////////////////////////////////////////////////////////////////////
/// Set the category bits, used during collision detection
void ODEGeom::SetCategoryBits(unsigned int bits)
{
  // NATY
  //this->physicsEngine->LockMutex();

  if (this->geomId)
    dGeomSetCategoryBits(this->geomId, bits);
  if (this->spaceId)
    dGeomSetCategoryBits((dGeomID)this->spaceId, bits);

  // NATY
  //this->physicsEngine->UnlockMutex();
}

////////////////////////////////////////////////////////////////////////////////
/// Set the collide bits, used during collision detection
void ODEGeom::SetCollideBits(unsigned int bits)
{
  // NATY
  //this->physicsEngine->LockMutex();

  if (this->geomId)
    dGeomSetCollideBits(this->geomId, bits);
  if (this->spaceId)
    dGeomSetCollideBits((dGeomID)this->spaceId, bits);

  // NATY
  //this->physicsEngine->UnlockMutex();
}

////////////////////////////////////////////////////////////////////////////////
/// Get the mass of the geom
Mass ODEGeom::GetBodyMassMatrix()
{
  Mass result;
  math::Pose pose;
  math::Vector3 cog, principals, products;
  dMass bodyMass;
  dQuaternion q;
  dMatrix3 r;

  if (!this->placeable)
    return NULL;

  cog = this->mass.GetCoG();
  principals = this->mass.GetPrincipalMoments();
  products = this->mass.GetProductsofInertia();

  pose = this->GetWorldPose(); // get pose of the geometry

  q[0] = pose.rot.w;
  q[1] = pose.rot.x;
  q[2] = pose.rot.y;
  q[3] = pose.rot.z;

  dQtoR(q,r); // turn quaternion into rotation matrix

  dMassSetZero(&bodyMass);
  dMassSetParameters(&bodyMass, this->mass.GetAsDouble(),
                     cog.x, cog.y, cog.z,
                     principals.x, principals.y, principals.z,
                     products.x, products.y, products.z );

  if (dMassCheck(&bodyMass))
  {
    dMassRotate(&bodyMass, r);
    dMassTranslate( &bodyMass, pose.pos.x, pose.pos.y, pose.pos.z);
  }

  ODEPhysics::ConvertMass(&result, &bodyMass);

  return result;
}

////////////////////////////////////////////////////////////////////////////////
/// Get the bounding box, defined by the physics engine
math::Box ODEGeom::GetBoundingBox() const
{
  math::Box box;
  dReal aabb[6];

  memset(aabb, 0, 6 * sizeof(dReal));

  //if (this->geomId && this->type != Shape::PLANE)
  dGeomGetAABB(this->geomId, aabb);

  box.min.Set(aabb[0], aabb[2], aabb[4]);
  box.max.Set(aabb[1], aabb[3], aabb[5]);

  return box;
}

////////////////////////////////////////////////////////////////////////////////
/// Get the bodies space ID
dSpaceID ODEGeom::GetSpaceId() const
{
  return this->spaceId;
}

////////////////////////////////////////////////////////////////////////////////
/// Set the bodies space ID
void ODEGeom::SetSpaceId(dSpaceID spaceid)
{
  this->spaceId = spaceid;
}
