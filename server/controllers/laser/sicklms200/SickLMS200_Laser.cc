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
/*
 * Desc: SickLMS200 Laser controller.
 * Author: Nathan Koenig
 * Date: 01 Feb 2007
 * SVN info: $Id: SickLMS200_Laser.cc 28 2007-05-31 00:53:17Z natepak $
 */

#include <algorithm>
#include <assert.h>

#include "Sensor.hh"
#include "Global.hh"
#include "XMLConfig.hh"
#include "HingeJoint.hh"
#include "World.hh"
#include "gazebo.h"
#include "GazeboError.hh"
#include "ControllerFactory.hh"
#include "RaySensor.hh"
#include "SickLMS200_Laser.hh"

using namespace gazebo;

GZ_REGISTER_STATIC_CONTROLLER("sicklms200_laser", SickLMS200_Laser);

////////////////////////////////////////////////////////////////////////////////
// Constructor
SickLMS200_Laser::SickLMS200_Laser(Entity *parent)
  : Controller(parent)
{
  this->myParent = dynamic_cast<RaySensor*>(this->parent);

  if (!this->myParent)
    gzthrow("SickLMS200_Laser controller requires a Ray Sensor as its parent");

  this->laserIface = NULL;
  this->fiducialIface = NULL;
}

////////////////////////////////////////////////////////////////////////////////
// Destructor
SickLMS200_Laser::~SickLMS200_Laser()
{
}

////////////////////////////////////////////////////////////////////////////////
// Load the controller
void SickLMS200_Laser::LoadChild(XMLConfigNode *node)
{
  std::vector<Iface*>::iterator iter;

  for (iter = this->ifaces.begin(); iter != this->ifaces.end(); iter++)
  {
    if ((*iter)->GetType() == "laser")
      this->laserIface = dynamic_cast<LaserIface*>(*iter);
    else if ((*iter)->GetType() == "fiducial")
      this->fiducialIface = dynamic_cast<FiducialIface*>(*iter);
  }

  if (!this->laserIface) gzthrow("SickLMS200_Laser controller requires a LaserIface"); 

}

////////////////////////////////////////////////////////////////////////////////
// Initialize the controller
void SickLMS200_Laser::InitChild()
{
}

////////////////////////////////////////////////////////////////////////////////
// Update the controller
void SickLMS200_Laser::UpdateChild(UpdateParams &params)
{
  this->PutLaserData();
  this->PutFiducialData();
}

////////////////////////////////////////////////////////////////////////////////
// Finalize the controller
void SickLMS200_Laser::FiniChild()
{
}

////////////////////////////////////////////////////////////////////////////////
// Put laser data to the interface
void SickLMS200_Laser::PutLaserData()
{
  int i, ja, jb;
  double ra, rb, r, b;
  int v;

  double maxAngle = this->myParent->GetMaxAngle();
  double minAngle = this->myParent->GetMinAngle();

  double maxRange = this->myParent->GetMaxRange();
  double minRange = this->myParent->GetMinRange();
  int rayCount = this->myParent->GetRayCount();
  int rangeCount = this->myParent->GetRangeCount();
 
  this->laserIface->Lock(1);

  // Data timestamp
  this->laserIface->data->time = World::Instance()->GetSimTime();
    
  // Read out the laser range data
  this->laserIface->data->min_angle = minAngle;
  this->laserIface->data->max_angle = maxAngle;
  this->laserIface->data->res_angle = (maxAngle - minAngle) / (rangeCount - 1);
  this->laserIface->data->max_range = maxRange;
  this->laserIface->data->range_count = rangeCount;

  assert(this->laserIface->data->range_count < GZ_LASER_MAX_RANGES );

  // Interpolate the range readings from the rays
  for (i = 0; i<rangeCount; i++)
  {
    b = (double) i * (rayCount - 1) / (rangeCount - 1);
    ja = (int) floor(b);
    jb = std::min(ja + 1, rayCount - 1);    
    b = b - floor(b);

    assert(ja >= 0 && ja < rayCount);
    assert(jb >= 0 && jb < rayCount);

    ra = std::min(this->myParent->GetRange(ja) , maxRange);
    rb = std::min(this->myParent->GetRange(jb) , maxRange);

    // Range is linear interpolation if values are close,
    // and min if they are very different
    if (fabs(ra - rb) < 0.10)
      r = (1 - b) * ra + b * rb;
    else r = std::min(ra, rb);

    // Intensity is either-or
    v = (int) this->myParent->GetRetro(ja) || (int) this->myParent->GetRetro(jb);

    this->laserIface->data->ranges[rangeCount-i-1] =  r + minRange;
    this->laserIface->data->intensity[i] = v;
  }
  this->laserIface->Unlock();

  // New data is available
  this->laserIface->Post();
}

//////////////////////////////////////////////////////////////////////////////
// Update the data in the interface
void SickLMS200_Laser::PutFiducialData()
{
  int i, j, count;
  FiducialFid *fid;
  double r, b;
  double ax, ay, bx, by, cx, cy;

  double maxAngle = this->myParent->GetMaxAngle();
  double minAngle = this->myParent->GetMinAngle();

  double maxRange = this->myParent->GetMaxRange();
  double minRange = this->myParent->GetMinRange();
  int rayCount = this->myParent->GetRayCount();
  int rangeCount = this->myParent->GetRangeCount();
 
  this->fiducialIface->Lock(1);

  // Data timestamp
  this->fiducialIface->data->time = World::Instance()->GetSimTime();
  this->fiducialIface->data->count = 0;

  // TODO: clean this up
  count = 0;
  for (i = 0; i < rayCount; i++)
  {
    if (this->myParent->GetFiducial(i) < 0)
      continue;

    // Find the end of the fiducial
    for (j = i + 1; j < rayCount; j++)
    {
      if (this->myParent->GetFiducial(j) != this->myParent->GetFiducial(i))
        break;
    }
    j--;

    // Need at least three points to get orientation
    if (j - i + 1 >= 3)
    {
      r = minRange + this->myParent->GetRange(i);
      b = minAngle + i * ((maxAngle-minAngle) / (rayCount - 1));
      ax = r * cos(b);
      ay = r * sin(b);

      r = minRange + this->myParent->GetRange(j);
      b = minAngle + j * ((maxAngle-minAngle) / (rayCount - 1));
      bx = r * cos(b);
      by = r * sin(b);

      cx = (ax + bx) / 2;
      cy = (ay + by) / 2;
      
      assert(count < GZ_FIDUCIAL_MAX_FIDS);
      fid = this->fiducialIface->data->fids + count++;

      fid->id = this->myParent->GetFiducial(j);
      fid->pos[0] = cx;
      fid->pos[1] = cy;
      fid->rot[2] = atan2(by - ay, bx - ax) + M_PI / 2;
    }

    // Fewer points get no orientation
    else
    {
      r = minRange + this->myParent->GetRange(i);
      b = minAngle + i * ((maxAngle-minAngle) / (rayCount - 1));
      ax = r * cos(b);
      ay = r * sin(b);

      r = minRange + this->myParent->GetRange(j);
      b = minAngle + j * ((maxAngle-minAngle) / (rayCount - 1));
      bx = r * cos(b);
      by = r * sin(b);

      cx = (ax + bx) / 2;
      cy = (ay + by) / 2;
      
      assert(count < GZ_FIDUCIAL_MAX_FIDS);
      fid = this->fiducialIface->data->fids + count++;

      fid->id = this->myParent->GetFiducial(j);
      fid->pos[0] = cx;
      fid->pos[1] = cy;
      fid->rot[2] = atan2(cy, cx) + M_PI;
    }

    /*printf("fiducial %d i[%d] j[%d] %.2f %.2f %.2f\n",
           fid->id, i,j,fid->pos[0], fid->pos[1], fid->rot[2]);
           */
    i = j;
  }

  this->fiducialIface->data->count = count;
  
  this->fiducialIface->Unlock();
  this->fiducialIface->Post();
}
