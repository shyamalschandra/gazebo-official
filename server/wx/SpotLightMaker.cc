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
#include <iostream>

#include "Events.hh"
#include "Camera.hh"
#include "MouseEvent.hh"
#include "Simulator.hh"
#include "Visual.hh"
#include "World.hh"
#include "SpotLightMaker.hh"

using namespace gazebo;

unsigned int SpotLightMaker::counter = 0;

SpotLightMaker::SpotLightMaker()
  : EntityMaker()
{
  this->state = 0;

  this->msg.set_type( msgs::Light::SPOT );
  Message::Set(this->msg.mutable_diffuse(), Color(0.5, 0.5, 0.5, 1));
  Message::Set(this->msg.mutable_specular(), Color(0.1, 0.1, 0.1, 1));
  Message::Set(this->msg.mutable_attenuation(), Vector3(0.5, 0.01, 0.0));
  Message::Set(this->msg.mutable_direction(), Vector3(0,0,-1));
  this->msg.set_range( 20 );
  this->msg.set_cast_shadows( false );
  this->msg.set_spot_inner_angle( 20 );
  this->msg.set_spot_outer_angle( 40 );
  this->msg.set_spot_falloff( 1.0 );
}

SpotLightMaker::~SpotLightMaker()
{
}

void SpotLightMaker::Start(Scene *scene)
{
  std::ostringstream stream;
  stream << "user_spot_light_" << counter++;
  this->msg.mutable_header()->set_str_id( stream.str() );
  this->state = 1;
}

void SpotLightMaker::Stop()
{
  this->state = 0;
  Events::moveModeSignal(true);
}

bool SpotLightMaker::IsActive() const
{
  return this->state > 0;
}

void SpotLightMaker::MousePushCB(const MouseEvent &event)
{
  if (this->state == 0)
    return;

  Vector3 norm;
  norm.Set(0,0,1);

  Message::Set(this->msg.mutable_pose()->mutable_position(), event.camera->GetWorldPointOnPlane(event.pressPos.x, event.pressPos.y, norm, 0) );
  this->msg.mutable_pose()->mutable_position()->set_z( 1.0 );
}

void SpotLightMaker::MouseReleaseCB(const MouseEvent &event)
{
  if (this->state == 0)
    return;

  this->state++;

  this->CreateTheEntity();
  this->Stop();
}

void SpotLightMaker::MouseDragCB(const MouseEvent & /*event*/)
{
}

void SpotLightMaker::CreateTheEntity()
{
  Message::CreationStamp(this->msg);
  Simulator::Instance()->SendMessage(this->msg);
}
