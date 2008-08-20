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
 */
/* Desc: Base class for all sensors
 * Author: Nathan Koenig
 * Date: 25 May 2007
 * SVN: $Id$
 */

#include "Controller.hh"
#include "gazebo.h"
#include "GazeboMessage.hh"
#include "GazeboError.hh"
#include "Body.hh"
#include "XMLConfig.hh"
#include "World.hh"
#include "ControllerFactory.hh"
#include "Sensor.hh"

using namespace gazebo;

////////////////////////////////////////////////////////////////////////////////
// Constructor
Sensor::Sensor(Body *body)
    : Entity(body)
{
  this->body = body;
  this->controller = NULL;
  this->active = true;
}

////////////////////////////////////////////////////////////////////////////////
// Destructor
Sensor::~Sensor()
{
}

////////////////////////////////////////////////////////////////////////////////
// Load the sensor
void Sensor::Load(XMLConfigNode *node)
{
  this->nameP->Load(node);
  this->LoadController( node->GetChildByNSPrefix("controller") );
  this->LoadChild(node);
}

////////////////////////////////////////////////////////////////////////////////
/// Save the sensor info in XML format
void Sensor::Save(std::string &prefix, std::ostream &stream)
{
  
}
 
////////////////////////////////////////////////////////////////////////////////
/// Initialize the sensor
void Sensor::Init()
{
  this->InitChild();
}

////////////////////////////////////////////////////////////////////////////////
/// Update the sensor
void Sensor::Update()
{
  this->UpdateChild();
  if (this->controller)
    this->controller->Update();
}

////////////////////////////////////////////////////////////////////////////////
/// Finalize the sensor
void Sensor::Fini()
{
  if (this->controller)
    this->controller->Fini();
  this->FiniChild();
}

////////////////////////////////////////////////////////////////////////////////
/// Load a controller helper function
void Sensor::LoadController(XMLConfigNode *node)
{
  if (!node)
  {
    gzmsg(0) << this->GetName() << " sensor has no controller.\n";
    return;
  }


  //Iface *iface;
  //XMLConfigNode *childNode;
  std::ostringstream stream;

  // Get the controller's type
  std::string controllerType = node->GetName();

  // Get the unique name of the controller
  std::string controllerName = node->GetString("name","",1);

  // Create the interface
  /*if ( (childNode = node->GetChildByNSPrefix("interface")) )
  {
    // Get the type of the interface (eg: laser)
    std::string ifaceType = childNode->GetName();

    // Get the name of the iface
    std::string ifaceName = childNode->GetString("name","",1);

    // Use the factory to get a new iface based on the type
    iface = IfaceFactory::NewIface(ifaceType);

    // Create the iface
    iface->Create(World::Instance()->GetGzServer(), ifaceName);

  }
  else
  {
    stream << "No interface defined for " << controllerName << "controller";
    gzthrow(stream.str());
  }*/
  
  // See if the controller is in a plugin
  std::string pluginName = node->GetString("plugin","",0);
  if (pluginName != "")
    ControllerFactory::LoadPlugin(pluginName, controllerType);

  // Create the controller based on it's type
  this->controller = ControllerFactory::NewController(controllerType, this);

  // Load the controller
  this->controller->Load(node);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Set whether the sensor is active or not
void Sensor::SetActive(bool value)
{
  this->active = value;
}


