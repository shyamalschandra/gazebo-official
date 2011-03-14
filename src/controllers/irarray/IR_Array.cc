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
/*
 * Desc: IR array controller.
 * Author: Wenguo Liu
 * Date: 24 Apr 2008
 */

#include <algorithm>
#include <assert.h>

#include "Sensor.hh"
#include "common/Global.hh"
#include "common/XMLConfig.hh"
#include "Model.hh"
#include "World.hh"
#include "Simulator.hh"
#include "common/GazeboError.hh"
#include "ControllerFactory.hh"
#include "IRSensor.hh"
#include "IR_Array.hh"

using namespace gazebo;

GZ_REGISTER_STATIC_CONTROLLER("irarray", IR_Array);

////////////////////////////////////////////////////////////////////////////////
// Constructor
IR_Array::IR_Array(Entity *parent)
    : Controller(parent)
{
   this->myParent = dynamic_cast<IRSensor*>(this->parent);

  if (!this->myParent)
    gzthrow("IR_Array controller requires a IRSensor as its parent");

  this->irIface = NULL;
  
  
}

////////////////////////////////////////////////////////////////////////////////
// Destructor
IR_Array::~IR_Array()
{
}

////////////////////////////////////////////////////////////////////////////////
// Load the controller
void IR_Array::LoadChild(XMLConfigNode *node)
{
  this->irIface = dynamic_cast<libgazebo::IRIface*>(this->GetIface("irarray"));
}

////////////////////////////////////////////////////////////////////////////////
// Initialize the controller
void IR_Array::InitChild()
{
}

////////////////////////////////////////////////////////////////////////////////
// Update the controller
void IR_Array::UpdateChild()
{
  /*bool opened = false;

  if (this->irIface->Lock(1))
  {
    opened = this->irIface->GetOpenCount() > 0;
    this->irIface->Unlock();
  }
*/
  //if (opened)
  {
    this->PutIRData();
  }

}

////////////////////////////////////////////////////////////////////////////////
// Finalize the controller
void IR_Array::FiniChild()
{
}

////////////////////////////////////////////////////////////////////////////////
// Put laser data to the interface
void IR_Array::PutIRData()
{
  if (this->irIface->Lock(1))
  {
    // Data timestamp
    this->irIface->data->head.time = this->myParent->GetWorld()->GetSimTime().Double();
    
    this->irIface->data->ir_count = this->myParent->GetIRCount();
    
    this->irIface->data->range_count = this->myParent->GetIRCount();
    
    for(int i=0;i<this->irIface->data->ir_count;i++)
    {
       this->irIface->data->ranges[i] = this->myParent->GetRange(i);
       Pose3d pose = this->myParent->GetPose(i);

       this->irIface->data->poses[i].pos.x = pose.pos.x;
       this->irIface->data->poses[i].pos.y = pose.pos.y;
       this->irIface->data->poses[i].pos.z = pose.pos.z;

       this->irIface->data->poses[i].roll = pose.rot.GetRoll();
       this->irIface->data->poses[i].pitch = pose.rot.GetPitch();
       this->irIface->data->poses[i].yaw = pose.rot.GetYaw();
    }

    this->irIface->Unlock();

    // New data is available
    this->irIface->Post();
  }
}


