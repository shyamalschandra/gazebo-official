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
#ifndef _TEXTVIEW_HH_
#define _TEXTVIEW_HH_

#include "gazebo/common/Time.hh"
#include "gazebo/msgs/msgs.hh"

#include "gazebo/transport/TransportTypes.hh"

#include "gazebo/gui/qt.h"
#include "gazebo/gui/viewers/TopicView.hh"

namespace gazebo
{
  namespace gui
  {
    class TextView : public TopicView
    {
      Q_OBJECT

      /// \brief Constructor
      /// \param[in] _parent Pointer to the parent widget.
      public: TextView(QWidget *_parent = 0);

      /// \brief Destructor
      public: virtual ~TextView();

      // Documentation inherited
      public: virtual void SetTopic(const std::string &_topicName);

      // Documentation inherited
      private: virtual void UpdateImpl();

      /// \brief Receives incoming text messages.
      /// \param[in] _msg New text message.
      private: void OnText(ConstGzStringPtr &_msg);

      /// \brief A scolling list of text data.
      private: QListView *textView;
    };
  }
}
#endif
