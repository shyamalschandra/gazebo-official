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
#include <sstream>

#include "msgs/msgs.h"
#include "gui/GuiEvents.hh"
#include "common/MouseEvent.hh"
#include "math/Quaternion.hh"

#include "rendering/UserCamera.hh"

#include "transport/Publisher.hh"

#include "gui/CylinderMaker.hh"

using namespace gazebo;
using namespace gui;

unsigned int CylinderMaker::counter = 0;

CylinderMaker::CylinderMaker()
  : EntityMaker()
{
  this->state = 0;
  this->visualMsg = new msgs::Visual();
  this->visualMsg->mutable_geometry()->set_type(  msgs::Geometry::CYLINDER );
  this->visualMsg->mutable_material()->set_script( "Gazebo/TurquoiseGlowOutline" );
  msgs::Set(this->visualMsg->mutable_pose()->mutable_orientation(), math::Quaternion());
}

CylinderMaker::~CylinderMaker()
{
  delete this->visualMsg;
}

void CylinderMaker::Start(const rendering::UserCameraPtr camera)
                          //const CreateCallback &cb)
{
  //this->createCB = cb;
  this->camera = camera;
  std::ostringstream stream;
  stream <<  "user_cylinder_" << counter++;
  this->visualMsg->set_name( stream.str() );
  this->state = 1;
}

void CylinderMaker::Stop()
{
  msgs::Request *msg = msgs::CreateRequest("entity_delete", this->visualMsg->name());

  this->requestPub->Publish(*msg);
  delete msg;

  this->state = 0;
  gui::Events::moveModeSignal(true);
}

bool CylinderMaker::IsActive() const
{
  return this->state > 0;
}

void CylinderMaker::OnMousePush(const common::MouseEvent &event)
{
  if (this->state == 0)
    return;

  this->mousePushPos = event.pressPos;
}

void CylinderMaker::OnMouseRelease(const common::MouseEvent &/*_event*/)
{
  if (this->state == 0)
    return;

  this->state++;

  if (this->state == 3)
  {
    this->CreateTheEntity();
    this->Stop();
  }
}

void CylinderMaker::OnMouseDrag(const common::MouseEvent &event)
{
  if (this->state == 0)
    return;

  math::Vector3 norm;
  math::Vector3 p1, p2;

  if (this->state == 1)
    norm.Set(0,0,1);
  else if (this->state == 2)
    norm.Set(1,0,0);

  p1 = this->camera->GetWorldPointOnPlane(this->mousePushPos.x, 
                                          this->mousePushPos.y, norm, 0);
  p1 = this->GetSnappedPoint( p1 );

  p2 = this->camera->GetWorldPointOnPlane(event.pos.x, event.pos.y ,norm, 0);
  p2 = this->GetSnappedPoint( p2 );

  if (this->state == 1)
    msgs::Set(this->visualMsg->mutable_pose()->mutable_position(), p1 );

  math::Vector3 p( this->visualMsg->pose().position().x(),
             this->visualMsg->pose().position().y(),
             this->visualMsg->pose().position().z() );

  math::Vector3 scale;

  if (this->state == 1)
  {
    double dist = p1.Distance(p2);

    this->visualMsg->mutable_geometry()->mutable_cylinder()->set_radius(dist);
    this->visualMsg->mutable_geometry()->mutable_cylinder()->set_length(0.01);
  }
  else
  {
    this->visualMsg->mutable_geometry()->mutable_cylinder()->set_length(
        (this->mousePushPos.y - event.pos.y)*0.01);

    p.z = ((this->mousePushPos.y - event.pos.y)*0.01)/2.0;
  }

  msgs::Set(this->visualMsg->mutable_pose()->mutable_position(), p );
  this->visPub->Publish(*this->visualMsg);
}

void CylinderMaker::CreateTheEntity()
{
  msgs::Factory msg;
  std::ostringstream newModelStr;


  newModelStr << "<gazebo version='1.0'>\
    <model name='" << this->visualMsg->name() << "_model'>\
      <origin pose='" << this->visualMsg->pose().position().x() << " " 
                      << this->visualMsg->pose().position().y() << " " 
                      << this->visualMsg->pose().position().z() << " 0 0 0'/>\
      <link name='body'>\
        <inertial mass='1.0'>\
            <inertia ixx='1' ixy='0' ixz='0' iyy='1' iyz='0' izz='1'/>\
        </inertial>\
        <collision name='geom'>\
          <geometry>\
            <cylinder radius='" << this->visualMsg->geometry().cylinder().radius() << "'\
                      length='" << this->visualMsg->geometry().cylinder().length() << "'/>\
          </geometry>\
        </collision>\
        <visual name='visual' cast_shadows='true'>\
          <geometry>\
            <cylinder radius='" << this->visualMsg->geometry().cylinder().radius() << "'\
                      length='" << this->visualMsg->geometry().cylinder().length() << "'/>\
          </geometry>\
          <material script='Gazebo/Grey'/>\
        </visual>\
      </link>\
    </model>\
    </gazebo>";

  msg.set_sdf( newModelStr.str() );

  msgs::Request *requestMsg = msgs::CreateRequest("entity_delete", this->visualMsg->name());
  this->requestPub->Publish(*requestMsg);
  delete requestMsg;

  this->makerPub->Publish(msg);
}

