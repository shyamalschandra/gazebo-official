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

#ifndef _MODEL_EDITOR_PALETTE_HH_
#define _MODEL_EDITOR_PALETTE_HH_

#include <string>

#include "gazebo/rendering/RenderTypes.hh"
#include "gazebo/common/Event.hh"
#include "gazebo/common/MouseEvent.hh"

#include "gazebo/gui/model/JointMaker.hh"
#include "gazebo/gui/model/ModelCreator.hh"
#include "gazebo/gui/qt.h"

namespace gazebo
{
  namespace rendering
  {
  }

  namespace gui
  {
    class JointMaker;
    class ModelCreator;

    /// \addtogroup gazebo_gui
    /// \{

    /// \class ModelEditorPalette ModelEditorPalette.hh
    /// \brief A palette of building items which can be added to the editor.
    class ModelEditorPalette : public QWidget
    {
      Q_OBJECT

      /// \brief Constructor
      /// \param[in] _parent Parent QWidget.
      public: ModelEditorPalette(QWidget *_parent = 0);

      /// \brief Destructor
      public: ~ModelEditorPalette();

      /// \brief Mouse event filter callback when mouse button is pressed in
      /// create part mode.
      /// \param[in] _event The mouse event.
      /// \return True if the event was handled
      private: bool OnMousePressPart(const common::MouseEvent &_event);

      /// \brief Mouse event filter callback when mouse is moved in create
      /// part mode.
      /// \param[in] _event The mouse event.
      /// \return True if the event was handled
      private: bool OnMouseMovePart(const common::MouseEvent &_event);

      /// \brief Received model selection user input
      private slots: void OnModelSelection(QTreeWidgetItem *_item, int _column);

      /// \brief Qt callback when cylinder button is clicked.
      private slots: void OnCylinder();

      /// \brief Qt callback when sphere button is clicked.
      private slots: void OnSphere();

      /// \brief Qt callback when box button is clicked.
      private slots: void OnBox();

      /// \brief Qt callback when fixed joint button is clicked.
      private slots: void OnFixedJoint();

      /// \brief Qt callback when hinge joint button is clicked.
      private slots: void OnHingeJoint();

      /// \brief Qt callback when hinge2 joint button is clicked.
      private slots: void OnHinge2Joint();

      /// \brief Qt callback when slider joint button is clicked.
      private slots: void OnSliderJoint();

      /// \brief Qt callback when screw joint button is clicked.
      private slots: void OnScrewJoint();

      /// \brief Qt callback when universal joint button is clicked.
      private slots: void OnUniversalJoint();

      /// \brief Qt callback when universal joint button is clicked.
      private slots: void OnBallJoint();

      /// \brief Qt callback when a joint has been added.
      private slots: void OnJointAdded();

      /// \brief Qt callback when a part has been added.
      private slots: void OnPartAdded();

      /// \brief Widget that display model properties.
      private: QTreeWidget *modelTreeWidget;

      /// \brief Model settings item in the tree widget.
      private: QTreeWidgetItem *modelSettingsItem;

      /// \brief Model parts item in the tree widget.
      private: QTreeWidgetItem *modelItem;

      /// \brief Plugin item in the tree widget.
      private: QTreeWidgetItem *pluginItem;

      /// \brief Joints button group.
      private: QButtonGroup *jointsButtonGroup;

      /// \brief Parts button group.
      private: QButtonGroup *partsButtonGroup;

      /// \brief Model creator.
      private: ModelCreator *modelCreator;

      /// \brief Joint maker.
      private: JointMaker *jointMaker;

    };
  }
}
#endif
