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

#ifndef _TERRAIN_EDITOR_PALETTE_HH_
#define _TERRAIN_EDITOR_PALETTE_HH_

#include <string>

#include "gazebo/rendering/RenderTypes.hh"
#include "gazebo/common/Event.hh"
#include "gazebo/common/MouseEvent.hh"
#include "gazebo/gui/qt.h"

namespace gazebo
{
  namespace rendering
  {
    class Heightmap;
  }

  namespace gui
  {
    /// \addtogroup gazebo_gui
    /// \{

    /// \class TerrainEditorPalette TerrainEditorPalette.hh
    /// \brief A palette of building items which can be added to the editor.
    class TerrainEditorPalette : public QWidget
    {
      Q_OBJECT

      /// \brief Constructor
      /// \param[in] _parent Parent QWidget.
      public: TerrainEditorPalette(QWidget *_parent = 0);

      /// \brief Destructor
      public: ~TerrainEditorPalette();

      /// \brief Mouse event filter callback when mouse button is pressed.
      /// \param[in] _event The mouse event.
      /// \return True if the brush was applied
      private: bool OnMousePress(const common::MouseEvent &_event);

      /// \brief Mouse event filter callback when mouse is moved.
      /// \param[in] _event The mouse event.
      /// \return True if the brush was applied
      private: bool OnMouseMove(const common::MouseEvent &_event);

      /// \brief Apply a brush
      /// \param[in] _event The mouse event.
      /// \return True if the brush was applied
      private: bool Apply(const common::MouseEvent &_event,
                   rendering::CameraPtr _camera,
                   rendering::Heightmap *_heightmap);

      /// \brief Set the current state.
      /// \param[in] _state String state(lower, raise, flatten ...). Empty
      /// string to disable terrain modification.
      private: void SetState(const std::string &_state);

      /// \brief QT callback when Raise button is toggled.
      /// \param[in] _toggle True if the button is enabled.
      private slots: void OnRaise(bool _toggle);

      /// \brief QT callback when Lower button is toggled.
      /// \param[in] _toggle True if the button is enabled.
      private slots: void OnLower(bool _toggle);

      /// \brief QT callback when Flatten button is toggled.
      /// \param[in] _toggle True if the button is enabled.
      private slots: void OnFlatten(bool _toggle);

      /// \brief QT callback when Course button is toggled.
      /// \param[in] _toggle True if the button is enabled.
      private slots: void OnCourse(bool _toggle);

      /// \brief QT callback when image is saved.
      private slots: void OnSave();

      /// \brief Button to raise the terrain.
      private: QPushButton *raiseButton;

      /// \brief Button to lower the terrain.
      private: QPushButton *lowerButton;

      /// \brief Button to flatten the terrain.
      private: QPushButton *flattenButton;

      /// \brief Button to roughen the terrain.
      private: QPushButton *courseButton;

      /// \brief Slider to control the size of the brush.
      private: QSlider *brushSizeSlider;

      /// \brief Slider to control the weight of the brush.
      private: QSlider *brushWeightSlider;

      /// \brief The current brush state.
      private: std::string state;
    };
  }
}
#endif
