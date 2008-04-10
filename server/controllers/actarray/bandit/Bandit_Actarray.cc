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
 * Desc: Actuator array controller for a Bandit robot.
 * Author: Nathan Koenig
 * Date: 19 Sep 2007
 * SVN info: $Id$
 */

#include "Global.hh"
#include "XMLConfig.hh"
#include "Model.hh"
#include "HingeJoint.hh"
#include "Simulator.hh"
#include "gazebo.h"
#include "GazeboError.hh"
#include "ControllerFactory.hh"
#include "Bandit_Actarray.hh"

using namespace gazebo;

GZ_REGISTER_STATIC_CONTROLLER("bandit_actarray", Bandit_Actarray);

////////////////////////////////////////////////////////////////////////////////
// Constructor
Bandit_Actarray::Bandit_Actarray(Entity *parent )
    : Controller(parent)
{
  this->myParent = dynamic_cast<Model*>(this->parent);

  if (!this->myParent)
    gzthrow("Bandit_Actarray controller requires a Model as its parent");

}

////////////////////////////////////////////////////////////////////////////////
// Destructor
Bandit_Actarray::~Bandit_Actarray()
{
}

////////////////////////////////////////////////////////////////////////////////
// Load the controller
void Bandit_Actarray::LoadChild(XMLConfigNode *node)
{
  XMLConfigNode *jNode;
  int i =0;
  this->myIface = dynamic_cast<ActarrayIface*>(this->ifaces[0]);

  if (!this->myIface)
    gzthrow("Bandit_Actarray controller requires a Actarray Iface");


  for (i=0, jNode = node->GetChild("joint"); jNode; i++)
  {
    std::string name = jNode->GetString("name","",1);

    this->joints[i] = dynamic_cast<HingeJoint*>(this->myParent->GetJoint(name));
    this->forces[i] = jNode->GetDouble("force",0.0,1);
    this->gains[i] = jNode->GetDouble("gain",0.0,1);

    jNode = jNode->GetNext("joint");
  }
}

////////////////////////////////////////////////////////////////////////////////
// Initialize the controller
void Bandit_Actarray::InitChild()
{
  for (int i=0; i<16; i++)
  {
    this->joints[i]->SetParam( dParamVel, 0.0);
    this->joints[i]->SetParam( dParamFMax, this->forces[i] );
  }
}

////////////////////////////////////////////////////////////////////////////////
// Update the controller
void Bandit_Actarray::UpdateChild(UpdateParams &params)
{
  HingeJoint *joint = NULL;
  float angle;
  float hiStop, loStop;

  this->myIface->Lock(1);
  this->myIface->data->head.time = Simulator::Instance()->GetSimTime();

  this->myIface->data->actuators_count = 16;

  for (int i=0; i<16; i++)
  {
    double cmdAngle = this->myIface->data->cmd_pos[i];
    joint = dynamic_cast<HingeJoint*>(this->joints[i]);

    if (cmdAngle > joint->GetHighStop())
    {
      cmdAngle = joint->GetHighStop();
    }
    else if (cmdAngle < joint->GetLowStop())
    {
      cmdAngle = joint->GetLowStop();
    }

    angle = cmdAngle - joint->GetAngle();

    if (fabs(angle) > 0.01)
    {
      joint->SetParam( dParamVel, this->gains[i] * angle);
      joint->SetParam( dParamFMax, this->forces[i] );
    }

    this->myIface->data->actuators[i].position = joint->GetAngle();
    this->myIface->data->actuators[i].speed = joint->GetAngleRate();
  }

  this->myIface->data->new_cmd = 0;

  this->myIface->Unlock();
}

////////////////////////////////////////////////////////////////////////////////
// Finalize the controller
void Bandit_Actarray::FiniChild()
{
}
