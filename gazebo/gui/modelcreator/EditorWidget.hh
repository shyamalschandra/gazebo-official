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

#ifndef _EDITOR_WIDGET_HH_
#define _EDITOR_WIDGET_HH_

#include <string>
#include <vector>
#include <list>

#include "gui/qt.h"

#include "transport/TransportTypes.hh"

#include "common/MouseEvent.hh"
#include "common/Event.hh"

#include "math/Pose.hh"

#include "msgs/msgs.hh"


namespace gazebo
{
  namespace gui
  {
    class BuildingCreatorWidget;

    class EditorWidget : public QWidget
    {
      Q_OBJECT

      public: EditorWidget(QWidget *_parent = 0);
      public: virtual ~EditorWidget();

      public: void SetMode(int mode);

      private: QFrame *editorFrame;
      private: QWidget *buildingCreatorWidget;

    };
  }
}

#endif
