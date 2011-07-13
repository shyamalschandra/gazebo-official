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
/* Desc: Box geometry
 * Author: Nate Keonig, Andrew Howard
 * Date: 8 May 2003
 */

#ifndef BOXSHAPE_HH
#define BOXSHAPE_HH

#include "physics/Shape.hh"

namespace gazebo
{
	namespace physics
  {
    /// \brief Box geom
    class BoxShape : public Shape
    {
      /// \brief Constructor
      public: BoxShape(GeomPtr parent);
  
      /// \brief Destructor
      public: virtual ~BoxShape();
  
      /// \brief Load the box
      public: virtual void Load( sdf::ElementPtr &_sdf );

      /// \brief Initialize the box
      public: virtual void Init();
 
      /// \brief Set the size of the box
      public: virtual void SetSize( const math::Vector3 &size );
    };
  }
}
#endif
