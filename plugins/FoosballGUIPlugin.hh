/*
 * Copyright (C) 2014-2015 Open Source Robotics Foundation
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
#ifndef _GUI_FOOSBALL_PLUGIN_HH_
#define _GUI_FOOSBALL_PLUGIN_HH_

#include <sdf/sdf.hh>
#include <gazebo/common/Plugin.hh>
#include <gazebo/gui/GuiPlugin.hh>
#ifndef Q_MOC_RUN  // See: https://bugreports.qt-project.org/browse/QTBUG-22829
# include <gazebo/transport/transport.hh>
# include <gazebo/gui/gui.hh>
#endif

namespace gazebo
{
  /// \brief A GUI plugin that ...
  class GAZEBO_VISIBLE FoosballGUIPlugin : public GUIPlugin
  {
    Q_OBJECT

    /// \brief Constructor.
    public: FoosballGUIPlugin();

    /// \brief Destructor.
    public: virtual ~FoosballGUIPlugin();

    // Documentation inherited.
    public: void Load(sdf::ElementPtr _elem);

    /// \brief SDF for this plugin.
    private: sdf::ElementPtr sdf;
  };
}

#endif
