/*
 * Copyright 2012 Open Source Robotics Foundation
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
#include <boost/filesystem.hpp>

#include "gazebo/common/Exception.hh"

#include "gazebo/rendering/UserCamera.hh"
#include "gazebo/rendering/Visual.hh"
#include "gazebo/rendering/Scene.hh"

#include "gazebo/math/Quaternion.hh"

#include "gazebo/transport/Publisher.hh"
#include "gazebo/transport/Node.hh"

#include "gazebo/physics/Inertial.hh"

#include "gazebo/gui/MouseEventHandler.hh"
#include "gazebo/gui/GuiEvents.hh"
#include "gazebo/gui/Gui.hh"

#include "gazebo/gui/model/PartGeneralTab.hh"
#include "gazebo/gui/model/PartInspector.hh"
#include "gazebo/gui/model/JointMaker.hh"
#include "gazebo/gui/model/ModelCreator.hh"


using namespace gazebo;
using namespace gui;

/////////////////////////////////////////////////
ModelCreator::ModelCreator()
{
  this->modelName = "";

  this->modelTemplateSDF.reset(new sdf::SDF);
  this->modelTemplateSDF->SetFromString(this->GetTemplateSDFString());

  this->boxCounter = 0;
  this->cylinderCounter = 0;
  this->sphereCounter = 0;

  this->node = transport::NodePtr(new transport::Node());
  this->node->Init();
  this->makerPub = this->node->Advertise<msgs::Factory>("~/factory");
  this->requestPub = this->node->Advertise<msgs::Request>("~/request");

  this->jointMaker = new JointMaker();

  this->Reset();
}

/////////////////////////////////////////////////
ModelCreator::~ModelCreator()
{
}

/////////////////////////////////////////////////
std::string ModelCreator::CreateModel()
{
  this->Reset();
  return this->modelName;
}

/////////////////////////////////////////////////
void ModelCreator::AddJoint(JointMaker::JointType _type)
{
  if (this->jointMaker)
    this->jointMaker->CreateJoint(_type);

}

/////////////////////////////////////////////////
std::string ModelCreator::AddBox(const math::Vector3 &_size,
    const math::Pose &_pose)
{
  if (!this->modelVisual)
    this->Reset();

  std::ostringstream linkNameStream;
  linkNameStream << "unit_box_" << this->boxCounter++;
  std::string linkName = linkNameStream.str();

  rendering::VisualPtr linkVisual(new rendering::Visual(linkName,
      this->modelVisual));
  linkVisual->Load();

  std::ostringstream visualName;
  visualName << linkName << "_visual";
  rendering::VisualPtr visVisual(new rendering::Visual(visualName.str(),
      linkVisual));
  sdf::ElementPtr visualElem =  this->modelTemplateSDF->root
      ->GetElement("model")->GetElement("link")->GetElement("visual");
  visualElem->GetElement("material")->GetElement("script")
      ->GetElement("name")->Set("Gazebo/OrangeTransparent");

  sdf::ElementPtr geomElem =  visualElem->GetElement("geometry");
  geomElem->ClearElements();
  ((geomElem->AddElement("box"))->AddElement("size"))->Set(_size);

  visVisual->Load(visualElem);

  linkVisual->SetPose(_pose);
  if (_pose == math::Pose::Zero)
  {
    linkVisual->SetPosition(math::Vector3(_pose.pos.x, _pose.pos.y,
    _pose.pos.z + _size.z/2));
  }

  this->CreatePart(visVisual);

  this->mouseVisual = linkVisual;

  return linkName;
}

/////////////////////////////////////////////////
std::string ModelCreator::AddSphere(double _radius,
    const math::Pose &_pose)
{
  if (!this->modelVisual)
    this->Reset();

  std::ostringstream linkNameStream;
  linkNameStream << "unit_sphere_" << this->sphereCounter++;
  std::string linkName = linkNameStream.str();

  rendering::VisualPtr linkVisual(new rendering::Visual(
      linkName, this->modelVisual));
  linkVisual->Load();

  std::ostringstream visualName;
  visualName << linkName << "_visual";
  rendering::VisualPtr visVisual(new rendering::Visual(visualName.str(),
        linkVisual));
  sdf::ElementPtr visualElem =  this->modelTemplateSDF->root
      ->GetElement("model")->GetElement("link")->GetElement("visual");
  visualElem->GetElement("material")->GetElement("script")
      ->GetElement("name")->Set("Gazebo/OrangeTransparent");

  sdf::ElementPtr geomElem =  visualElem->GetElement("geometry");
  geomElem->ClearElements();
  ((geomElem->AddElement("sphere"))->GetElement("radius"))->Set(_radius);

  visVisual->Load(visualElem);

  linkVisual->SetPose(_pose);
  if (_pose == math::Pose::Zero)
  {
    linkVisual->SetPosition(math::Vector3(_pose.pos.x, _pose.pos.y,
    _pose.pos.z + _radius));
  }

  this->CreatePart(visVisual);
  this->mouseVisual = linkVisual;

  return linkName;
}

/////////////////////////////////////////////////
std::string ModelCreator::AddCylinder(double _radius, double _length,
    const math::Pose &_pose)
{
  if (!this->modelVisual)
    this->Reset();

  std::ostringstream linkNameStream;
  linkNameStream << "unit_cylinder_" << this->cylinderCounter++;
  std::string linkName = linkNameStream.str();

  rendering::VisualPtr linkVisual(new rendering::Visual(
      linkName, this->modelVisual));
  linkVisual->Load();

  std::ostringstream visualName;
  visualName << linkName << "_visual";
  rendering::VisualPtr visVisual(new rendering::Visual(visualName.str(),
        linkVisual));
  sdf::ElementPtr visualElem =  this->modelTemplateSDF->root
      ->GetElement("model")->GetElement("link")->GetElement("visual");
  visualElem->GetElement("material")->GetElement("script")
      ->GetElement("name")->Set("Gazebo/OrangeTransparent");

  sdf::ElementPtr geomElem =  visualElem->GetElement("geometry");
  geomElem->ClearElements();
  sdf::ElementPtr cylinderElem = geomElem->AddElement("cylinder");
  (cylinderElem->GetElement("radius"))->Set(_radius);
  (cylinderElem->GetElement("length"))->Set(_length);

  visVisual->Load(visualElem);

  linkVisual->SetPose(_pose);
  if (_pose == math::Pose::Zero)
  {
    linkVisual->SetPosition(math::Vector3(_pose.pos.x, _pose.pos.y,
    _pose.pos.z + _length/2));
  }

  this->CreatePart(visVisual);
  this->mouseVisual = linkVisual;

  return linkName;
}

/////////////////////////////////////////////////
std::string ModelCreator::AddCustom(std::string _path, const math::Vector3 &_scale,
    const math::Pose &_pose)
{
  if (!this->modelVisual)
    this->Reset();

  std::string path = _path;

  std::ostringstream linkNameStream;
  linkNameStream << "custom_" << this->customCounter++;
  std::string linkName = linkNameStream.str();

  rendering::VisualPtr linkVisual(new rendering::Visual(this->modelName + "::" +
        linkName, this->modelVisual));
  linkVisual->Load();

  std::ostringstream visualName;
  visualName << linkName << "_visual";
  rendering::VisualPtr visVisual(new rendering::Visual(visualName.str(),
        linkVisual));
  sdf::ElementPtr visualElem =  this->modelTemplateSDF->root
      ->GetElement("model")->GetElement("link")->GetElement("visual");
  visualElem->GetElement("material")->GetElement("script")
      ->GetElement("name")->Set("Gazebo/OrangeTransparent");

  sdf::ElementPtr geomElem =  visualElem->GetElement("geometry");
  geomElem->ClearElements();
  sdf::ElementPtr meshElem = geomElem->AddElement("mesh");
  meshElem->GetElement("scale")->Set(_scale);
  meshElem->GetElement("uri")->Set(path);
  visVisual->Load(visualElem);

  linkVisual->SetPose(_pose);
  if (_pose == math::Pose::Zero)
  {
    linkVisual->SetPosition(math::Vector3(_pose.pos.x, _pose.pos.y,
    _pose.pos.z + _scale.z/2));
  }

  this->CreatePart(visVisual);
  this->mouseVisual = linkVisual;

  return linkName;
}

/////////////////////////////////////////////////
void ModelCreator::CreatePart(rendering::VisualPtr _visual)
{
  PartData *part = new PartData;
  part->name = _visual->GetName();
  part->visuals.push_back(_visual);

  part->gravity = true;
  part->selfCollide = false;
  part->kinematic = false;
  part->pose = _visual->GetParent()->GetWorldPose();

  part->inspector = new PartInspector;
  part->inspector->setModal(false);
  connect(part->inspector, SIGNAL(Applied()),
      part, SLOT(OnApply()));


  part->inertial = new physics::Inertial;
  part->sensorData = new SensorData;

  this->allParts[part->name] = part;
}

/////////////////////////////////////////////////
void ModelCreator::RemovePart(const std::string &_partName)
{
  if (!this->modelVisual)
    this->Reset();

  PartData *part = this->allParts[_partName];
  if (!part)
  {
    gzerr << _partName << " does not exist\n";
    return;
  }

  for (unsigned int i = 0; i < part->visuals.size(); ++i)
  {
    rendering::VisualPtr vis = part->visuals[i];
    rendering::VisualPtr visParent = vis->GetParent();
    rendering::ScenePtr scene = vis->GetScene();
    scene->RemoveVisual(vis);
    if (visParent)
      scene->RemoveVisual(visParent);
  }

  delete part->inspector;
  delete part->inertial;
  delete part->sensorData;

  this->allParts.erase(_partName);
}

/////////////////////////////////////////////////
void ModelCreator::Reset()
{
  if (!gui::get_active_camera() ||
      !gui::get_active_camera()->GetScene())
    return;

  this->jointMaker->Reset();

  this->modelName = "default";

  rendering::ScenePtr scene = gui::get_active_camera()->GetScene();

  this->isStatic = false;
  this->autoDisable = true;

  while (this->allParts.size() > 0)
    this->RemovePart(this->allParts.begin()->first);
  this->allParts.clear();

  if (this->modelVisual)
    scene->RemoveVisual(this->modelVisual);

  this->modelVisual.reset(new rendering::Visual(this->modelName,
      scene->GetWorldVisual()));

  this->modelVisual->Load();
  this->modelPose = math::Pose::Zero;
  this->modelVisual->SetPose(this->modelPose);
  scene->AddVisual(this->modelVisual);

  MouseEventHandler::Instance()->AddPressFilter("model_part",
      boost::bind(&ModelCreator::OnMousePressPart, this, _1));

  MouseEventHandler::Instance()->AddMoveFilter("model_part",
      boost::bind(&ModelCreator::OnMouseMovePart, this, _1));

  MouseEventHandler::Instance()->AddDoubleClickFilter("model_part",
      boost::bind(&ModelCreator::OnMouseDoubleClickPart, this, _1));
}

/////////////////////////////////////////////////
void ModelCreator::SetModelName(const std::string &_modelName)
{
  this->modelName = _modelName;
}

/////////////////////////////////////////////////
std::string ModelCreator::GetModelName() const
{
  return this->modelName;
}

/////////////////////////////////////////////////
void ModelCreator::SetStatic(bool _static)
{
  this->isStatic = _static;
}

/////////////////////////////////////////////////
void ModelCreator::SetAutoDisable(bool _auto)
{
  this->autoDisable = _auto;
}

/////////////////////////////////////////////////
void ModelCreator::SaveToSDF(const std::string &_savePath)
{
  std::ofstream savefile;
  boost::filesystem::path path;
  path = boost::filesystem::operator/(_savePath, this->modelName + ".sdf");
  savefile.open(path.string().c_str());
  savefile << this->modelSDF->ToString();
  savefile.close();
}

/////////////////////////////////////////////////
void ModelCreator::FinishModel()
{
  event::Events::setSelectedEntity("", "normal");
  this->Reset();
  this->CreateTheEntity();
//  this->Stop();
}

/////////////////////////////////////////////////
void ModelCreator::CreateTheEntity()
{
  msgs::Factory msg;
  msg.set_sdf(this->modelSDF->ToString());
  this->makerPub->Publish(msg);
}

/////////////////////////////////////////////////
std::string ModelCreator::GetTemplateSDFString()
{
  std::ostringstream newModelStr;
  newModelStr << "<sdf version ='" << SDF_VERSION << "'>"
    << "<model name='template_model'>"
    << "<pose>0 0 0.0 0 0 0</pose>"
    << "<link name ='link'>"
    <<   "<visual name ='visual'>"
    <<     "<pose>0 0 0.0 0 0 0</pose>"
    <<     "<geometry>"
    <<       "<box>"
    <<         "<size>1.0 1.0 1.0</size>"
    <<       "</box>"
    <<     "</geometry>"
    <<     "<material>"
    <<       "<script>"
    <<         "<uri>file://media/materials/scripts/gazebo.material</uri>"
    <<         "<name>Gazebo/Grey</name>"
    <<       "</script>"
    <<     "</material>"
    <<   "</visual>"
    << "</link>"
    << "<static>true</static>"
    << "</model>"
    << "</sdf>";

  return newModelStr.str();
}


/////////////////////////////////////////////////
void ModelCreator::AddPart(PartType _type)
{
  this->addPartType = _type;
  if (_type != PART_NONE)
  {
    switch (_type)
    {
      case PART_BOX:
      {
        this->AddBox();
        break;
      }
      case PART_SPHERE:
      {
        this->AddSphere();
        break;
      }
      case PART_CYLINDER:
      {
        this->AddCylinder();
        break;
      }
      default:
        break;
    }
  }
  else
  {
  }
}

/////////////////////////////////////////////////
bool ModelCreator::OnMousePressPart(const common::MouseEvent &_event)
{
  if (!this->mouseVisual || _event.button != common::MouseEvent::LEFT)
    return false;
  else
  {
    emit PartAdded();
    this->mouseVisual.reset();
    this->AddPart(PART_NONE);
    return true;
  }
}

/////////////////////////////////////////////////
bool ModelCreator::OnMouseMovePart(const common::MouseEvent &_event)
{
  if (!this->mouseVisual)
    return false;

  if (!gui::get_active_camera())
    return false;

  math::Pose pose = this->mouseVisual->GetWorldPose();

  math::Vector3 origin1, dir1, p1;
  math::Vector3 origin2, dir2, p2;


  // Cast two rays from the camera into the world
  gui::get_active_camera()->GetCameraToViewportRay(_event.pos.x, _event.pos.y,
      origin1, dir1);

  // Compute the distance from the camera to plane of translation
  math::Plane plane(math::Vector3(0, 0, 1), 0);

  double dist1 = plane.Distance(origin1, dir1);

  // Compute two points on the plane. The first point is the current
  // mouse position, the second is the previous mouse position
  p1 = origin1 + dir1 * dist1;
  pose.pos = p1;

  if (!_event.shift)
  {
    if (ceil(pose.pos.x) - pose.pos.x <= .4)
      pose.pos.x = ceil(pose.pos.x);
    else if (pose.pos.x - floor(pose.pos.x) <= .4)
      pose.pos.x = floor(pose.pos.x);

    if (ceil(pose.pos.y) - pose.pos.y <= .4)
      pose.pos.y = ceil(pose.pos.y);
    else if (pose.pos.y - floor(pose.pos.y) <= .4)
      pose.pos.y = floor(pose.pos.y);

  }
  pose.pos.z = this->mouseVisual->GetWorldPose().pos.z;

  this->mouseVisual->SetWorldPose(pose);

  return true;
}


/////////////////////////////////////////////////
bool ModelCreator::OnMouseDoubleClickPart(const common::MouseEvent &_event)
{
  rendering::VisualPtr vis = gui::get_active_camera()->GetVisual(_event.pos);
  if (vis)
  {
    if (this->allParts.find(vis->GetName()) !=
        this->allParts.end())
    {

      PartData *part = this->allParts[vis->GetName()];
      PartGeneralTab *general = part->inspector->GetGeneral();
      general->SetGravity(part->gravity);
      general->SetSelfCollide(part->selfCollide);
      general->SetKinematic(part->kinematic);
      general->SetPose(part->pose);
      general->SetMass(part->inertial->GetMass());
      general->SetInertialPose(part->inertial->GetPose());
      general->SetInertia(part->inertial->GetIXX(), part->inertial->GetIYY(),
          part->inertial->GetIZZ(), part->inertial->GetIXY(),
          part->inertial->GetIXZ(), part->inertial->GetIYZ());

      part->inspector->show();
      gzerr << " got model part " << vis->GetName() <<  std::endl;

      return true;
    }
  }
  return false;
}

/////////////////////////////////////////////////
JointMaker *ModelCreator::GetJointMaker() const
{
  return this->jointMaker;
}

/////////////////////////////////////////////////
void ModelCreator::GenerateSDF()
{
  sdf::ElementPtr modelElem;
  sdf::ElementPtr linkElem;
  sdf::ElementPtr visualElem;
  sdf::ElementPtr collisionElem;

  this->modelSDF.reset(new sdf::SDF);
  this->modelSDF->SetFromString(this->GetTemplateSDFString());

  modelElem = this->modelSDF->root->GetElement("model");

  linkElem = modelElem->GetElement("link");
  sdf::ElementPtr templateLinkElem = linkElem->Clone();
  sdf::ElementPtr templateVisualElem = templateLinkElem->GetElement(
      "visual")->Clone();
  sdf::ElementPtr templateCollisionElem = templateLinkElem->GetElement(
      "collision")->Clone();
  modelElem->ClearElements();
  std::stringstream visualNameStream;
  std::stringstream collisionNameStream;

  modelElem->GetAttribute("name")->Set(this->modelName);

  boost::unordered_map<std::string, PartData *>::iterator partsIt;

  // loop through all parts and generate sdf
  for (partsIt = this->allParts.begin(); partsIt != this->allParts.end();
      ++partsIt)
  {
    visualNameStream.str("");
    collisionNameStream.str("");

    std::string name = partsIt->first;
    PartData *part = partsIt->second;
    rendering::VisualPtr visual = part->visuals[0];
    sdf::ElementPtr newLinkElem = templateLinkElem->Clone();
    visualElem = newLinkElem->GetElement("visual");
    collisionElem = newLinkElem->GetElement("collision");
    newLinkElem->GetAttribute("name")->Set(visual->GetParent()->GetName());
    newLinkElem->GetElement("pose")->Set(part->pose);
    newLinkElem->GetElement("gravity")->Set(part->gravity);
    newLinkElem->GetElement("self_collide")->Set(part->selfCollide);
    newLinkElem->GetElement("kinematic")->Set(part->kinematic);
    sdf::ElementPtr inertialElem = newLinkElem->GetElement("inertial");
    inertialElem->GetElement("mass")->Set(part->inertial->GetMass());
    inertialElem->GetElement("pose")->Set(part->inertial->GetPose());
    sdf::ElementPtr inertiaElem = inertialElem->GetElement("inertia");
    inertiaElem->GetElement("ixx")->Set(part->inertial->GetIXX());
    inertiaElem->GetElement("iyy")->Set(part->inertial->GetIYY());
    inertiaElem->GetElement("izz")->Set(part->inertial->GetIZZ());
    inertiaElem->GetElement("ixy")->Set(part->inertial->GetIXY());
    inertiaElem->GetElement("ixz")->Set(part->inertial->GetIXZ());
    inertiaElem->GetElement("iyz")->Set(part->inertial->GetIYZ());

    modelElem->InsertElement(newLinkElem);

    visualElem->GetAttribute("name")->Set(visual->GetParent()->GetName()
        + "_visual");
    collisionElem->GetAttribute("name")->Set(visual->GetParent()->GetName()
        + "_collision");
    visualElem->GetElement("pose")->Set(visual->GetPose());
    collisionElem->GetElement("pose")->Set(visual->GetPose());

    sdf::ElementPtr geomElem =  visualElem->GetElement("geometry");
    geomElem->ClearElements();

    math::Vector3 scale = visual->GetScale();
    if (visual->GetParent()->GetName().find("unit_box") != std::string::npos)
    {
      sdf::ElementPtr boxElem = geomElem->AddElement("box");
      (boxElem->GetElement("size"))->Set(scale);
    }
    else if (visual->GetParent()->GetName().find("unit_cylinder")
       != std::string::npos)
    {
      sdf::ElementPtr cylinderElem = geomElem->AddElement("cylinder");
      (cylinderElem->GetElement("radius"))->Set(scale.x/2.0);
      (cylinderElem->GetElement("length"))->Set(scale.z);
    }
    else if (visual->GetParent()->GetName().find("unit_sphere")
        != std::string::npos)
    {
      sdf::ElementPtr sphereElem = geomElem->AddElement("sphere");
      (sphereElem->GetElement("radius"))->Set(scale.x/2.0);
    }
    else if (visual->GetParent()->GetName().find("custom")
        != std::string::npos)
    {
      sdf::ElementPtr customElem = geomElem->AddElement("mesh");
      (customElem->GetElement("scale"))->Set(scale);
      (customElem->GetElement("uri"))->Set(visual->GetMeshName());
      gzerr << "visual->GetMeshName() "<< visual->GetMeshName() << std::endl;
    }
    sdf::ElementPtr geomElemClone = geomElem->Clone();
    geomElem =  collisionElem->GetElement("geometry");
    geomElem->ClearElements();
    geomElem->InsertElement(geomElemClone->GetFirstElement());
  }

  // Add joints sdf
  this->jointMaker->GenerateSDF();
  sdf::ElementPtr jointsElem = this->jointMaker->GetSDF();
  sdf::ElementPtr jointElem;
  if (jointsElem->HasElement("joint"))
    jointElem = jointsElem->GetElement("joint");
  while (jointElem)
  {
    modelElem->InsertElement(jointElem);
    jointElem = jointElem->GetNextElement("joint");
  }

  // Model settings
  modelElem->GetElement("static")->Set(this->isStatic);
  modelElem->GetElement("allow_auto_disable")->Set(this->autoDisable);
//   qDebug() << this->modelSDF->ToString().c_str();
}


/////////////////////////////////////////////////
void PartData::OnApply()
{
  PartGeneralTab *general = this->inspector->GetGeneral();
  this->gravity = general->GetGravity();
  this->selfCollide = general->GetSelfCollide();
  this->kinematic = general->GetKinematic();

  this->inertial->SetMass(general->GetMass());
  this->inertial->SetCoG(general->GetInertialPose());
  this->inertial->SetInertiaMatrix(
      general->GetInertiaIXX(), general->GetInertiaIYY(),
      general->GetInertiaIZZ(), general->GetInertiaIXY(),
      general->GetInertiaIXZ(), general->GetInertiaIYZ());
  this->pose = general->GetPose();
  this->visuals[0]->GetParent()->SetWorldPose(this->pose);

}
