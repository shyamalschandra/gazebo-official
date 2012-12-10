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
#ifndef _TOPICSELECTOR_HH_
#define _TOPICSELECTOR_HH_

#include "gazebo/gui/qt.h"

namespace gazebo
{
  namespace gui
  {
    /// \addtogroup gazebo_gui
    /// \{

    /// \class TopicSelector TopicSelector.hh gui/TopicSelector.hh
    /// \brief A widget that provides a list of topics to select from.
    class TopicSelector : public QWidget
    {
      Q_OBJECT

      /// \brief Constructor
      /// \param[in] _parent Parent widget pointer.
      public: TopicSelector(QWidget *_parent = 0);

      /// \brief Destructor
      public: virtual ~TopicSelector();

      private: void GetTopicList();

      private: QTreeWidget *treeWidget;
    };
    /// \}
  }
}
#endif
