/*
 * Copyright 2011 Nate Koenig & Andrew Howard
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
// This was leveraged from rviz.

#ifndef GRID_HH
#define GRID_HH

#include <stdint.h>
#include <vector>

// TODO: remove this line
#include "rendering/ogre.h"

#include "common/Param.hh"
#include "common/Color.hh"

namespace Ogre
{
  class SceneManager;
  
  class ManualObject;
  class SceneNode;
  
  class Any;
}

namespace gazebo
{
	namespace rendering
  {
    class Scene;
    
    /**
     * \class Grid
     * \brief Displays a grid of cells, drawn with lines
     *
     * Displays a grid of cells, drawn with lines.  A grid with an identity orientation is drawn along the XZ plane.
     */
    class Grid
    {
      /// \brief Constructor
      ///
      /// \param Scene The scene this object is part of
      /// \param cell_count The number of cells to draw
      /// \param cell_length The size of each cell
      /// \param r Red color component, in the range [0, 1]
      /// \param g Green color component, in the range [0, 1]
      /// \param b Blue color component, in the range [0, 1]
      public: Grid( Scene *scene, uint32_t cell_count, float cell_length, 
                    float line_width, const common::Color &color );
  
      /// \brief Destructor
      public: ~Grid();
  
      public: void Init();
  
      
      /// \brief Get the Ogre scene node associated with this grid
      /// \return The Ogre scene node associated with this grid
      public: Ogre::SceneNode* GetSceneNode() { return this->sceneNode; }
      
      /// \brief Sets user data on all ogre objects we own
      public: void SetUserData( const Ogre::Any& data );
      
      public: void SetColor(const common::Color& color);
      public: common::Color GetColor() { return **this->colorP; }
  
      public: void SetCellCount(uint32_t count);
      public: float GetCellCount() { return **this->cellCountP; }
  
      public: void SetCellLength(float len);
      public: float GetCellLength() { return **this->cellLengthP; }
  
      public: void SetLineWidth(float width);
      public: float GetLineWidth() { return **this->lineWidthP; }
  
      public: void SetHeight(uint32_t count);
      public: uint32_t GetHeight() { return this->height; }
      
      private: void Create();
  
      private: Ogre::SceneNode* sceneNode;           ///< The scene node that this grid is attached to
      private: Ogre::ManualObject* manualObject;     ///< The manual object used to draw the grid
      
      private: Ogre::MaterialPtr material;
  
      private: common::ParamT<unsigned int> *cellCountP;
      private: common::ParamT<float> *cellLengthP;
      private: common::ParamT<float> *lineWidthP;
      private: common::ParamT<common::Color> *colorP;
      private: common::ParamT<float> *h_offsetP;
      private: std::vector<common::Param*> parameters;
  
      private: std::string name;
      private: unsigned int height;
  
      private: Scene *scene;
    };
  
  }

}
#endif
