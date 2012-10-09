/*
 * Copyright 2011 Nate Koenig
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

#ifndef _GUI_ACTIONS_HH_
#define _GUI_ACTIONS_HH_

class QAction;

namespace gazebo
{
  namespace gui
  {
    extern QAction *g_newAct;
    extern QAction *g_openAct;
    extern QAction *g_importAct;
    extern QAction *g_saveAct;
    extern QAction *g_saveAsAct;
    extern QAction *g_aboutAct;
    extern QAction *g_quitAct;

    extern QAction *g_newModelAct;
    extern QAction *g_resetModelsAct;
    extern QAction *g_resetWorldAct;
    extern QAction *g_editWorldPropertiesAct;

    extern QAction *g_playAct;
    extern QAction *g_pauseAct;
    extern QAction *g_stepAct;

    extern QAction *g_boxCreateAct;
    extern QAction *g_sphereCreateAct;
    extern QAction *g_cylinderCreateAct;
    extern QAction *g_meshCreateAct;
    extern QAction *g_pointLghtCreateAct;
    extern QAction *g_spotLghtCreateAct;
    extern QAction *g_dirLghtCreateAct;

    extern QAction *g_viewGridAct;
    extern QAction *g_viewResetAct;
    extern QAction *g_viewFullScreenAct;
    extern QAction *g_viewFPSAct;
    extern QAction *g_viewOrbitAct;

    extern QAction *g_arrowAct;
    extern QAction *g_ringPoseAct;
  }
}
#endif
