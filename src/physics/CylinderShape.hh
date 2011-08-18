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
/* Desc: Cylinder geometry
 * Author: Nate Keonig, Andrew Howard
 * Date: 8 May 2003
 */

#ifndef CYLINDERSHAPE_HH
#define CYLINDERSHAPE_HH

#include "physics/Shape.hh"

namespace gazebo
{
	namespace physics
  {
    /// \addtogroup gazebo_physics
    /// \{
 
    /// \brief Cylinder geom
    class CylinderShape : public Shape
    {
      /// \brief Constructor
      public: CylinderShape(GeomPtr parent);
  
      /// \brief Destructor
      public: virtual ~CylinderShape();
  
      /// \brief Load the cylinder
      public: virtual void Load( sdf::ElementPtr &_sdf );

      /// \brief Initialize the cylinder
      public: void Init();

      /// \brief Set radius
      public: void SetRadius(const double &radius);
  
      /// \brief Set length
      public: void SetLength(const double &length);
  
  
      /// \brief Set the size of the cylinder
      public: virtual void SetSize( const double &radius, const double &length  );
  
    };
    /// \}
  }
}
#endif
