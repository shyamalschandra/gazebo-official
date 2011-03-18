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
#include <wx/aui/aui.h>

#include "common/Image.hh"
#include "common/Global.hh"
#include "common/GazeboConfig.hh"
#include "common/MeshManager.hh"

#include "rendering/Light.hh"
#include "rendering/Scene.hh"
#include "rendering/UserCamera.hh"
#include "rendering/OrbitViewController.hh"
#include "rendering/FPSViewController.hh"
#include "rendering/Visual.hh"

#include "gui/RenderControl.hh"
#include "gui/ModelBuilder.hh"

using namespace gazebo;
using namespace gui;


ModelBuilder::ModelBuilder( wxWindow *parent )
  : wxDialog(parent,wxID_ANY, wxT("Model Builder"), wxDefaultPosition, wxSize(600, 600), wxDEFAULT_FRAME_STYLE)
{
/*  this->scene = OgreAdaptor::Instance()->CreateScene("viewer_scene");
  this->dirLight = new Light(NULL, scene);
  this->dirLight->Load(NULL);
  this->dirLight->SetLightType("directional");
  this->dirLight->SetDiffuseColor( Color(1.0, 1.0, 1.0) );
  this->dirLight->SetSpecularColor( Color(0.1, 0.1, 0.1) );
  this->dirLight->SetDirection( common::Vector3(0, 0, -1) );
  */

  //this->world = Simulator::Instance()->CreateWorld("gazebo_model_builder_world");

  /* NATY: FIx this
  this->renderControl = new RenderControl(this);
  this->renderControl->ViewScene(this->world->GetScene());
  this->renderControl->Init();
  UserCamera *cam = this->renderControl->GetCamera();
  cam->SetClipDist(0.01, 1000);
  cam->SetWorldPosition(common::Vector3(-1,0,2));
  cam->RotatePitch(DTOR(-30));
  cam->SetViewController( OrbitViewController::GetTypeString() );

  this->MakeToolbar();

  this->auiManager = new wxAuiManager(this);
  this->auiManager->AddPane(this->renderControl, wxAuiPaneInfo().CenterPane().Name(wxT("Render")));
  this->auiManager->AddPane(this->toolbar, wxAuiPaneInfo().ToolbarPane().RightDockable(false).LeftDockable(false).MinSize(100,30).Top().Name(wxT("Tools")).Caption(wxT("Tools")));
  this->auiManager->Update();

  this->model = new Model(NULL);
  */
}

ModelBuilder::~ModelBuilder()
{
  /*
  delete this->renderControl;
  delete this->dirLight;
  Simulator::Instance()->RemoveWorld(this->world->GetName());

  this->auiManager->UnInit();
  delete this->auiManager;
  */
}

void ModelBuilder::MakeToolbar()
{
  /*
#if !defined(__WXMAC__)
  Image image;

  image.Load("control_play_blue.png");
  wxBitmap addbody_bitmap(wxString::FromAscii(image.GetFilename().c_str()), wxBITMAP_TYPE_PNG);

  this->toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTB_HORIZONTAL);
  this->toolbar->Connect( wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( ModelBuilder::OnToolClicked), NULL , this );

  this->toolbar->AddCheckTool(ADD_BODY, wxT("Add Body"), addbody_bitmap, wxNullBitmap, wxT("Add a body"));

  this->toolbar->Realize();

#endif
*/
}

////////////////////////////////////////////////////////////////////////////////
void ModelBuilder::OnToolClicked( wxCommandEvent &event )
{
  /*
  int id = event.GetId();
  if (id == ADD_BODY)
  {
    this->model->CreateBody();
  }
  */
}
