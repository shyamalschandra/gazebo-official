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

#include "gazebo/transport/transport.hh"

#include "gazebo/rendering/RenderEvents.hh"
#include "gazebo/rendering/RenderingIface.hh"
#include "gazebo/rendering/Visual.hh"
#include "gazebo/rendering/RenderEngine.hh"
#include "gazebo/rendering/Scene.hh"
#include "gazebo/rendering/UserCamera.hh"
#include "gazebo/rendering/SelectionObj.hh"

#include "gazebo/gui/qt.h"
#include "gazebo/gui/MouseEventHandler.hh"
#include "gazebo/gui/GuiIface.hh"

#include "gazebo/gui/ModelManipulatorPrivate.hh"
#include "gazebo/gui/ModelManipulator.hh"

using namespace gazebo;
using namespace gui;

/////////////////////////////////////////////////
ModelManipulator::ModelManipulator()
  : dataPtr(new ModelManipulatorPrivate)
{
  this->dataPtr->initialized = false;
  this->dataPtr->selectionObj.reset();
  this->dataPtr->mouseMoveVis.reset();

  this->dataPtr->manipMode = "";
  this->dataPtr->globalManip = false;
}

/////////////////////////////////////////////////
ModelManipulator::~ModelManipulator()
{
  this->dataPtr->modelPub.reset();
  this->dataPtr->selectionObj.reset();
  delete this->dataPtr;
  this->dataPtr = NULL;
}

/////////////////////////////////////////////////
void ModelManipulator::Init()
{
  if (this->dataPtr->initialized)
    return;

  rendering::UserCameraPtr cam = gui::get_active_camera();
  if (!cam)
    return;

  if (!cam->GetScene())
    return;

  this->dataPtr->userCamera = cam;
  this->dataPtr->scene =  cam->GetScene();

  this->dataPtr->node = transport::NodePtr(new transport::Node());
  this->dataPtr->node->Init();
  this->dataPtr->modelPub =
      this->dataPtr->node->Advertise<msgs::Model>("~/model/modify");
  this->dataPtr->lightPub =
      this->dataPtr->node->Advertise<msgs::Light>("~/light");

  this->dataPtr->selectionObj.reset(new rendering::SelectionObj("__GL_MANIP__",
      this->dataPtr->scene->GetWorldVisual()));
  this->dataPtr->selectionObj->Load();

  this->dataPtr->initialized = true;
}

/////////////////////////////////////////////////
void ModelManipulator::RotateEntity(rendering::VisualPtr &_vis,
    const ignition::math::Vector3d &_axis, bool _local)
{
  ignition::math::Vector3d normal;

  if (_local)
  {
    if (_axis.X() > 0)
      normal = this->dataPtr->mouseMoveVisStartPose.Rot().XAxis();
    else if (_axis.Y() > 0)
      normal = this->dataPtr->mouseMoveVisStartPose.Rot().YAxis();
    else if (_axis.Z() > 0)
      normal = this->dataPtr->mouseMoveVisStartPose.Rot().ZAxis();
  }
  else
    normal = _axis;

  double offset = this->dataPtr->mouseMoveVisStartPose.Pos().Dot(normal);

  ignition::math::Vector3d pressPoint;
  this->dataPtr->userCamera->GetWorldPointOnPlane(
      this->dataPtr->mouseEvent.pressPos.X(),
      this->dataPtr->mouseEvent.pressPos.Y(),
      ignition::math::Planed(normal, offset), pressPoint);

  ignition::math::Vector3d newPoint;
  this->dataPtr->userCamera->GetWorldPointOnPlane(
      this->dataPtr->mouseEvent.pos.X(),
      this->dataPtr->mouseEvent.pos.Y(),
      ignition::math::Planed(normal, offset), newPoint);

  ignition::math::Vector3d v1 = pressPoint -
    this->dataPtr->mouseMoveVisStartPose.Pos();
  ignition::math::Vector3d v2 = newPoint -
    this->dataPtr->mouseMoveVisStartPose.Pos();
  v1 = v1.Normalize();
  v2 = v2.Normalize();
  double signTest = v1.Cross(v2).Dot(normal);
  double angle = atan2((v1.Cross(v2)).Length(), v1.Dot(v2));

  if (signTest < 0 )
    angle *= -1;

  if (this->dataPtr->mouseEvent.control)
    angle = rint(angle / (M_PI * 0.25)) * (M_PI * 0.25);

  ignition::math::Quaterniond rot(_axis, angle);

  if (_local)
    rot = this->dataPtr->mouseMoveVisStartPose.Rot() * rot;
  else
    rot = rot * this->dataPtr->mouseMoveVisStartPose.Rot();

  _vis->SetWorldRotation(rot);
}

/////////////////////////////////////////////////
ignition::math::Vector3d ModelManipulator::GetMousePositionOnPlane(
    rendering::CameraPtr _camera,
    const common::MouseEvent &_event)
{
  ignition::math::Vector3d origin1, dir1, p1;

  // Cast ray from the camera into the world
  _camera->GetCameraToViewportRay(_event.pos.X(), _event.pos.Y(),
      origin1, dir1);

  // Compute the distance from the camera to plane of translation
  ignition::math::Planed plane(ignition::math::Vector3d(0, 0, 1), 0);
  double dist1 = plane.Distance(origin1, dir1);

  p1 = origin1 + dir1 * dist1;

  return p1;
}

/////////////////////////////////////////////////
ignition::math::Vector3d ModelManipulator::SnapPoint(
    const ignition::math::Vector3d &_point,
    double _interval, double _sensitivity)
{
  if (_interval < 0)
  {
    gzerr << "Interval distance must be greater than or equal to 0"
        << std::endl;
    return ignition::math::Vector3d::Zero;
  }

  if (_sensitivity < 0 || _sensitivity > 1.0)
  {
    gzerr << "Sensitivity must be between 0 and 1" << std::endl;
    return ignition::math::Vector3d::Zero;
  }

  ignition::math::Vector3d point = _point;
  double snap = _interval * _sensitivity;

  double remainder = fmod(point.X(), _interval);
  int sign = remainder >= 0 ? 1 : -1;
  if (fabs(remainder) < snap)
      point.X() -= remainder;
  else if (fabs(remainder) > (_interval - snap))
      point.X() = point.X() - remainder + _interval * sign;

  remainder = fmod(point.Y(), _interval);
  sign = remainder >= 0 ? 1 : -1;
  if (fabs(remainder) < snap)
      point.Y() -= remainder;
  else if (fabs(remainder) > (_interval - snap))
      point.Y() = point.Y() - remainder + _interval * sign;

  remainder = fmod(point.Z(), _interval);
  sign = remainder >= 0 ? 1 : -1;
  if (fabs(remainder) < snap)
      point.Z() -= remainder;
  else if (fabs(remainder) > (_interval - snap))
      point.Z() = point.Z() - remainder + _interval * sign;

  return point;
}

/////////////////////////////////////////////////
ignition::math::Vector3d ModelManipulator::GetMouseMoveDistance(
    rendering::CameraPtr _camera,
    const ignition::math::Vector2i &_start,
    const ignition::math::Vector2i &_end,
    const ignition::math::Pose3d &_pose,
    const ignition::math::Vector3d &_axis, bool _local)
{
  ignition::math::Pose3d pose = _pose;

  ignition::math::Vector3d origin1, dir1, p1;
  ignition::math::Vector3d origin2, dir2, p2;

  // Cast two rays from the camera into the world
  _camera->GetCameraToViewportRay(_end.X(),
      _end.Y(), origin1, dir1);
  _camera->GetCameraToViewportRay(_start.X(),
      _start.Y(), origin2, dir2);

  ignition::math::Vector3d planeNorm(0, 0, 0);
  ignition::math::Vector3d projNorm(0, 0, 0);

  ignition::math::Vector3d planeNormOther(0, 0, 0);

  if (_axis.X() > 0 && _axis.Y() > 0)
  {
    planeNorm.Z() = 1;
    projNorm.Z() = 1;
  }
  else if (_axis.Z() > 0)
  {
    planeNorm.Y() = 1;
    projNorm.X() = 1;
    planeNormOther.X() = 1;
  }
  else if (_axis.X() > 0)
  {
    planeNorm.Z() = 1;
    projNorm.Y() = 1;
    planeNormOther.Y() = 1;
  }
  else if (_axis.Y() > 0)
  {
    planeNorm.Z() = 1;
    projNorm.X() = 1;
    planeNormOther.X() = 1;
  }

  if (_local)
  {
    planeNorm = pose.Rot().RotateVector(planeNorm);
    projNorm = pose.Rot().RotateVector(projNorm);
  }

  // Fine tune ray casting: cast a second ray and compare the two rays' angle
  // to plane. Use the one that is less parallel to plane for better results.
  double angle = dir1.Dot(planeNorm);
  if (_local)
    planeNormOther = pose.Rot().RotateVector(planeNormOther);
  double angleOther = dir1.Dot(planeNormOther);
  if (fabs(angleOther) > fabs(angle))
  {
    projNorm = planeNorm;
    planeNorm = planeNormOther;
  }

  // Compute the distance from the camera to plane
  double d = pose.Pos().Dot(planeNorm);
  ignition::math::Planed plane(planeNorm, d);
  double dist1 = plane.Distance(origin1, dir1);
  double dist2 = plane.Distance(origin2, dir2);

  // Compute two points on the plane. The first point is the current
  // mouse position, the second is the previous mouse position
  p1 = origin1 + dir1 * dist1;
  p2 = origin2 + dir2 * dist2;

  if (_local)
    p1 = p1 - (p1-p2).Dot(projNorm) * projNorm;

  ignition::math::Vector3d distance = p1 - p2;

  if (!_local)
    distance *= _axis;

  return distance;
}

/////////////////////////////////////////////////
ignition::math::Vector3d ModelManipulator::GetMouseMoveDistance(
    const ignition::math::Pose3d &_pose,
    const ignition::math::Vector3d &_axis, bool _local) const
{
  return GetMouseMoveDistance(this->dataPtr->userCamera,
      this->dataPtr->mouseStart,
      ignition::math::Vector2i(this->dataPtr->mouseEvent.pos.X(),
      this->dataPtr->mouseEvent.pos.Y()), _pose, _axis, _local);
}

/////////////////////////////////////////////////
void ModelManipulator::ScaleEntity(rendering::VisualPtr &_vis,
    const ignition::math::Vector3d &_axis, bool _local)
{
  ignition::math::Box bbox = this->dataPtr->mouseVisualBbox;
  ignition::math::Pose3d pose = _vis->GetWorldPose();
  ignition::math::Vector3d distance =
    this->GetMouseMoveDistance(pose, _axis, _local);

  ignition::math::Vector3d bboxSize = bbox.Size();
  ignition::math::Vector3d scale =
    (bboxSize + pose.Rot().RotateVectorReverse(distance)) / bboxSize;

  // a bit hacky to check for unit sphere and cylinder simple shapes in order
  // to restrict the scaling dimensions.
  if (this->dataPtr->keyEvent.key == Qt::Key_Shift ||
      _vis->GetName().find("unit_sphere") != std::string::npos)
  {
    if (_axis.X() > 0)
    {
      scale.Y() = scale.X();
      scale.Z() = scale.X();
    }
    else if (_axis.Y() > 0)
    {
      scale.X() = scale.Y();
      scale.Z() = scale.Y();
    }
    else if (_axis.Z() > 0)
    {
      scale.X() = scale.Z();
      scale.Y() = scale.Z();
    }
  }
  else if (_vis->GetName().find("unit_cylinder") != std::string::npos)
  {
    if (_axis.X() > 0)
    {
      scale.Y() = scale.X();
    }
    else if (_axis.Y() > 0)
    {
      scale.X() = scale.Y();
    }
  }
  else if (_vis->GetName().find("unit_box") != std::string::npos)
  {
  }
  else
  {
    // TODO scaling for complex models are not yet functional.
    // Limit scaling to simple shapes for now.
    gzwarn << " Scaling is currently limited to simple shapes." << std::endl;
    return;
  }

  ignition::math::Vector3d newScale =
    this->dataPtr->mouseVisualScale * scale.Abs();

  if (this->dataPtr->mouseEvent.control)
  {
    newScale = SnapPoint(newScale);
  }

  _vis->SetScale(newScale);
}

/////////////////////////////////////////////////
void ModelManipulator::TranslateEntity(rendering::VisualPtr &_vis,
    const ignition::math::Vector3d &_axis, bool _local)
{
  ignition::math::Pose3d pose = _vis->GetWorldPose();
  ignition::math::Vector3d distance =
    this->GetMouseMoveDistance(pose, _axis, _local);

  pose.Pos() = this->dataPtr->mouseMoveVisStartPose.Pos() + distance;

  if (this->dataPtr->mouseEvent.control)
  {
    pose.Pos() = SnapPoint(pose.Pos());
  }

  if (!(_axis.Z() > 0) && !_local)
    pose.Pos().Z() = _vis->GetWorldPose().Pos().Z();

  _vis->SetWorldPose(pose);
}

/////////////////////////////////////////////////
void ModelManipulator::PublishVisualPose(rendering::VisualPtr _vis)
{
  if (_vis)
  {
    // Check to see if the visual is a model.
    if (gui::get_entity_id(_vis->GetName()))
    {
      msgs::Model msg;
      msg.set_id(gui::get_entity_id(_vis->GetName()));
      msg.set_name(_vis->GetName());

      msgs::Set(msg.mutable_pose(), _vis->GetWorldPose());
      this->dataPtr->modelPub->Publish(msg);
    }
    // Otherwise, check to see if the visual is a light
    else if (this->dataPtr->scene->GetLight(_vis->GetName()))
    {
      msgs::Light msg;
      msg.set_name(_vis->GetName());
      msgs::Set(msg.mutable_pose(), _vis->GetWorldPose());
      this->dataPtr->lightPub->Publish(msg);
    }
  }
}

/////////////////////////////////////////////////
void ModelManipulator::PublishVisualScale(rendering::VisualPtr _vis)
{
  if (_vis)
  {
    // Check to see if the visual is a model.
    if (gui::get_entity_id(_vis->GetName()))
    {
      msgs::Model msg;
      msg.set_id(gui::get_entity_id(_vis->GetName()));
      msg.set_name(_vis->GetName());

      msgs::Set(msg.mutable_scale(), _vis->GetScale());
      this->dataPtr->modelPub->Publish(msg);
      _vis->SetScale(this->dataPtr->mouseVisualScale);
    }
  }
}

/////////////////////////////////////////////////
void ModelManipulator::OnMousePressEvent(const common::MouseEvent &_event)
{
  this->dataPtr->mouseEvent = _event;
  this->dataPtr->mouseStart = _event.pressPos;
  this->SetMouseMoveVisual(rendering::VisualPtr());

  rendering::VisualPtr vis;
  rendering::VisualPtr mouseVis =
    this->dataPtr->userCamera->GetVisual(this->dataPtr->mouseEvent.pos);
  // set the new mouse vis only if there are no modifier keys pressed and the
  // entity was different from the previously selected one.
  if (!this->dataPtr->keyEvent.key && (this->dataPtr->selectionObj->GetMode() ==
       rendering::SelectionObj::SELECTION_NONE
      || (mouseVis && mouseVis != this->dataPtr->selectionObj->GetParent())))
  {
    vis = mouseVis;
  }
  else
  {
    vis = this->dataPtr->selectionObj->GetParent();
  }

  if (vis && !vis->IsPlane() &&
      this->dataPtr->mouseEvent.button == common::MouseEvent::LEFT)
  {
    if (gui::get_entity_id(vis->GetRootVisual()->GetName()))
    {
      vis = vis->GetRootVisual();
    }

    this->dataPtr->mouseMoveVisStartPose = vis->GetWorldPose();

    this->SetMouseMoveVisual(vis);

    event::Events::setSelectedEntity(
        this->dataPtr->mouseMoveVis->GetName(), "move");
    QApplication::setOverrideCursor(Qt::ClosedHandCursor);

    if (this->dataPtr->mouseMoveVis && !this->dataPtr->mouseMoveVis->IsPlane())
    {
      this->dataPtr->selectionObj->Attach(this->dataPtr->mouseMoveVis);
      this->dataPtr->selectionObj->SetMode(this->dataPtr->manipMode);
    }
    else
    {
      this->dataPtr->selectionObj->SetMode(
          rendering::SelectionObj::SELECTION_NONE);
      this->dataPtr->selectionObj->Detach();
    }
  }
  else
    this->dataPtr->userCamera->HandleMouseEvent(this->dataPtr->mouseEvent);
}

/////////////////////////////////////////////////
void ModelManipulator::OnMouseMoveEvent(const common::MouseEvent &_event)
{
  this->dataPtr->mouseEvent = _event;
  if (this->dataPtr->mouseEvent.dragging)
  {
    if (this->dataPtr->mouseMoveVis &&
        this->dataPtr->mouseEvent.button == common::MouseEvent::LEFT)
    {
      ignition::math::Vector3d axis = ignition::math::Vector3d::Zero;
      if (this->dataPtr->keyEvent.key == Qt::Key_X)
        axis.X() = 1;
      else if (this->dataPtr->keyEvent.key == Qt::Key_Y)
        axis.Y() = 1;
      else if (this->dataPtr->keyEvent.key == Qt::Key_Z)
        axis.Z() = 1;

      if (this->dataPtr->selectionObj->GetMode() ==
          rendering::SelectionObj::TRANS)
      {
        if (axis != ignition::math::Vector3d::Zero)
        {
          this->TranslateEntity(this->dataPtr->mouseMoveVis, axis, false);
        }
        else if (this->dataPtr->selectionObj->GetState()
            == rendering::SelectionObj::TRANS_X)
        {
          this->TranslateEntity(this->dataPtr->mouseMoveVis,
              ignition::math::Vector3d::UnitX, !this->dataPtr->globalManip);
        }
        else if (this->dataPtr->selectionObj->GetState()
            == rendering::SelectionObj::TRANS_Y)
        {
          this->TranslateEntity(this->dataPtr->mouseMoveVis,
              ignition::math::Vector3d::UnitY, !this->dataPtr->globalManip);
        }
        else if (this->dataPtr->selectionObj->GetState()
            == rendering::SelectionObj::TRANS_Z)
        {
          this->TranslateEntity(this->dataPtr->mouseMoveVis,
            ignition::math::Vector3d::UnitZ, !this->dataPtr->globalManip);
        }
        else
        {
          this->TranslateEntity(
              this->dataPtr->mouseMoveVis, ignition::math::Vector3d(1, 1, 0));
        }
      }
      else if (this->dataPtr->selectionObj->GetMode()
          == rendering::SelectionObj::ROT)
      {
        if (axis != ignition::math::Vector3d::Zero)
        {
          this->RotateEntity(this->dataPtr->mouseMoveVis, axis, false);
        }
        else if (this->dataPtr->selectionObj->GetState()
            == rendering::SelectionObj::ROT_X
            || this->dataPtr->keyEvent.key == Qt::Key_X)
        {
          this->RotateEntity(this->dataPtr->mouseMoveVis,
              ignition::math::Vector3d::UnitX, !this->dataPtr->globalManip);
        }
        else if (this->dataPtr->selectionObj->GetState()
            == rendering::SelectionObj::ROT_Y
            || this->dataPtr->keyEvent.key == Qt::Key_Y)
        {
          this->RotateEntity(this->dataPtr->mouseMoveVis,
              ignition::math::Vector3d::UnitY, !this->dataPtr->globalManip);
        }
        else if (this->dataPtr->selectionObj->GetState()
            == rendering::SelectionObj::ROT_Z
            || this->dataPtr->keyEvent.key == Qt::Key_Z)
        {
          this->RotateEntity(this->dataPtr->mouseMoveVis,
              ignition::math::Vector3d::UnitZ, !this->dataPtr->globalManip);
        }
      }
      else if (this->dataPtr->selectionObj->GetMode()
          == rendering::SelectionObj::SCALE)
      {
        if (axis != ignition::math::Vector3d::Zero)
        {
          this->ScaleEntity(this->dataPtr->mouseMoveVis, axis, false);
        }
        else if (this->dataPtr->selectionObj->GetState()
            == rendering::SelectionObj::SCALE_X
            || this->dataPtr->keyEvent.key == Qt::Key_X)
        {
          this->ScaleEntity(this->dataPtr->mouseMoveVis,
              ignition::math::Vector3d::UnitX, true);
        }
        else if (this->dataPtr->selectionObj->GetState()
            == rendering::SelectionObj::SCALE_Y
            || this->dataPtr->keyEvent.key == Qt::Key_Y)
        {
          this->ScaleEntity(this->dataPtr->mouseMoveVis,
              ignition::math::Vector3d::UnitY, true);
        }
        else if (this->dataPtr->selectionObj->GetState()
            == rendering::SelectionObj::SCALE_Z
            || this->dataPtr->keyEvent.key == Qt::Key_Z)
        {
          this->ScaleEntity(this->dataPtr->mouseMoveVis,
              ignition::math::Vector3d::UnitZ, true);
        }
      }
    }
    else
      this->dataPtr->userCamera->HandleMouseEvent(this->dataPtr->mouseEvent);
  }
  else
  {
    std::string manipState;
    this->dataPtr->userCamera->GetVisual(this->dataPtr->mouseEvent.pos,
        manipState);
    this->dataPtr->selectionObj->SetState(manipState);

    if (!manipState.empty())
      QApplication::setOverrideCursor(Qt::OpenHandCursor);
    else
    {
      rendering::VisualPtr vis = this->dataPtr->userCamera->GetVisual(
          this->dataPtr->mouseEvent.pos);

      if (vis && !vis->IsPlane())
        QApplication::setOverrideCursor(Qt::OpenHandCursor);
      else
        QApplication::setOverrideCursor(Qt::ArrowCursor);
      this->dataPtr->userCamera->HandleMouseEvent(this->dataPtr->mouseEvent);
    }
  }
}

//////////////////////////////////////////////////
void ModelManipulator::OnMouseReleaseEvent(const common::MouseEvent &_event)
{
  this->dataPtr->mouseEvent = _event;
  if (this->dataPtr->mouseEvent.dragging)
  {
    // If we were dragging a visual around, then publish its new pose to the
    // server
    if (this->dataPtr->mouseMoveVis)
    {
      if (this->dataPtr->manipMode == "scale")
      {
        this->dataPtr->selectionObj->UpdateSize();
        this->PublishVisualScale(this->dataPtr->mouseMoveVis);
      }
      else
        this->PublishVisualPose(this->dataPtr->mouseMoveVis);
      this->SetMouseMoveVisual(rendering::VisualPtr());
      QApplication::setOverrideCursor(Qt::OpenHandCursor);
    }
    event::Events::setSelectedEntity("", "normal");
  }
  else
  {
    if (this->dataPtr->mouseEvent.button == common::MouseEvent::LEFT)
    {
      rendering::VisualPtr vis =
        this->dataPtr->userCamera->GetVisual(this->dataPtr->mouseEvent.pos);
      if (vis && vis->IsPlane())
      {
        this->dataPtr->selectionObj->SetMode(
            rendering::SelectionObj::SELECTION_NONE);
        this->dataPtr->selectionObj->Detach();
      }
    }
  }
  this->dataPtr->userCamera->HandleMouseEvent(this->dataPtr->mouseEvent);
}

//////////////////////////////////////////////////
void ModelManipulator::SetManipulationMode(const std::string &_mode)
{
  this->dataPtr->manipMode = _mode;
  if (this->dataPtr->selectionObj->GetMode() !=
      rendering::SelectionObj::SELECTION_NONE ||  this->dataPtr->mouseMoveVis)
  {
    this->dataPtr->selectionObj->SetMode(this->dataPtr->manipMode);
    if (this->dataPtr->manipMode != "translate"
        && this->dataPtr->manipMode != "rotate"
        && this->dataPtr->manipMode != "scale")
      this->SetMouseMoveVisual(rendering::VisualPtr());
  }
}

/////////////////////////////////////////////////
void ModelManipulator::SetAttachedVisual(rendering::VisualPtr _vis)
{
  rendering::VisualPtr vis = _vis;

  if (gui::get_entity_id(vis->GetRootVisual()->GetName()))
    vis = vis->GetRootVisual();

  this->dataPtr->mouseMoveVisStartPose = vis->GetWorldPose();

  this->SetMouseMoveVisual(vis);

  if (this->dataPtr->mouseMoveVis && !this->dataPtr->mouseMoveVis->IsPlane())
    this->dataPtr->selectionObj->Attach(this->dataPtr->mouseMoveVis);
}

/////////////////////////////////////////////////
void ModelManipulator::SetMouseMoveVisual(rendering::VisualPtr _vis)
{
  this->dataPtr->mouseMoveVis = _vis;
  if (_vis)
  {
    this->dataPtr->mouseVisualScale = _vis->GetScale();
    this->dataPtr->mouseVisualBbox = _vis->GetBoundingBox();
  }
  else
    this->dataPtr->mouseVisualScale = ignition::math::Vector3d::One;
}

//////////////////////////////////////////////////
void ModelManipulator::OnKeyPressEvent(const common::KeyEvent &_event)
{
  this->dataPtr->keyEvent = _event;
  // reset mouseMoveVisStartPose if in manipulation mode.
  if (this->dataPtr->manipMode == "translate"
      || this->dataPtr->manipMode == "rotate"
      || this->dataPtr->manipMode == "scale")
  {
    if (_event.key == Qt::Key_X || _event.key == Qt::Key_Y
        || _event.key == Qt::Key_Z)
    {
      this->dataPtr->mouseStart = this->dataPtr->mouseEvent.pos;
      if (this->dataPtr->mouseMoveVis)
      {
        this->dataPtr->mouseMoveVisStartPose =
            this->dataPtr->mouseMoveVis->GetWorldPose();
      }
    }
    else  if (this->dataPtr->keyEvent.key == Qt::Key_Shift)
    {
      this->dataPtr->globalManip = true;
      this->dataPtr->selectionObj->SetGlobal(this->dataPtr->globalManip);
    }
  }
}

//////////////////////////////////////////////////
void ModelManipulator::OnKeyReleaseEvent(const common::KeyEvent &_event)
{
  this->dataPtr->keyEvent = _event;
  // reset mouseMoveVisStartPose if in manipulation mode.
  if (this->dataPtr->manipMode == "translate"
      || this->dataPtr->manipMode == "rotate"
      || this->dataPtr->manipMode == "scale")
  {
    if (_event.key == Qt::Key_X || _event.key == Qt::Key_Y
        || _event.key == Qt::Key_Z)
    {
      this->dataPtr->mouseStart = this->dataPtr->mouseEvent.pos;
      if (this->dataPtr->mouseMoveVis)
      {
        this->dataPtr->mouseMoveVisStartPose =
            this->dataPtr->mouseMoveVis->GetWorldPose();
      }
    }
    else  if (this->dataPtr->keyEvent.key == Qt::Key_Shift)
    {
      this->dataPtr->globalManip = false;
      this->dataPtr->selectionObj->SetGlobal(this->dataPtr->globalManip);
    }
  }
  this->dataPtr->keyEvent.key = 0;
}

// Function migrated here from GLWidget.cc and commented out since it doesn't
// seem like it's currently used. Kept here for future references
/////////////////////////////////////////////////
/*void GLWidget::SmartMoveVisual(rendering::VisualPtr _vis)
{
  if (!this->dataPtr->mouseEvent.dragging)
    return;

  // Get the point on the plane which correspoinds to the mouse
  ignition::math::Vector3d pp;

  // Rotate the visual using the middle mouse button
  if (this->dataPtr->mouseEvent.buttons == common::MouseEvent::MIDDLE)
  {
    ignition::math::Vector3d rpy = this->dataPtr->mouseMoveVisStartPose.Rot().Euler();
    ignition::math::Vector2i delta = this->dataPtr->mouseEvent.Pos() -
        this->dataPtr->mouseEvent.pressPos;
    double yaw = (delta.X() * 0.01) + rpy.Z();
    if (!this->dataPtr->mouseEvent.shift)
    {
      double snap = rint(yaw / (M_PI * .25)) * (M_PI * 0.25);

      if (fabs(yaw - snap) < IGN_DTOR(10))
        yaw = snap;
    }

    _vis->SetWorldRotation(ignition::math::Quaterniond(rpy.x, rpy.y, yaw));
  }
  else if (this->dataPtr->mouseEvent.buttons == common::MouseEvent::RIGHT)
  {
    ignition::math::Vector3d rpy = this->dataPtr->mouseMoveVisStartPose.Rot().Euler();
    ignition::math::Vector2i delta = this->dataPtr->mouseEvent.Pos() -
        this->dataPtr->mouseEvent.pressPos;
    double pitch = (delta.Y() * 0.01) + rpy.Y();
    if (!this->dataPtr->mouseEvent.shift)
    {
      double snap = rint(pitch / (M_PI * .25)) * (M_PI * 0.25);

      if (fabs(pitch - snap) < IGN_DTOR(10))
        pitch = snap;
    }

    _vis->SetWorldRotation(ignition::math::Quaterniond(rpy.x, pitch, rpy.Z()));
  }
  else if (this->dataPtr->mouseEvent.buttons & common::MouseEvent::LEFT &&
           this->dataPtr->mouseEvent.buttons & common::MouseEvent::RIGHT)
  {
    ignition::math::Vector3d rpy = this->dataPtr->mouseMoveVisStartPose.Rot().Euler();
    ignition::math::Vector2i delta = this->dataPtr->mouseEvent.Pos() -
        this->dataPtr->mouseEvent.pressPos;
    double roll = (delta.X() * 0.01) + rpy.X();
    if (!this->dataPtr->mouseEvent.shift)
    {
      double snap = rint(roll / (M_PI * .25)) * (M_PI * 0.25);

      if (fabs(roll - snap) < IGN_DTOR(10))
        roll = snap;
    }

    _vis->SetWorldRotation(ignition::math::Quaterniond(roll, rpy.y, rpy.Z()));
  }
  else
  {
    this->TranslateEntity(_vis);
  }
}*/
