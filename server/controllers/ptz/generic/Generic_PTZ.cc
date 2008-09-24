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
 * Desc: A generic ptz controller
 * Author: Nathan Koenig
 * Date: 26 Nov 2007
 * SVN: $Id$
 */

#include <algorithm>
#include <assert.h>

#include "Model.hh"
#include "Global.hh"
#include "XMLConfig.hh"
#include "Simulator.hh"
#include "gazebo.h"
#include "GazeboError.hh"
#include "ControllerFactory.hh"
#include "HingeJoint.hh"
#include "Generic_PTZ.hh"

using namespace gazebo;

GZ_REGISTER_STATIC_CONTROLLER("generic_ptz", Generic_PTZ);

////////////////////////////////////////////////////////////////////////////////
// Constructor
Generic_PTZ::Generic_PTZ(Entity *parent)
    : Controller(parent)
{
  this->myParent = dynamic_cast<Model*>(this->parent);

  if (!this->myParent)
    gzthrow("Generic_PTZ controller requires a Model as its parent");

  this->panJoint = NULL;
  this->tiltJoint = NULL;

  this->panJoint = NULL;
  this->tiltJoint = NULL;

  Param::Begin(&this->parameters);
  this->panJointNameP = new ParamT<std::string>("panJoint", "", 1);
  this->tiltJointNameP = new ParamT<std::string>("tiltJoint", "", 1);
  this->motionGainP = new ParamT<double>("motionGain",2,0);
  this->forceP = new ParamT<double>("force",10,0);
  Param::End();
}

////////////////////////////////////////////////////////////////////////////////
// Destructor
Generic_PTZ::~Generic_PTZ()
{
  if (this->panJoint)
    delete this->panJoint;
  if (this->tiltJoint)
    delete this->tiltJoint;

  this->panJoint = NULL;
  this->tiltJoint = NULL;

  delete this->panJointNameP;
  delete this->tiltJointNameP;
  delete this->motionGainP;
  delete this->forceP;
}

////////////////////////////////////////////////////////////////////////////////
// Load the controller
void Generic_PTZ::LoadChild(XMLConfigNode *node)
{
  this->ptzIface = dynamic_cast<PTZIface*>(this->ifaces[0]);

  if (!this->ptzIface)
    gzthrow("Generic_PTZ controller requires a PTZIface");

  this->panJointNameP->Load(node);
  this->tiltJointNameP->Load(node);
  this->motionGainP->Load(node);
  this->forceP->Load(node);

  this->panJoint = dynamic_cast<HingeJoint*>(this->myParent->GetJoint(this->panJointNameP->GetValue()));
  this->tiltJoint = dynamic_cast<HingeJoint*>(this->myParent->GetJoint(this->tiltJointNameP->GetValue()));

  if (!this->panJoint)
    gzthrow("couldn't get pan hinge joint");

  if (!this->tiltJoint)
    gzthrow("couldn't get tilt hinge joint");

}

////////////////////////////////////////////////////////////////////////////////
/// Save the controller.
void Generic_PTZ::SaveChild(std::string &prefix, std::ostream &stream)
{
  stream << prefix << *(this->panJointNameP) << "\n";
  stream << prefix << *(this->tiltJointNameP) << "\n";
  stream << prefix << *(this->motionGainP) << "\n";
  stream << prefix << *(this->forceP) << "\n";
}

////////////////////////////////////////////////////////////////////////////////
// Initialize the controller
void Generic_PTZ::InitChild()
{
}

////////////////////////////////////////////////////////////////////////////////
// Reset the controller
void Generic_PTZ::ResetChild()
{

}

////////////////////////////////////////////////////////////////////////////////
// Update the controller
void Generic_PTZ::UpdateChild()
{
  this->ptzIface->Lock(1);

  this->cmdPan = this->ptzIface->data->cmd_pan;
  this->cmdTilt = this->ptzIface->data->cmd_tilt;
  //this->cmdZoom = this->hfov / this->ptzIface->data->cmd_zoom;

  this->ptzIface->Unlock();

  // Apply joint limits to commanded pan/tilt angles
  if (this->cmdTilt > M_PI*0.3)
    this->cmdTilt = M_PI*0.3;
  else if (this->cmdTilt < -M_PI*0.3)
    this->cmdTilt = -M_PI*0.3;

  if (this->cmdPan > M_PI*0.3)
    this->cmdPan = M_PI*0.3;
  else if (this->cmdPan < -M_PI*0.3)
    this->cmdPan = -M_PI*0.3;

  // Apply limits on commanded zoom
  /*if (this->cmdZoom < this->zoomMin)
    this->cmdZoom = this->zoomMin;
  if (this->cmdZoom > this->zoomMax)
    this->cmdZoom = this->zoomMax;
    */

  // Set the pan and tilt motors; can't set angles so track cmds with
  // a proportional control
  float tilt = this->cmdTilt - this->tiltJoint->GetAngle();
  float pan = this->cmdPan - this->panJoint->GetAngle();

  this->tiltJoint->SetParam( dParamVel, **(this->motionGainP) * tilt);
  this->tiltJoint->SetParam( dParamFMax, **(this->forceP) );

  this->panJoint->SetParam( dParamVel, **(this->motionGainP) * pan);
  this->panJoint->SetParam( dParamFMax, **(this->forceP) );

  this->PutPTZData();
}

////////////////////////////////////////////////////////////////////////////////
// Finalize the controller
void Generic_PTZ::FiniChild()
{
}

////////////////////////////////////////////////////////////////////////////////
// Put laser data to the interface
void Generic_PTZ::PutPTZData()
{
  PTZData *data = this->ptzIface->data;

  this->ptzIface->Lock(1);

  // Data timestamp
  data->head.time = Simulator::Instance()->GetSimTime();

  data->pan = this->panJoint->GetAngle();
  data->tilt = this->tiltJoint->GetAngle();
  //data->zoom = this->camera->GetFOV();

  this->ptzIface->Unlock();

  // New data is available
  this->ptzIface->Post();
}

