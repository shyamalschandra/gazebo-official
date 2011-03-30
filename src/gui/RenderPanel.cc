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
#include <boost/lexical_cast.hpp>

#include "common/Pose3d.hh"
#include "common/Angle.hh"


#include "rendering/UserCamera.hh"

#include "gui/RenderControl.hh"
#include "gui/RenderPanel.hh"

using namespace gazebo;
using namespace gui;

////////////////////////////////////////////////////////////////////////////////
// Constructor
RenderPanel::RenderPanel(wxWindow *parent)
  : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(320,240), wxTAB_TRAVERSAL)
{
  wxBoxSizer *boxSizer1 = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *boxSizer2 = new wxBoxSizer(wxHORIZONTAL);

  wxSize poseCtrlSize (65,25);

  this->renderControl = new RenderControl(this);
  boxSizer1->Add(this->renderControl, 2, wxALL | wxEXPAND );

  wxStaticText *xyzPosText = new wxStaticText( this, wxID_ANY, wxT("XYZ:"), wxDefaultPosition, wxDefaultSize, 0);
  xyzPosText->Wrap(-1);
  boxSizer2->Add(xyzPosText, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 5);

  this->xPosCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, poseCtrlSize, 0, wxTextValidator(wxFILTER_NUMERIC));
  this->xPosCtrl->Connect(wxEVT_SET_FOCUS, wxFocusEventHandler(RenderPanel::OnXPosSetFocus), NULL, this);
  this->xPosCtrl->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler(RenderPanel::OnXPosKillFocus), NULL, this);
  boxSizer2->Add(this->xPosCtrl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 1);

  this->yPosCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, poseCtrlSize,0, wxTextValidator(wxFILTER_NUMERIC));
  this->yPosCtrl->Connect(wxEVT_SET_FOCUS, wxFocusEventHandler(RenderPanel::OnYPosSetFocus), NULL, this);
  this->yPosCtrl->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler(RenderPanel::OnYPosKillFocus), NULL, this);
  boxSizer2->Add(this->yPosCtrl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 1);

  this->zPosCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, poseCtrlSize,0, wxTextValidator(wxFILTER_NUMERIC));
  this->zPosCtrl->Connect(wxEVT_SET_FOCUS, wxFocusEventHandler(RenderPanel::OnZPosSetFocus), NULL, this);
  this->zPosCtrl->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler(RenderPanel::OnZPosKillFocus), NULL, this);
  boxSizer2->Add(this->zPosCtrl, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);



  wxStaticText *rpyText = new wxStaticText( this, wxID_ANY, wxT("RPY:"), wxDefaultPosition, wxDefaultSize, 0);
  rpyText->Wrap(-1);
  boxSizer2->Add(rpyText, 0, wxALL | wxALIGN_CENTER_VERTICAL, 1);

  this->rollCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, poseCtrlSize, 0, wxTextValidator(wxFILTER_NUMERIC));
  this->rollCtrl->Connect(wxEVT_SET_FOCUS, wxFocusEventHandler(RenderPanel::OnRollSetFocus), NULL, this);
  this->rollCtrl->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler(RenderPanel::OnRollKillFocus), NULL, this);
  boxSizer2->Add(this->rollCtrl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 1);

  this->pitchCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, poseCtrlSize,0, wxTextValidator(wxFILTER_NUMERIC) );
  this->pitchCtrl->Connect(wxEVT_SET_FOCUS, wxFocusEventHandler(RenderPanel::OnPitchSetFocus), NULL, this);
  this->pitchCtrl->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler(RenderPanel::OnPitchKillFocus), NULL, this);
  boxSizer2->Add(this->pitchCtrl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 1);

  this->yawCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, poseCtrlSize,0, wxTextValidator(wxFILTER_NUMERIC));
  this->yawCtrl->Connect(wxEVT_SET_FOCUS, wxFocusEventHandler(RenderPanel::OnYawSetFocus), NULL, this);
  this->yawCtrl->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler(RenderPanel::OnYawKillFocus), NULL, this);
  boxSizer2->Add(this->yawCtrl, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);



  wxStaticText *fpsText = new wxStaticText( this, wxID_ANY, wxT("FPS:"), wxDefaultPosition, wxDefaultSize, 0);
  fpsText->Wrap(-1);
  boxSizer2->Add(fpsText, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  this->fpsCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
  boxSizer2->Add(this->fpsCtrl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 1);



  wxStaticText *triangleCountText = new wxStaticText( this, wxID_ANY, wxT("Triangles:"), wxDefaultPosition, wxDefaultSize, 0);
  triangleCountText->Wrap(-1);
  boxSizer2->Add(triangleCountText, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  this->triangleCountCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
  boxSizer2->Add(this->triangleCountCtrl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 1);


  boxSizer1->Add(boxSizer2,0,wxEXPAND,5);

  this->SetSizer(boxSizer1);
  this->Layout();

  this->xUpdate = true;
  this->yUpdate = true;
  this->zUpdate = true;
  this->rollUpdate = true;
  this->pitchUpdate = true;
  this->yawUpdate = true;
}

////////////////////////////////////////////////////////////////////////////////
// Destructor
RenderPanel::~RenderPanel()
{
  delete this->renderControl;
}

////////////////////////////////////////////////////////////////////////////////
// Init
void RenderPanel::Init()
{
  this->renderControl->Init();
}

////////////////////////////////////////////////////////////////////////////////
// Create a camera
void RenderPanel::ViewScene(rendering::Scene *scene)
{
  this->renderControl->ViewScene(scene);
}

////////////////////////////////////////////////////////////////////////////////
// Update the render panel
void RenderPanel::MyUpdate()
{
  rendering::UserCamera *cam = this->renderControl->GetCamera();
  if (!cam)
    return;

  float fps = cam->GetAvgFPS();
  int triangleCount = cam->GetTriangleCount();
  common::Pose3d pose = cam->GetWorldPose();

  wxString str;

  if (this->xUpdate)
  {
    str.Printf(wxT("%6.2f"), pose.pos.x);
    this->xPosCtrl->ChangeValue(str);
  }

  if (this->yUpdate)
  {
    str.Printf(wxT("%6.2f"), pose.pos.y);
    this->yPosCtrl->SetValue(str);
  }

  if (this->zUpdate)
  {
    str.Printf(wxT("%6.2f"), pose.pos.z);
    this->zPosCtrl->SetValue(str);
  }

  if (this->rollUpdate)
  {
    str.Printf(wxT("%6.2f"), RTOD(pose.rot.GetAsEuler().x));
    this->rollCtrl->SetValue(str);
  }

  if (this->pitchUpdate)
  {
    str.Printf(wxT("%6.2f"), RTOD(pose.rot.GetAsEuler().y));
    this->pitchCtrl->SetValue(str);
  }

  if (this->yawUpdate)
  {
    str.Printf(wxT("%6.2f"), RTOD(pose.rot.GetAsEuler().z));
    this->yawCtrl->SetValue(str);
  }

  str.Printf(wxT("%6.2f"), fps);
  this->fpsCtrl->SetValue(str);

  str.Printf(wxT("%d"), triangleCount);
  this->triangleCountCtrl->SetValue( str );
}

////////////////////////////////////////////////////////////////////////////////
/// Get the camera
rendering::UserCamera *RenderPanel::GetCamera()
{
  return this->renderControl->GetCamera();
}


void RenderPanel::OnXPosSetFocus(wxFocusEvent &event)
{
  this->xUpdate = false;
}

void RenderPanel::OnYPosSetFocus(wxFocusEvent &event)
{
  this->yUpdate = false;
}

void RenderPanel::OnZPosSetFocus(wxFocusEvent &event)
{
  this->zUpdate = false;
}

void RenderPanel::OnRollSetFocus(wxFocusEvent &event)
{
  this->rollUpdate = false;
}

void RenderPanel::OnPitchSetFocus(wxFocusEvent &event)
{
  this->pitchUpdate = false;
}

void RenderPanel::OnYawSetFocus(wxFocusEvent &event)
{
  this->yawUpdate = false;
}




void RenderPanel::OnXPosKillFocus(wxFocusEvent &event)
{
  rendering::UserCamera *cam = this->renderControl->GetCamera();
  common::Vector3 pos = cam->GetWorldPosition();

  std::string str = std::string(this->xPosCtrl->GetValue().mb_str());
  boost::trim(str);

  pos.x = boost::lexical_cast<float>(str);

  cam->SetWorldPosition(pos);

  this->xUpdate = true;
}

void RenderPanel::OnYPosKillFocus(wxFocusEvent &event)
{
  rendering::UserCamera *cam = this->renderControl->GetCamera();
  common::Vector3 pos = cam->GetWorldPosition();

  std::string str = std::string(this->yPosCtrl->GetValue().mb_str());
  boost::trim(str);

  pos.y = boost::lexical_cast<float>(str);

  cam->SetWorldPosition(pos);

  this->yUpdate = true;
}

void RenderPanel::OnZPosKillFocus(wxFocusEvent &event)
{
  rendering::UserCamera *cam = this->renderControl->GetCamera();
  common::Vector3 pos = cam->GetWorldPosition();

  std::string str = std::string(this->zPosCtrl->GetValue().mb_str());
  boost::trim(str);

  pos.z = boost::lexical_cast<float>(str);

  cam->SetWorldPosition(pos);

  this->zUpdate = true;
}

void RenderPanel::OnRollKillFocus(wxFocusEvent &event)
{
  rendering::UserCamera *cam = this->renderControl->GetCamera();
  common::Pose3d pose = cam->GetWorldPose();
  common::Vector3 rpy = pose.rot.GetAsEuler();

  std::string str = std::string(this->rollCtrl->GetValue().mb_str());
  boost::trim(str);

  rpy.x = DTOR(boost::lexical_cast<float>(str));

  pose.rot.SetFromEuler(rpy);
  cam->SetWorldPose(pose);

  this->rollUpdate = true;
}

void RenderPanel::OnPitchKillFocus(wxFocusEvent &event)
{
  rendering::UserCamera *cam = this->renderControl->GetCamera();
  common::Pose3d pose = cam->GetWorldPose();
  common::Vector3 rpy = pose.rot.GetAsEuler();

  std::string str = std::string(this->pitchCtrl->GetValue().mb_str());
  boost::trim(str);

  rpy.y = DTOR(boost::lexical_cast<float>(str));

  pose.rot.SetFromEuler(rpy);
  cam->SetWorldPose(pose);

  this->pitchUpdate = true;
}

void RenderPanel::OnYawKillFocus(wxFocusEvent &event)
{
  rendering::UserCamera *cam = this->renderControl->GetCamera();
  common::Pose3d pose = cam->GetWorldPose();
  common::Vector3 rpy = pose.rot.GetAsEuler();

  std::string str = std::string(this->yawCtrl->GetValue().mb_str());
  boost::trim(str);

  rpy.z = DTOR(boost::lexical_cast<float>(str));

  pose.rot.SetFromEuler(rpy);
  cam->SetWorldPose(pose);

  this->yawUpdate = true;
}
