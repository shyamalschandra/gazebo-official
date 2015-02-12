/*
 * Copyright (C) 2012-2015 Open Source Robotics Foundation
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
#ifndef __GAZEBO_GUI_PLUGIN__
#define __GAZEBO_GUI_PLUGIN__

#include <sdf/sdf.hh>
#include "gazebo/gui/qt.h"

/// \brief GUI plugin QT interface
/// All GUI plugins must inherit from this class.
///
/// Example:
///
/// #ifndef _MYEXAMPLE_
/// #define _MYEXAMPLE_
///
/// #include "gazebo/gui/GuiInterface.hh"
///
/// class MyPlugin : public QWidget, GUIPlugin
/// {
///   Q_OBJECT
///   Q_INTERFACES(GUIPlugin)
///
///   public: virtual ~MyPlugin();
///   public: virtual void Load();
/// };
///
/// #endif

namespace gazebo
{
  namespace gui
  {
    class GUIPlugin
    {
      /// \brief Destructor
      public: virtual ~GUIPlugin() {}

      /// \brief The Load function is called when the plugin is created.
      /// \param[in] _elem Pointer to an SDF element. This parameter could be
      /// NULL, particularly in the case when a plugin is a loaded from
      /// the gui.ini file.
      public: virtual void Load(sdf::ElementPtr _elem) = 0;
    };
  }
}
Q_DECLARE_INTERFACE(gazebo::gui::GUIPlugin, "gazebo.GUIPlugin/1.0")
#endif
