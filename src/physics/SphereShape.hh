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
/* Desc: Sphere shape
 * Author: Nate Keonig
 * Date: 14 Oct 2009
 */

#ifndef SPHERESHAPE_HH
#define SPHERESHAPE_HH

#include "common/Param.hh"
#include "physics/Shape.hh"
#include "physics/PhysicsTypes.hh"

namespace gazebo
{
	namespace physics
  {
    /// \brief Sphere geom
    class SphereShape : public Shape
    {
      /// \brief Constructor
      public: SphereShape(GeomPtr parent);
  
      /// \brief Destructor
      public: virtual ~SphereShape();
  
      /// \brief Load the sphere
      public: virtual void Load(common::XMLConfigNode *node);

      /// \brief Initialize the sphere
      public: virtual void Init();
 
      /// \brief Save shape parameters
      public: virtual void Save(std::string &prefix, std::ostream &stream);
  
      /// \brief Set the size
      public: virtual void SetSize(const double &radius);
  
      private: common::ParamT<double> *radiusP;
    };
  }
}
#endif
