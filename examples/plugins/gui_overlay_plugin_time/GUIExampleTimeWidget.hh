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
#ifndef _GUI_EXAMPLE_TIME_WIDGET_HH_
#define _GUI_EXAMPLE_TIME_WIDGET_HH_
/*
#include "gazebo/gui/qt.h"
#include "gazebo/gui/GUIPlugin.hh"
namespace gazebo
{
  namespace gui
  {
    class MyExample : public QWidget, GUIPlugin
    {
      Q_OBJECT
      Q_INTERFACES(gazebo::gui::GUIPlugin)

      public: virtual ~MyExample();
      public: virtual void Load(sdf::ElementPtr _elem);
    };
  }
}
*/

#include <string>

#include <gazebo/gui/GUIPlugin.hh>
#ifndef Q_MOC_RUN  // See: https://bugreports.qt-project.org/browse/QTBUG-22829
# include <gazebo/transport/transport.hh>
# include <gazebo/gui/gui.hh>
#endif

namespace gazebo
{
  namespace gui
  {
    class GUIExampleTimeWidget : public QWidget, GUIPlugin
    {
      Q_OBJECT
      Q_INTERFACES(gazebo::gui::GUIPlugin)

      /// \brief Destructor
      public: virtual ~GUIExampleTimeWidget();

      /// \brief The Load function is called when the plugin is created.
      /// \param _elem Pointer to an SDF element. This parameter could be NULL,
      /// particularly in the case when a plugin is a loaded from the gui.ini
      /// file.
      public: virtual void Load(sdf::ElementPtr _elem);

      /// \brief A signal used to set the sim time line edit.
      /// \param[in] _string String representation of sim time.
      signals: void SetSimTime(QString _string);

      /// \brief Callback that received world statistics messages.
      /// \param[in] _msg World statistics message that is received.
      private: void OnStats(ConstWorldStatisticsPtr &_msg);

      /// \brief Helper function to format time string.
      /// \param[in] _msg Time message.
      /// \return Time formatted as a string.
      private: std::string FormatTime(const msgs::Time &_msg) const;

      /// \brief Node used to establish communication with gzserver.
      private: transport::NodePtr node;

      /// \brief Subscriber to world statistics messages.
      private: transport::SubscriberPtr statsSub;
      private: rendering::UserCameraPtr cam;
    };
  }
}
#endif
