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
/* Desc: Plane shape
 * Author: Nate Keonig
 * Date: 14 Oct 2009
 * SVN: $Id:$
 */

#ifndef PLANESHAPE_HH
#define PLANESHAPE_HH

#include "common/Param.hh"
#include "Shape.hh"

namespace gazebo
{
  namespace common
  {
    class XMLConfig;
  }

	namespace physics
  {
    /// \addtogroup gazebo_physics_geom
    /// \{
    /** \defgroup gazebo_plane_geom Plane Geom
        \brief Geom for an infinite plane.
  
         \par Attributes
        The following attributes are supported.
    
        \htmlinclude default_geom_attr_include.html
   
        - normal (float tuple, unit vector)
          - Set the plane's normal vector
          - Default: 0 0 1
  
        - segments (int tuple)
          - Set the number of segments of the plane. Larger numbers increase the vertex density.
          - Default: 1 1
  
        - uvTile (float tuple)
          - Set the UV tiling, used for texture mapping
          - Default: 1 1
  
        - size (float tuple, meters)
          - Size of the plane
          - Default: 0 0
    
        \par Example
        \verbatim
        <geom:plane name="plane1_geom">
          <normal>0 0 1</normal>
          <size>2000 2000</size>
          <segments>10 10</segments>
          <uvTile>100 100</uvTile>
          <material>Gazebo/GrassFloor</material>
        </geom:plane>
        \endverbatim
    */
    /// \}
    /// \addtogroup gazebo_plane_geom 
    /// \{
  
  
    /// \brief Geom for an infinite plane.
    /// 
    /// This geom is used primarily for ground planes.  Note that while
    /// the plane in infinite, only the part near the camera is drawn.  
    class PlaneShape : public Shape
    {
      /// \brief Constructor
      /// \param body Body to which we are attached.
      public: PlaneShape(Geom *parent);
  
      /// \brief Destructor
      public: virtual ~PlaneShape();
   
      /// \brief Load the plane
      public: virtual void Load(common::XMLConfigNode *node);
  
      /// \brief Save child parameters
      public: virtual void Save(std::string &prefix, std::ostream &stream);
  
      /// \brief Create the plane
      public: virtual void CreatePlane();
  
      /// \brief Set the altitude of the plane
      public: virtual void SetAltitude(const common::Vector3 &pos);
  
      /// \brief Set the normal
      public: void SetNormal( const common::Vector3 &norm );
  
      protected: common::ParamT<common::Vector3> *normalP;
    };
    
    /// \}
  }
}
#endif
