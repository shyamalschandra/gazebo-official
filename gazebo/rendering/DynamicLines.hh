/*
 * Copyright 2012 Nate Koenig
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
/* Desc: Dynamic line generator
 * Author: Nate Koenig
 * Date: 28 June 2007
 */

#ifndef DYNAMICLINES_HH
#define DYNAMICLINES_HH

#include <vector>
#include <string>

#include "math/Vector3.hh"
#include "rendering/DynamicRenderable.hh"

namespace gazebo
{
  namespace rendering
  {
    /// \addtogroup gazebo_rendering
    /// \{

    /// \class DynamicLines DynamicLines.hh rendering/rendering.hh
    /// \brief Class for drawing lines that can change
    class DynamicLines : public DynamicRenderable
    {
      /// \brief Constructor
      /// \param[in] _opType The type of Line
      public: DynamicLines(RenderOpType _opType = RENDERING_LINE_STRIP);

      /// \brief Destructor
      public: virtual ~DynamicLines();

      /// \brief Get type of movable
      /// \return This returns "gazebo::dynamiclines"
      public: static std::string GetMovableType();

      /// \brief Overridden function from Ogre's base class.
      /// \return Returns "gazebo::ogredynamicslines"
      public: virtual const Ogre::String &getMovableType() const;

      /// \brief Add a point to the point list
      /// \param[in] pt math::Vector3 point
      public: void AddPoint(const math::Vector3 &_pt);

      /// \brief Add a point to the point list.
      /// \param[in] _x X position.
      /// \param[in] _y Y position.
      /// \param[in] _z Z position.
      public: void AddPoint(double _x, double _y, double _z);

      /// \brief Change the location of an existing point in the point list
      /// \param[in] _index Index of the point to set
      /// \param[in] _value math::Vector3 value to set the point to
      public: void SetPoint(unsigned int _index, const math::Vector3 &_value);

      /// \brief Return the location of an existing point in the point list
      /// \param[in] _index Number of the point to return
      /// \return math::Vector3 value of the point
      public: const math::Vector3& GetPoint(unsigned int _index) const;

      /// \brief Return the total number of points in the point list
      /// \return Number of points
      public: unsigned int GetPointCount() const;

      /// \brief Remove all points from the point list
      public: void Clear();

      /// \brief Call this to update the hardware buffer after making changes.
      public: void Update();

      /// \brief Implementation DynamicRenderable,
      /// creates a simple vertex-only decl
      protected: virtual void  CreateVertexDeclaration();

      /// \brief Implementation DynamicRenderable, pushes point
      /// list out to hardware memory
      protected: virtual void FillHardwareBuffers();

      /// \brief List of points for the line
      private: std::vector<math::Vector3> points;

      /// \brief Used to indicate if the lines require an update
      private: bool dirty;
    };
    /// \}
  }
}
#endif
