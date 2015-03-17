/*
 * Copyright (C) 2015 Open Source Robotics Foundation
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
#ifndef _GAZEBO_LOG_PLAY_WIDGET_HH_
#define _GAZEBO_LOG_PLAY_WIDGET_HH_

#include <vector>
#include <list>
#include <string>

#include "gazebo/gui/qt.h"
#include "gazebo/transport/TransportTypes.hh"
#include "gazebo/msgs/MessageTypes.hh"
#include "gazebo/common/Event.hh"
#include "gazebo/common/Time.hh"
#include "gazebo/util/system.hh"

class QLineEdit;
class QLabel;

namespace gazebo
{
  namespace gui
  {
    class LogPlayWidgetPrivate;
    class LogPlayViewPrivate;

    class GAZEBO_VISIBLE LogPlayWidget : public QWidget
    {
      Q_OBJECT

      /// \brief Constructor
      public: LogPlayWidget(QWidget *_parent = 0);

      /// \brief Destructor
      public: virtual ~LogPlayWidget();

      /// \brief Called when a world stats message is received.
      /// \param[in] _msg World statistics message.
      private: void OnStats(ConstWorldStatisticsPtr &_msg);

      /// \brief Helper function to format time string.
      /// \param[in] _msg Time message.
      private: static std::string FormatTime(const msgs::Time &_msg);

      /// \internal
      /// \brief Pointer to private data.
      private: LogPlayWidgetPrivate *dataPtr;
    };

    // TODO
    class GAZEBO_VISIBLE LogPlayView: public QGraphicsView
    {
      Q_OBJECT

      /// \brief Constructor;
      public: LogPlayView(LogPlayWidget *_parent = 0);

      /// \brief Qt mouse release event.
      /// \param[in] _event Qt mouse event.
      public: void SetTime(QString _time);

      /// \brief Qt mouse release event.
      /// \param[in] _event Qt mouse event.
      public: void SetIterations(QString _time);

      /// \brief Qt mouse release event.
      /// \param[in] _event Qt mouse event.
      public: void SetPaused(bool _paused);

      /// \brief Qt mouse release event.
      /// \param[in] _event Qt mouse event.
      private: void mouseReleaseEvent(QMouseEvent *_event);

      /// \internal
      /// \brief Pointer to private data.
      private: LogPlayViewPrivate *dataPtr;
    };
  }
}

#endif
