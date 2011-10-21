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
#include <boost/bind.hpp>

#include "gui/Gui.hh"
#include "rendering/Scene.hh"
#include "rendering/Camera.hh"
#include "rendering/DepthCamera.hh"
#include "rendering/RenderEngine.hh"
#include "rendering/UserCamera.hh"
#include "rendering/GUIOverlay.hh"
#include "gazebo.h"

namespace gazebo
{
  class GUITest : public GUIPlugin
  {
    public: void Load()
    {
      this->node = transport::NodePtr(new transport::Node());
      this->node->Init();

      this->connections.push_back( 
          event::Events::ConnectPreRender( 
            boost::bind(&GUITest::PreRender, this) ) );
    }

    private: bool OnSelect(const CEGUI::EventArgs& _e)
             {
               printf("OnSelect\n");
               return true;
             }


    private: void PreRender()
             {
               static bool connected = false;

               if (!connected)
               {
                 rendering::UserCameraPtr userCam = gui::get_active_camera();
                 rendering::ScenePtr scene = rendering::RenderEngine::Instance()->GetScene("default");
                 if (!scene)
                   gzerr << "Unable to find scene[default]\n";

                 //this->camera = scene->CreateDepthCamera("my_camera");
                 this->camera = scene->CreateCamera("my_camera");

                 this->camera->Load();
                 this->camera->Init();
                 this->camera->SetClipDist(0.1, 20);
                 this->camera->SetCaptureData(true);
                 this->camera->SetImageWidth(500);
                 this->camera->SetImageHeight(500);
                 this->camera->SetFOV( 1.5707 );

                 this->camera->CreateRenderTexture("help_me");
                 //this->camera->CreateDepthTexture("help_me");

                 this->camera->SetWorldPosition( math::Vector3(0,0,0.5) );

                 if (!camera)
                   gzerr << "Unable to find camera[camera]\n";

                 userCam->GetGUIOverlay()->AttachCameraToImage( this->camera, 
                     "Root/CameraView");

                 userCam->GetGUIOverlay()->ButtonCallback( 
                     "Root/PrepositionButton", 
                     &GUITest::OnPrepositionButton, this  );

                 userCam->GetGUIOverlay()->ButtonCallback( 
                     "Root/VerbButton", &GUITest::OnVerbButton, this  );

                 userCam->GetGUIOverlay()->GetWindow("Root/PrepositionButton")->hide();
                 userCam->GetGUIOverlay()->GetWindow("Root/NounButton")->hide();
                 userCam->GetGUIOverlay()->GetWindow("Root/VerbList")->hide();
                 userCam->GetGUIOverlay()->GetWindow("Root/PrepositionList")->hide();
                 userCam->GetGUIOverlay()->GetWindow("Root/NounList")->hide();

                 ((CEGUI::Listbox*)userCam->GetGUIOverlay()->GetWindow("Root/VerbList"))->subscribeEvent( 
                     CEGUI::Listbox::EventSelectionChanged, 
                     CEGUI::Event::Subscriber( &GUITest::OnSelect, this) );

                 connected = true;
               }
             }

    private: void OnPrepositionButton()
             {
               printf("GUITest::Prep Button\n");
             }

    private: void OnVerbButton()
             {
               rendering::UserCameraPtr userCam = gui::get_active_camera();
               CEGUI::Listbox *win = (CEGUI::Listbox*)(userCam->GetGUIOverlay()->GetWindow("Root/VerbList"));
               win->show();

               win->resetList();
               CEGUI::ListboxTextItem *item = new CEGUI::ListboxTextItem("Hello");
               item->setSelectionColours( CEGUI::colour(0.0, 1.0, 0.0, 1.0) );
               win->addItem(item);
             }


    private: void Init()
    {
      rendering::UserCameraPtr userCam = gui::get_active_camera();
      if (userCam && userCam->GetGUIOverlay())
      {
        userCam->GetGUIOverlay()->LoadLayout( "gui_test.layout" );
      }

    }

    private: transport::NodePtr node;
    private: std::vector<event::ConnectionPtr> connections;
    private: rendering::CameraPtr camera;
  };
  
  // Register this plugin with the simulator
  GZ_REGISTER_GUI_PLUGIN(GUITest)
}
 
