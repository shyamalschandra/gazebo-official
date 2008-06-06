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
/* Desc: The Simulator; Top level managing object
 * Author: Jordi Polo
 * Date: 3 Jan 2008
 */

#include <assert.h>
#include <iostream>
#include <fstream>
#include <sys/time.h>
//#include <boost/signals.hpp>
//#include <boost/bind.hpp>

#include "World.hh"
#include "GuiFactory.hh"
#include "Gui.hh"
#include "DummyGui.hh"
#include "XMLConfig.hh"
#include "GazeboConfig.hh"
#include "gazebo.h"
#include "PhysicsEngine.hh"
#include "OgreAdaptor.hh"
#include "GazeboMessage.hh"
#include "Global.hh"

#include "Simulator.hh"

#define MAX_FRAME_RATE 35

using namespace gazebo;

////////////////////////////////////////////////////////////////////////////////
// Constructor
Simulator::Simulator()
: xmlFile(NULL),
  gui(NULL),
  renderEngine(NULL),
  gazeboConfig(NULL),
  loaded(false),
  pause(false),
  iterations(0),
  simTime(0.0),
  pauseTime(0.0),
  startTime(0.0),
  physicsUpdates(0),
  checkpoint(0.0),
  renderUpdates(0),
  userPause(false),
  userStep(false),
  userStepInc(false),
  userQuit(false)
{
}

////////////////////////////////////////////////////////////////////////////////
// Destructor
Simulator::~Simulator()
{
  this->Close();
}

////////////////////////////////////////////////////////////////////////////////
/// Closes the Simulator and frees everything
void Simulator::Close()
{
  if (!this->loaded)
    return;

  GZ_DELETE (this->gui)
  GZ_DELETE (this->xmlFile)
  GZ_DELETE (this->gazeboConfig)
  gazebo::World::Instance()->Close();
  gazebo::OgreAdaptor::Instance()->Close();

  //GZ_DELETE(this->renderEngine);
}

////////////////////////////////////////////////////////////////////////////////
/// Load the world configuration file
/// Any error that reach this level must make the simulator exit
void Simulator::Load(const std::string &worldFileName, unsigned int serverId )
{
  if (loaded)
  {
    this->Close();
    loaded=false;
  }

    // Load the world file
  this->xmlFile=new gazebo::XMLConfig();
  try
  {
    this->xmlFile->Load(worldFileName);
  }
  catch (GazeboError e)
  {
    gzthrow("The XML config file can not be loaded, please make sure is a correct file\n" << e); 
  }
  XMLConfigNode *rootNode(xmlFile->GetRootNode());

    // Load the messaging system
  gazebo::GazeboMessage::Instance()->Load(rootNode);

    // load the configuration options 
  this->gazeboConfig=new gazebo::GazeboConfig();
  try
  {
    this->gazeboConfig->Load();
  }
  catch (GazeboError e)
  {
    gzthrow("Error loading the Gazebo configuration file, check the .gazeborc file on your HOME directory \n" << e); 
  }

  //Create and initialize the Gui
  try
  {
    this->gui=GuiFactory::NewGui(rootNode);
    this->gui->Init();
  }
  catch (GazeboError e)
  {
    gzthrow( "Error loading the GUI\n" << e);
  }

  //Initialize RenderEngine //create factory
  //this->renderEngine = new OgreAdaptor();
  try
  {
    OgreAdaptor::Instance()->Init(rootNode);
    this->renderEngine = OgreAdaptor::Instance();
    //this->renderEngine->Init();
  }
  catch (gazebo::GazeboError e)
  {
    gzthrow("Failed to Initialize the Rendering engine subsystem\n" << e );
  }

    //Create the world
  try
  {
    gazebo::World::Instance()->Load(rootNode, serverId);
  }
  catch (GazeboError e)
  {
    gzthrow("Failed to load the GUI\n"  << e);
  }

  this->loaded=true;
}

////////////////////////////////////////////////////////////////////////////////
/// Save the world configuration file
void Simulator::Save(const std::string& filename)
{
  // Saving in the preferred order
  XMLConfigNode* root=xmlFile->GetRootNode();
  GazeboMessage::Instance()->Save(root);
  World::Instance()->GetPhysicsEngine()->Save(root);
  this->GetRenderEngine()->Save(root);
  this->SaveGui(root);
  World::Instance()->Save(root);

  if (xmlFile->Save(filename)<0)
  {
   gzthrow("The XML file could not be written back to " << filename );
   }
}


////////////////////////////////////////////////////////////////////////////////
/// Initialize the simulation
int Simulator::Init()
{
  this->startTime = this->GetWallTime();

  //Initialize the world
  if (gazebo::World::Instance()->Init() != 0)
    return -1;
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
/// Finalize the simulation
void Simulator::Fini( )
{
  gazebo::World::Instance()->Fini();
}

////////////////////////////////////////////////////////////////////////////////
/// Main simulation loop, when this loop ends the simulation finish
void Simulator::MainLoop()
{
  double step = World::Instance()->GetPhysicsEngine()->GetStepTime();
  double physicsUpdateRate = World::Instance()->GetPhysicsEngine()->GetUpdateRate();
  double renderUpdateRate = OgreAdaptor::Instance()->GetUpdateRate();

  double currTime;
  double elapsedTime;

  this->prevPhysicsTime = this->GetRealTime();
  this->prevRenderTime = this->GetRealTime();
 
 printf("Period Physics[%f] Render[%f]\n",  physicsUpdateRate, renderUpdateRate);

  while (!this->userQuit)
  {
    currTime = this->GetRealTime();

    if (physicsUpdateRate == 0 || 
        currTime - this->prevPhysicsTime >= 1.0/physicsUpdateRate) 
    {
      this->simTime += step;

      // Update the physics engine
      if (!this->GetUserPause() && !this->GetUserStep() ||
          (this->GetUserStep() && this->GetUserStepInc()))
      {
        this->iterations++;
        this->pause=false;
        this->SetUserStepInc(!this->GetUserStepInc());
      }
      else
      {
        this->pauseTime += step;
        this->pause=true;
      }

      World::Instance()->Update();

      this->prevPhysicsTime = this->GetRealTime();
    }

    // Update the rendering
    if (renderUpdateRate == 0 || 
        currTime - this->prevRenderTime >= 1.0/renderUpdateRate)
    {
      this->GetRenderEngine()->Render(); 
      this->prevRenderTime = this->GetRealTime();
    }

    // Update the gui
    this->gui->Update();

    elapsedTime = (this->GetRealTime() - currTime);

    // Wait if we're going too fast
    if ( elapsedTime < 1.0/MAX_FRAME_RATE )
    {
      //printf("Too fast Elapsed Time[%f] [%f]\n",elapsedTime, (int)((1.0/MAX_FRAME_RATE - elapsedTime) * 1e6));
      usleep( (int)((1.0/MAX_FRAME_RATE - elapsedTime) * 1e6)  );
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
/// Gets our current GUI interface
Gui *Simulator::GetUI() const
{
  return this->gui;
}

////////////////////////////////////////////////////////////////////////////////
/// Gets local configuration for this computer
GazeboConfig *Simulator::GetGazeboConfig() const
{
  return this->gazeboConfig;
}

OgreAdaptor *Simulator::GetRenderEngine() const
{
  return this->renderEngine;
}

////////////////////////////////////////////////////////////////////////////////
// Return when this simulator is paused
bool Simulator::IsPaused() const
{
  return this->pause;
}


////////////////////////////////////////////////////////////////////////////////
/// Get the number of iterations of this simulation session
unsigned long Simulator::GetIterations() const
{
  return this->iterations;
}

////////////////////////////////////////////////////////////////////////////////
// Get the simulation time
double Simulator::GetSimTime() const
{
  return this->simTime;
}

////////////////////////////////////////////////////////////////////////////////
// Get the pause time
double Simulator::GetPauseTime() const
{
  return this->pauseTime;
}

////////////////////////////////////////////////////////////////////////////////
/// Get the start time
double Simulator::GetStartTime() const
{
  return this->startTime;
}

////////////////////////////////////////////////////////////////////////////////
/// Get the real time (elapsed time)
double Simulator::GetRealTime() const
{
  return this->GetWallTime() - this->startTime;
}

////////////////////////////////////////////////////////////////////////////////
/// Get the wall clock time
double Simulator::GetWallTime() const
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + tv.tv_usec * 1e-6;
}


void Simulator::SetUserQuit()
{
  //  this->Save("test.xml");
  this->userQuit = true;
}

////////////////////////////////////////////////////////////////////////////////
bool Simulator::GetUserPause() const
{
  return userPause;
}

////////////////////////////////////////////////////////////////////////////////
void Simulator::SetUserPause(bool pause)
{
  userPause = pause;
}

////////////////////////////////////////////////////////////////////////////////
bool Simulator::GetUserStep() const 
{
  return userStep;
}

////////////////////////////////////////////////////////////////////////////////
void Simulator::SetUserStep( bool step )
{
  userStep = step;
}

////////////////////////////////////////////////////////////////////////////////
bool Simulator::GetUserStepInc() const
{
  return userStepInc;
}

////////////////////////////////////////////////////////////////////////////////
void Simulator::SetUserStepInc(bool step)
{
  userStepInc = step;
}


// Move to GuiFactory?
void Simulator::SaveGui(XMLConfigNode *node)
{
  Vector2<int> size;
  XMLConfigNode* childNode = node->GetChild("gui");

  if (childNode)
  {
    size.x = this->gui->GetWidth();
    size.y = this->gui->GetHeight();
    childNode->SetValue("size", size);
    //TODO: node->SetValue("pos", Vector2<int>(x,y));
  }

}

