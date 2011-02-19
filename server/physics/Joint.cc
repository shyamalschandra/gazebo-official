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
/* Desc: The base joint class
 * Author: Nate Keonig, Andrew Howard
 * Date: 21 May 2003
 * CVS: $Id$
 */

#include "PhysicsEngine.hh"
#include "OgreVisual.hh"
#include "OgreCreator.hh"
#include "OgreDynamicLines.hh"
#include "Global.hh"
#include "Body.hh"
#include "Model.hh"
#include "World.hh"
#include "Joint.hh"

using namespace gazebo;

std::string Joint::TypeNames[Joint::TYPE_COUNT] = {"slider", "hinge", "hinge2", "ball", "universal"};

//////////////////////////////////////////////////////////////////////////////
// Constructor
Joint::Joint()
  : Common()
{
  this->visual = NULL;
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
  this->anchorBody = NULL;
 
  this->physics = World::Instance()->GetPhysicsEngine();

  World::Instance()->ConnectShowJointsSignal( 
      boost::bind(&Joint::ShowJoints, this, _1) );
}


//////////////////////////////////////////////////////////////////////////////
// Desctructor
Joint::~Joint()
{
  World::Instance()->DisconnectShowJointsSignal( 
      boost::bind(&Joint::ShowJoints, this, _1) );

  if (this->visual)
  {
    OgreCreator::Instance()->DeleteVisual( this->visual );
    this->visual = NULL;
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
// Get the type of the joint
Joint::Type Joint::GetType() const
{
  return this->type;
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
    if ( !(**this->anchorBodyNameP).empty() )
      this->anchorBody = this->model->GetBody(**(this->anchorBodyNameP));
    else
      gzmsg(0) << "Warning: Joint[" << this->GetName() << "], anchorBody is not set\n";
  }
  else
  {
    visname << this->GetName() << "_VISUAL";
    this->body1 = dynamic_cast<Body*>(World::Instance()->GetEntityByName( **(this->body1NameP) ));
    this->body2 = dynamic_cast<Body*>(World::Instance()->GetEntityByName( **(this->body2NameP) ));
    this->anchorBody = dynamic_cast<Body*>(World::Instance()->GetEntityByName( **(this->anchorBodyNameP) ));
  }

  if (!this->body1 && this->body1NameP->GetValue() != std::string("world"))
    gzthrow("Couldn't Find Body[" + node->GetString("body1","",1));

  if (!this->body2 && this->body2NameP->GetValue() != std::string("world"))
    gzthrow("Couldn't Find Body[" + node->GetString("body2","",1));

  // setting anchor relative to gazebo body frame origin
  if (this->anchorBody)
      this->anchorPos = (Pose3d(**(this->anchorOffsetP),Quatern()) + this->anchorBody->GetWorldPose()).pos;

  this->Attach(this->body1, this->body2);

  /// Add a renderable for the joint
  this->visual = OgreCreator::Instance()->CreateVisual(
      visname.str(), NULL);

  if (this->visual)
  {
    this->visual->SetPosition(this->anchorPos);
    this->visual->SetCastShadows(false);
    this->visual->AttachMesh("joint_anchor");
    this->visual->SetMaterial("Gazebo/JointAnchor");
    this->visual->SetVisible(false);

    this->line1 = OgreCreator::Instance()->CreateDynamicLine(OgreDynamicRenderable::OT_LINE_LIST);
    this->line2 = OgreCreator::Instance()->CreateDynamicLine(OgreDynamicRenderable::OT_LINE_LIST);

    this->line1->setMaterial("Gazebo/BlueEmissive");
    this->line2->setMaterial("Gazebo/BlueEmissive");

    this->visual->AttachObject(this->line1);
    this->visual->AttachObject(this->line2);

    this->line1->AddPoint(Vector3(0,0,0));
    this->line1->AddPoint(Vector3(0,0,0));
    this->line2->AddPoint(Vector3(0,0,0));
    this->line2->AddPoint(Vector3(0,0,0));
  }

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
  std::string typeName;

  switch (this->type)
  {
    case SLIDER: 
      typeName="slider"; 
      break;
    case HINGE: 
      typeName = "hinge"; 
      break;
    case HINGE2: 
      typeName = "hinge2"; 
      break;
    case BALL: 
      typeName = "ball"; 
      break;
    case UNIVERSAL: 
      typeName = "universal"; 
      break;
    default:
      gzthrow("Unable to save joint of type[" << this->type << "]\n");
      break;
  }

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
  if (this->visual && this->visual->GetVisible())
  {
    if (this->GetName() == "left_paddle_joint")
      std::cout << "Joint[" << this->GetName() << "] Angle[" << this->GetAngle(0) << "]\n";
    this->anchorPos = (Pose3d(**(this->anchorOffsetP),Quatern()) + 
        this->anchorBody->GetWorldPose()).pos;

    this->visual->SetPosition(this->anchorPos);

    if (this->body1) 
      this->line1->SetPoint(1, this->body1->GetWorldPose().pos - this->anchorPos);

    if (this->body2)
      this->line2->SetPoint(1, this->body2->GetWorldPose().pos - this->anchorPos);
  }
}

//////////////////////////////////////////////////////////////////////////////
// Set the joint to show visuals
void Joint::ShowJoints(bool s)
{
  if (this->visual)
    this->visual->SetVisible(s);
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
}
