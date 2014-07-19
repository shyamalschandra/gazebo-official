/*
 * Copyright (C) 2012-2014 Open Source Robotics Foundation
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

#include <ignition/math/Vector3.hh>
#include <ignition/math/Quaternion.hh>
#include <ignition/math/Pose3.hh>

#include "gazebo/common/MeshManager.hh"

#include "gazebo/rendering/DynamicLines.hh"
#include "gazebo/rendering/ogre_gazebo.h"
#include "gazebo/rendering/Scene.hh"
#include "gazebo/rendering/COMVisualPrivate.hh"
#include "gazebo/rendering/COMVisual.hh"

using namespace gazebo;
using namespace rendering;

/////////////////////////////////////////////////
COMVisual::COMVisual(const std::string &_name, VisualPtr _vis)
  : Visual(*new COMVisualPrivate, _name, _vis, false)
{
}

/////////////////////////////////////////////////
COMVisual::~COMVisual()
{
}

/////////////////////////////////////////////////
void COMVisual::Load(sdf::ElementPtr _elem)
{
  Visual::Load();
  ignition::math::Pose3d pose = _elem->Get<ignition::math::Pose3d>("origin");
  this->Load(pose);
}

/////////////////////////////////////////////////
void COMVisual::Load(ConstLinkPtr &_msg)
{
  Visual::Load();

  ignition::math::Vector3d xyz(_msg->inertial().pose().position().x(),
                    _msg->inertial().pose().position().y(),
                    _msg->inertial().pose().position().z());
  ignition::math::Quaterniond q(_msg->inertial().pose().orientation().w(),
                     _msg->inertial().pose().orientation().x(),
                     _msg->inertial().pose().orientation().y(),
                     _msg->inertial().pose().orientation().z());

  // Use principal moments of inertia to scale COM visual
  // \todo: rotate COM to match principal axes when product terms are nonzero
  // This can be done with Eigen, or with code from the following paper:
  // A Method for Fast Diagonalization of a 2x2 or 3x3 Real Symmetric Matrix
  // http://arxiv.org/abs/1306.6291v3
  double mass = _msg->inertial().mass();
  double Ixx = _msg->inertial().ixx();
  double Iyy = _msg->inertial().iyy();
  double Izz = _msg->inertial().izz();
  ignition::math::Vector3d boxScale;
  if (mass < 0 || Ixx < 0 || Iyy < 0 || Izz < 0 ||
      Ixx + Iyy < Izz || Iyy + Izz < Ixx || Izz + Ixx < Iyy)
  {
    // Unrealistic inertia, load with default scale
    gzlog << "The link " << _msg->name() << " has unrealistic inertia, "
          << "unable to visualize box of equivalent inertia." << std::endl;
    this->Load(ignition::math::Pose3d(xyz, q));
  }
  else
  {
    // Compute dimensions of box with uniform density and equivalent inertia.
    boxScale.Set(sqrt(6*(Izz + Iyy - Ixx) / mass),
                 sqrt(6*(Izz + Ixx - Iyy) / mass),
                 sqrt(6*(Ixx + Iyy - Izz) / mass));
    this->Load(ignition::math::Pose3d(xyz, q), boxScale);
  }
}

/////////////////////////////////////////////////
void COMVisual::Load(const ignition::math::Pose3d &_pose,
                     const ignition::math::Vector3d &_scale)
{
  COMVisualPrivate *dPtr =
      reinterpret_cast<COMVisualPrivate *>(this->dataPtr);

  ignition::math::Vector3d p1(0, 0, -2*_scale.Z());
  ignition::math::Vector3d p2(0, 0,  2*_scale.Z());
  ignition::math::Vector3d p3(0, -2*_scale.Y(), 0);
  ignition::math::Vector3d p4(0,  2*_scale.Y(), 0);
  ignition::math::Vector3d p5(-2*_scale.X(), 0, 0);
  ignition::math::Vector3d p6(2*_scale.X(),  0, 0);
  p1 += _pose.Pos();
  p2 += _pose.Pos();
  p3 += _pose.Pos();
  p4 += _pose.Pos();
  p5 += _pose.Pos();
  p6 += _pose.Pos();
  p1 = _pose.Rot().RotateVector(p1);
  p2 = _pose.Rot().RotateVector(p2);
  p3 = _pose.Rot().RotateVector(p3);
  p4 = _pose.Rot().RotateVector(p4);
  p5 = _pose.Rot().RotateVector(p5);
  p6 = _pose.Rot().RotateVector(p6);

  dPtr->crossLines = this->CreateDynamicLine(rendering::RENDERING_LINE_LIST);
  dPtr->crossLines->setMaterial("Gazebo/Green");
  dPtr->crossLines->AddPoint(p1);
  dPtr->crossLines->AddPoint(p2);
  dPtr->crossLines->AddPoint(p3);
  dPtr->crossLines->AddPoint(p4);
  dPtr->crossLines->AddPoint(p5);
  dPtr->crossLines->AddPoint(p6);

  this->InsertMesh("unit_box");

  Ogre::MovableObject *boxObj =
    (Ogre::MovableObject*)(dPtr->scene->GetManager()->createEntity(
          this->GetName()+"__BOX__", "unit_box"));
  boxObj->setVisibilityFlags(GZ_VISIBILITY_GUI);
  ((Ogre::Entity*)boxObj)->setMaterialName("__GAZEBO_TRANS_PURPLE_MATERIAL__");

  dPtr->boxNode =
      dPtr->sceneNode->createChildSceneNode(this->GetName() + "_BOX");

  dPtr->boxNode->attachObject(boxObj);
  dPtr->boxNode->setScale(Conversions::Convert(_scale));
  dPtr->boxNode->setPosition(Conversions::Convert(_pose.Pos()));
  dPtr->boxNode->setOrientation(Conversions::Convert(_pose.Rot()));

  this->SetVisibilityFlags(GZ_VISIBILITY_GUI);
}
