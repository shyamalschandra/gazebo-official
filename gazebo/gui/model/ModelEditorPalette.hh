/*
 * Copyright (C) 2013-2015 Open Source Robotics Foundation
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

namespace boost
{
  class recursive_mutex;
}

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

      /// \brief Qt callback when cylinder button is clicked.
      private slots: void OnCylinder();

      /// \brief Qt callback when sphere button is clicked.
      private slots: void OnSphere();

      /// \brief Qt callback when box button is clicked.
      private slots: void OnBox();

      /// \brief Qt callback when custom button is clicked.
      private slots: void OnCustom();

      /// \brief Qt callback when a link has been added.
      private slots: void OnLinkAdded();

      /// \brief Qt callback when the model is to be made static.
      private slots: void OnStatic();

      /// \brief Qt callback when the model is allowed to auto disable at rest.
      private slots: void OnAutoDisable();

      /// \brief Qt callback when the Model Name field is changed.
      /// \param[in] _name New name.
      private slots: void OnNameChanged(const QString &_name);

      /// \brief Qt callback when a tree item has been double clicked.
      /// \param[in] _item Item clicked.
      /// \param[in] _column Column index.
      private slots: void OnItemDoubleClick(QTreeWidgetItem *item, int column);

      /// \brief Add a link to the tree.
      /// \param[in] _linkName Scoped link name.
      private: void OnLinkInserted(const std::string &_linkName);

      /// \brief Add a joint to the tree.
      /// \param[in] _jointId Unique joint identifying name.
      /// \param[in] _jointName Scoped name which can be changed by the user.
      private: void OnJointInserted(const std::string &_jointId,
          const std::string &_jointName);

      /// \brief Remove a link from the tree.
      /// \param[in] _linkId Unique link identifying name.
      private: void OnLinkRemoved(const std::string &_linkId);

      /// \brief Remove a joint from the tree.
      /// \param[in] _jointId Unique joint identifying name.
      private: void OnJointRemoved(const std::string &_jointId);

      /// \brief Remove all links and joints from the tree.
      private: void ClearModelTree();

      /// \brief Update a joint item text in the tree.
      /// \param[in] _jointId Unique joint identifying name.
      /// \param[in] _newJointName New scoped joint name.
      private: void OnJointNameChanged(const std::string &_jointId,
          const std::string &_newJointName);

      /// \brief Callback when user has provided information on where to save
      /// the model to.
      /// \param[in] _saveName Name of model being saved.
      private: void OnSaveModel(const std::string &_saveName);

      /// \brief Event received when the user starts a new model.
      private: void OnNewModel();

      /// \brief Event received when the model properties changed.
      /// \param[in] _static New static property of the model.
      /// \param[in] _autoDisable New allow_auto_disable property of the model.
      /// \param[in] _pose New model pose.
      /// \param[in] _name New name.
      private: void OnModelPropertiesChanged(bool _static, bool _autoDisable,
          const math::Pose &_pose, const std::string &_name);

      /// \brief A list of gui editor events connected to this palette.
      private: std::vector<event::ConnectionPtr> connections;

      /// \brief Links button group.
      private: QButtonGroup *linkButtonGroup;

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

      /// \brief The tree holding all links and joints.
      private: QTreeWidget *modelTreeWidget;

      /// \brief Parent item for all links.
      private: QTreeWidgetItem *linksItem;

      /// \brief Parent item for all joints.
      private: QTreeWidgetItem *jointsItem;

      /// \brief Mutex to protect updates.
      private: boost::recursive_mutex *updateMutex;
    };
  }
}
#endif
