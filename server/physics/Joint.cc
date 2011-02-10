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
/* Desc: The base joint class
 * Author: Nate Keonig, Andrew Howard
 * Date: 21 May 2003
 * CVS: $Id$
 */

#include "Simulator.hh"
#include "RenderTypes.hh"
#include "Events.hh"
#include "PhysicsEngine.hh"
#include "GazeboError.hh"
#include "GazeboMessage.hh"
#include "Global.hh"
#include "Body.hh"
#include "Model.hh"
#include "World.hh"
#include "Joint.hh"

using namespace gazebo;

//////////////////////////////////////////////////////////////////////////////
// Constructor
Joint::Joint()
  : Common(NULL)
{
  this->AddType(JOINT);
  this->visualMsg = NULL;
  this->line1Msg = NULL;
  this->line2Msg = NULL;
  this->model = NULL;

  Param::Begin(&this->parameters);
  this->erpP = new ParamT<double>("erp",0.4,0);
  this->cfmP = new ParamT<double>("cfm",10e-3,0);
  this->stopKpP = new ParamT<double>("stopKp",1000000.0,0);
  this->stopKdP = new ParamT<double>("stopKd",1.0,0);
  this->body1NameP = new ParamT<std::string>("body1",std::string(),1);
  this->body2NameP = new ParamT<std::string>("body2",std::string(),1);
  this->anchorBodyNameP = new ParamT<std::string>("anchor",std::string(),0);
  this->anchorOffsetP = new ParamT<Vector3>("anchorOffset",Vector3(0,0,0), 0);
  this->provideFeedbackP = new ParamT<bool>("provideFeedback", false, 0);
  this->fudgeFactorP = new ParamT<double>( "fudgeFactor", 1.0, 0 );
  Param::End();

  this->body1 = NULL;
  this->body2 = NULL;

  Events::ConnectShowJointsSignal(boost::bind(&Joint::ToggleShowJoints, this) );
}


//////////////////////////////////////////////////////////////////////////////
// Desctructor
Joint::~Joint()
{
  Events::DisconnectShowJointsSignal(boost::bind(&Joint::ToggleShowJoints, this));

  if (this->visualMsg)
  {
    this->visualMsg->action = VisualMsg::DELETE;
    Simulator::Instance()->SendMessage(*this->visualMsg);
    delete this->visualMsg;
    this->visualMsg = NULL;
  }

  if (this->line1Msg)
  {
    this->line1Msg->action = VisualMsg::DELETE;
    Simulator::Instance()->SendMessage(*this->line1Msg);
    delete this->line1Msg;
    this->line1Msg = NULL;
  }

  if (this->line2Msg)
  {
    this->line2Msg->action = VisualMsg::DELETE;
    Simulator::Instance()->SendMessage(*this->line2Msg);
    delete this->line2Msg;
    this->line2Msg = NULL;
  }

  delete this->erpP;
  delete this->cfmP;
  delete this->stopKpP;
  delete this->stopKdP;
  delete this->body1NameP;
  delete this->body2NameP;
  delete this->anchorBodyNameP;
  delete this->anchorOffsetP;
  delete this->provideFeedbackP;
  delete this->fudgeFactorP;
}

//////////////////////////////////////////////////////////////////////////////
// Load a joint
void Joint::Load(XMLConfigNode *node)
{
  // Name the joint
  this->nameP->Load(node);

  this->body1NameP->Load(node);
  this->body2NameP->Load(node);
  this->anchorBodyNameP->Load(node);
  this->anchorOffsetP->Load(node);
  this->erpP->Load(node);
  this->cfmP->Load(node);
  this->stopKpP->Load(node);
  this->stopKdP->Load(node);
  this->provideFeedbackP->Load(node);
  this->fudgeFactorP->Load(node);

  std::ostringstream visname;

  if (this->model)
  {
    visname << this->model->GetScopedName() << "::" << this->GetName() << "_VISUAL";

    this->body1 = this->model->GetBody( **(this->body1NameP));
    this->body2 = this->model->GetBody(**(this->body2NameP));
    this->anchorBody = this->model->GetBody(**(this->anchorBodyNameP));
  }
  else
  {
    visname << this->GetName() << "_VISUAL";
    this->body1 = dynamic_cast<Body*>(this->GetWorld()->GetByName( **(this->body1NameP) ));
    this->body2 = dynamic_cast<Body*>(this->GetWorld()->GetByName( **(this->body2NameP) ));
    this->anchorBody = dynamic_cast<Body*>(this->GetWorld()->GetByName( **(this->anchorBodyNameP) ));
  }

  if (!this->body1 && this->body1NameP->GetValue() != std::string("world"))
    gzthrow("Couldn't Find Body[" + node->GetString("body1","",1));

  if (!this->body2 && this->body2NameP->GetValue() != std::string("world"))
    gzthrow("Couldn't Find Body[" + node->GetString("body2","",1));

  // setting anchor relative to gazebo body frame origin
  this->anchorPos = (Pose3d(**(this->anchorOffsetP),Quatern()) + this->anchorBody->GetWorldPose()).pos ;

  this->Attach(this->body1, this->body2);

  /// Add a renderable for the joint
  this->visualMsg = new VisualMsg();
  this->visualMsg->parentId = this->GetName();
  this->visualMsg->id = visname.str();
  this->visualMsg->render = RENDERING_MESH_RESOURCE;
  this->visualMsg->pose.pos = this->anchorPos;
  this->visualMsg->castShadows = false;
  this->visualMsg->mesh = "joint_anchor";
  this->visualMsg->material = "Gazebo/JointAnchor";
  this->visualMsg->visible = false;
  
  this->line1Msg = new VisualMsg();
  this->line1Msg->parentId = this->visualMsg->id;
  this->line1Msg->id = this->visualMsg->id + "/line1";
  this->line1Msg->render = RENDERING_LINE_LIST;
  this->line1Msg->material = "Gazebo/BlueGlow";
  this->line1Msg->points.push_back(Vector3(0,0,0));
  this->line1Msg->points.push_back(Vector3(0,0,0));

  this->line2Msg = new VisualMsg();
  this->line2Msg->parentId = this->visualMsg->id;
  this->line2Msg->id = this->visualMsg->id + "line2";
  this->line2Msg->render = RENDERING_LINE_LIST;
  this->line2Msg->material = "Gazebo/BlueGlow";
  this->line2Msg->points.push_back(Vector3(0,0,0));
  this->line2Msg->points.push_back(Vector3(0,0,0));

  // Set the anchor vector
  if (this->anchorBody)
  {
    this->SetAnchor(0, this->anchorPos);
  }
}

////////////////////////////////////////////////////////////////////////////////
/// Save a joint to a stream in XML format
void Joint::Save(std::string &prefix, std::ostream &stream)
{
  std::string typeName = EntityTypename[ (int)this->GetLeafType() ];

  stream << prefix << "<joint:" << typeName << " name=\"" << **(this->nameP) << "\">\n";
  stream << prefix << "  " << *(this->body1NameP) << "\n";
  stream << prefix << "  " << *(this->body2NameP) << "\n";
  stream << prefix << "  " << *(this->anchorBodyNameP) << "\n";
  stream << prefix << "  " << *(this->anchorOffsetP) << "\n";

  stream << prefix << "  " << *(this->erpP) << "\n";
  stream << prefix << "  " << *(this->cfmP) << "\n";
  stream << prefix << "  " << *(this->fudgeFactorP) << "\n";

  this->SaveJoint(prefix, stream);

  std::string p = prefix + "  ";

  stream << prefix << "</joint:" << typeName << ">\n";
}

////////////////////////////////////////////////////////////////////////////////
/// Update the joint
void Joint::Update()
{
  this->jointUpdateSignal();

  //TODO: Evaluate impact of this code on performance
  if (this->visualMsg && this->visualMsg->visible)
  {
    this->visualMsg->pose.pos = this->anchorPos;

    if (this->body1) 
      this->line1Msg->points[1] = this->body1->GetWorldPose().pos - this->anchorPos;

    if (this->body2)
      this->line2Msg->points[1] = this->body2->GetWorldPose().pos - this->anchorPos;

    Simulator::Instance()->SendMessage( *this->visualMsg );
    Simulator::Instance()->SendMessage( *this->line1Msg );
    Simulator::Instance()->SendMessage( *this->line2Msg );
  }
}

//////////////////////////////////////////////////////////////////////////////
// Toggle joint visibility
void Joint::ToggleShowJoints()
{
  if (this->visualMsg)
  {
    this->visualMsg->visible = !this->visualMsg->visible;
    Simulator::Instance()->SendMessage( *this->visualMsg );
  }
}

//////////////////////////////////////////////////////////////////////////////
// Set the joint to show visuals
void Joint::ShowJoints(bool s)
{
  if (this->visualMsg)
  {
    this->visualMsg->visible = s;
    Simulator::Instance()->SendMessage( *this->visualMsg );
  }
}

//////////////////////////////////////////////////////////////////////////////
/// Reset the joint
void Joint::Reset()
{
  this->SetForce(0,0);
  this->SetMaxForce(0,0);
  this->SetVelocity(0,0);
}

//////////////////////////////////////////////////////////////////////////////
/// Attach the two bodies with this joint
void Joint::Attach( Body *one, Body *two )
{
  this->body1 = one;
  this->body2 = two;
}


//////////////////////////////////////////////////////////////////////////////
// Set the model this joint belongs too
void Joint::SetModel(Model *model)
{
  this->model = model;
  this->SetWorld(this->model->GetWorld());
}
