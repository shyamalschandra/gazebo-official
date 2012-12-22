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

#ifndef _WINDOW_ITEM_HH_
#define _WINDOW_ITEM_HH_

#include "gui/qt.h"

namespace gazebo
{
  namespace gui
  {
    class RectItem;

    class BuildingItem;

    class WindowItem : public RectItem, public BuildingItem
    {
        public: WindowItem();

        public: ~WindowItem();

        public: virtual QVector3D GetSize();

        public: virtual QVector3D GetScenePosition();

        public: virtual double GetSceneRotation();

        private: virtual void paint (QPainter *_painter,
            const QStyleOptionGraphicsItem *_option, QWidget *_widget);

        private: void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *_event);

        private: void WindowChanged();

        private: double windowDepth;

        private: double windowHeight;

        private: double windowWidth;

        private: double windowSideBar;

        private: QPointF windowPos;

        private: double windowElevation;

        private: double scale;
    };
  }
}
#endif
