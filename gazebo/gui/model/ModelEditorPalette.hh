/*
 * Copyright (C) 2013-2014 Open Source Robotics Foundation
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
#include <vector>

#include "gazebo/rendering/RenderTypes.hh"
#include "gazebo/common/Event.hh"

#include "gazebo/gui/model/ModelCreator.hh"
#include "gazebo/gui/qt.h"
#include "gazebo/util/system.hh"

namespace gazebo
{
  namespace rendering
  {
  }

  namespace gui
  {
    class ModelCreator;

    /// \addtogroup gazebo_gui
    /// \{

    /// \class ModelEditorPalette ModelEditorPalette.hh
    /// \brief A palette of model items which can be added to the editor.
    class GAZEBO_VISIBLE ModelEditorPalette : public QWidget
    {
      Q_OBJECT

      /// \brief Constructor
      /// \param[in] _parent Parent QWidget.
      public: ModelEditorPalette(QWidget *_parent = 0);

      /// \brief Destructor
      public: ~ModelEditorPalette();

      /// \brief Add a joint to the model.
      /// \param[in] _type Type of joint to add.
      public: void AddJoint(const std::string &_type);

      /// \brief Get the model creator.
      /// \return a pointer to the model creator.
      public: ModelCreator *GetModelCreator();

      /// \brief Key event filter callback when key is pressed.
      /// \param[in] _event The key event.
      /// \return True if the event was handled
      private: bool OnKeyPress(const common::KeyEvent &_event);

      /// \brief Received item selection user input.
      /// \param[in] _item Item selected.
      /// \param[in] _column Column index.
      private slots: void OnItemSelection(QTreeWidgetItem *_item, int _column);

      /// \brief Qt callback when cylinder button is clicked.
      private slots: void OnCylinder();

      /// \brief Qt callback when sphere button is clicked.
      private slots: void OnSphere();

      /// \brief Qt callback when box button is clicked.
      private slots: void OnBox();

      /// \brief Qt callback when custom button is clicked.
      private slots: void OnCustom();

      /// \brief Qt callback when a part has been added.
      private slots: void OnPartAdded();

      /// \brief Qt callback when the model is to be made static.
      private slots: void OnStatic();

      /// \brief Qt callback when the model is allowed to auto disable at rest.
      private slots: void OnAutoDisable();

      /// \brief Qt callback when the Model Name field is changed.
      /// \param[in] _name New name.
      private slots: void OnNameChanged(const QString &_name);

      /// \brief Callback when user has provided information on where to save
      /// the model to.
      /// \param[in] _saveName Name of model being saved.
      private: void OnSaveModel(const std::string &_saveName);

      /// \brief Event received when the user starts a new model.
      private: void OnNewModel();

      /// \brief A list of gui editor events connected to this palette.
      private: std::vector<event::ConnectionPtr> connections;

      /// \brief Parts button group.
      private: QButtonGroup *partButtonGroup;

      /// \brief Model creator.
      private: ModelCreator *modelCreator;

      /// \brief Static checkbox, true to create a static model.
      private: QCheckBox *staticCheck;

      /// \brief Auto disable checkbox, true to allow model to auto-disable at
      /// rest.
      private: QCheckBox *autoDisableCheck;

      /// \brief Default name of the model.
      private: std::string modelDefaultName;

      /// \brief Edit the name of the model.
      private: QLineEdit *modelNameEdit;
    };
  }
}
#endif
