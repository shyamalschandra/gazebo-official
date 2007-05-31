/*
 *  Gazebo - Outdoor Multi-Robot Simulator
 *  Copyright (C) 2003  
 *     Nate Koenig & Andrew Howard
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/* Desc: Ray proximity sensor
 * Author: Carle Cote
 * Date: 23 february 2004
 * SVN: $Id:$
*/

#include <assert.h>
#include <float.h>
#include <sstream>

#include "SensorFactory.hh"
#include "XMLConfig.hh"
#include "Global.hh"
#include "GazeboError.hh"
#include "RayGeom.hh"
#include "World.hh"
#include "PhysicsEngine.hh"
#include "GazeboError.hh"
#include "ODEPhysics.hh"
#include "XMLConfig.hh"

#include "Ray.hh"

#include "Vector3.hh"

using namespace gazebo;

GZ_REGISTER_STATIC_SENSOR("Ray", Ray);

//////////////////////////////////////////////////////////////////////////////
// Constructor
Ray::Ray()
    : Sensor()
{
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
Ray::~Ray()
{
  std::vector<RayGeom*>::iterator iter;
  for (iter=this->rays.begin(); iter!=this->rays.end(); iter++)
  {
    delete *iter;
  }
  this->rays.clear();

}

//////////////////////////////////////////////////////////////////////////////
/// Load the ray using parameter from an XMLConfig node
void Ray::LoadChild(XMLConfigNode *node)
{
  int rayCount;

  rayCount = node->GetInt("rayCount",0,1);

  this->body = World::Instance()->GetPhysicsEngine()->CreateBody(this);
  
  // Create a space to contain the ray space
  this->superSpaceId = dSimpleSpaceCreate( 0 );
    
  // Create a space to contain all the rays
  this->raySpaceId = dSimpleSpaceCreate( this->superSpaceId );
  
  // Set collision bits
  //TODO: dGeomSetCategoryBits((dGeomID) this->raySpaceId, GZ_LASER_COLLIDE);
  //TODO: dGeomSetCollideBits((dGeomID) this->raySpaceId, ~GZ_LASER_COLLIDE);

  //this->body->spaceId = this->raySpaceId;

  // Create and array of ray geoms
  for (int i = 0; i < rayCount; i++)
  {
    this->rays.push_back(new RayGeom(this->body));
  }

}

//////////////////////////////////////////////////////////////////////////////
// Init the ray
void Ray::InitChild()
{
}

//////////////////////////////////////////////////////////////////////////////
// Init the ray
void Ray::FiniChild()
{
}


//////////////////////////////////////////////////////////////////////////////
// Set ray endpoints
void Ray::SetRay(int index, const Vector3 &a, const Vector3 &b)
{
  RayGeom *ray;

  if (index >= 0 && index < (int)this->rays.size())
  {
    std::ostringstream stream;
    stream << "index[" << index << "] out of range[0-" 
           << this->rays.size() << "]";
    gzthrow(stream.str());
  }

  ray = this->rays[index];
  
  /*ray->SetColor( GzColor(0, 0, 1) );
  ray->SetCategoryBits( GZ_LASER_COLLIDE );
  ray->SetCollideBits( ~GZ_LASER_COLLIDE );
  */

  ray->SetLength(a.Distance(b));
  ray->pos[0] = a;
  ray->pos[1] = b;
}

//////////////////////////////////////////////////////////////////////////////
// Return a pointer to the ray geom
void Ray::GetRay(int index, Vector3 &pos, Vector3 &dir)
{
  if (index >= 0 && index < (int)this->rays.size())
  {
    std::ostringstream stream;
    stream << "index[" << index << "] out of range[0-" 
           << this->rays.size() << "]";
    gzthrow(stream.str());
  }
  
  this->rays[index]->Get(pos, dir);
}


//////////////////////////////////////////////////////////////////////////////
// Get detected range for a ray
double Ray::GetRange(int index)
{
  if (index >= 0 && index < (int)this->rays.size())
  {
    std::ostringstream stream;
    stream << "index[" << index << "] out of range[0-" 
           << this->rays.size() << "]";
    gzthrow(stream.str());
  }
 
  return this->rays[index]->contactDepth;
}


//////////////////////////////////////////////////////////////////////////////
// Get detected retro (intensity) value for a ray.
double Ray::GetRetro(int index)
{
  if (index >= 0 && index < (int)this->rays.size())
  {
    std::ostringstream stream;
    stream << "index[" << index << "] out of range[0-" 
           << this->rays.size() << "]";
    gzthrow(stream.str());
  }
 
  return this->rays[index]->contactRetro;
}


//////////////////////////////////////////////////////////////////////////////
// Get detected fiducial value for a ray.
int Ray::GetFiducial(int index)
{
  if (index >= 0 && index < (int)this->rays.size())
  {
    std::ostringstream stream;
    stream << "index[" << index << "] out of range[0-" 
           << this->rays.size() << "]";
    gzthrow(stream.str());
  }
 
  return this->rays[index]->contactFiducial;
}

//////////////////////////////////////////////////////////////////////////////
// Update the sensor information
void Ray::UpdateChild(UpdateParams &params)
{
  std::vector<RayGeom*>::iterator iter;
  Pose3d pose;
  Vector3 a, b;

  // Get the pose of the sensor body (global cs)
  pose = this->body->GetPose();
  
  // Reset the ray lengths and mark the geoms as dirty (so they get
  // redrawn)
  for (iter = this->rays.begin(); iter != this->rays.end(); iter++)
  {
    (*iter)->contactDepth = DBL_MAX;
    (*iter)->contactRetro = 0.0;
    (*iter)->contactFiducial = -1;

    // Update the ray endpoints (global cs)
    a = pose.CoordPositionAdd((*iter)->pos[0]);
    //a = GzCoordPositionAdd((*iter)->pos[0], pose.pos, pose.rot);

    b = pose.CoordPositionAdd((*iter)->pos[1]);
    //b = GzCoordPositionAdd((*iter)->pos[1], pose.pos, pose.rot);    

    b -= a;
    b.Normalize();
    (*iter)->Set(a, b);
  }

  ODEPhysics *ode = dynamic_cast<ODEPhysics*>(World::Instance()->GetPhysicsEngine());

  if (ode == NULL)
  {
    gzthrow( "Invalid physics engine. Must use ODE." );
  }

  // Do collision detection
  dSpaceCollide2( ( dGeomID ) ( this->superSpaceId ),
                  ( dGeomID ) ( ode->GetSpaceId() ),
                  this, &UpdateCallback );
}



/////////////////////////////////////////////////////////////////////////////
// Callback for ray intersection test
void Ray::UpdateCallback( void *data, dGeomID o1, dGeomID o2 )
{
  int n;
  dContactGeom contact;
  dxGeom *geom1, *geom2;
  RayGeom *rayGeom;
  Geom *hitGeom;
  Ray *self;

  self = (Ray*) data;
 

  // Check space
  if ( dGeomIsSpace( o1 ) || dGeomIsSpace( o2 ) )
  {
    if (dGeomGetSpace(o1) == self->superSpaceId || dGeomGetSpace(o2) == self->superSpaceId)
    {
      dSpaceCollide2( o1, o2, self, &UpdateCallback );
    }
    if (dGeomGetSpace(o1) == self->raySpaceId || dGeomGetSpace(o2) == self->raySpaceId)
    {
      dSpaceCollide2( o1, o2, self, &UpdateCallback );
    }
  }
  else
  {
    geom1 = NULL;
    geom2 = NULL;
    
    // Get pointers to the underlying geoms
    if (dGeomGetClass(o1) == dGeomTransformClass)
      geom1 = (dxGeom*) dGeomGetData(dGeomTransformGetGeom(o1));
    else
      geom1 = (dxGeom*) dGeomGetData(o1);
    
    if (dGeomGetClass(o2) == dGeomTransformClass)
      geom2 = (dxGeom*) dGeomGetData(dGeomTransformGetGeom(o2));
    else
      geom2 = (dxGeom*) dGeomGetData(o2);

    assert(geom1 && geom2);

    rayGeom = NULL;
    hitGeom = NULL;

    // Figure out which one is a ray; note that this assumes
    // that the ODE dRayClass is used *soley* by the RayGeom.    
    if (dGeomGetClass(o1) == dRayClass)
    {
      rayGeom = (RayGeom*) geom1;
      hitGeom = (Geom*) geom2;
      dGeomRaySetParams(o1, 0, 0);
      dGeomRaySetClosestHit(o1, 1);
    }    

    if (dGeomGetClass(o2) == dRayClass)
    {
      assert(rayGeom == NULL);
      rayGeom = (RayGeom*) geom2;
      hitGeom = (Geom* )geom1;
      dGeomRaySetParams(o2, 0, 0);
      dGeomRaySetClosestHit(o2, 1);
    }
        
    // Check for ray/geom intersections
    if ( rayGeom && hitGeom )
    {
      n = dCollide(o1, o2, 1, &contact, sizeof(contact));       

      if ( n > 0 )
      {       
        if (contact.depth < rayGeom->contactDepth)
        {
          Vector3 start;
          Vector3 dir;
          rayGeom->Get(start, dir);
          rayGeom->contactDepth = contact.depth;
          //TODO: rayGeom->contactRetro = hitGeom->GetRetro();
          //TODO: rayGeom->contactFiducial = hitGeom->GetFiducial();
        }
      }
    }
  }
}
