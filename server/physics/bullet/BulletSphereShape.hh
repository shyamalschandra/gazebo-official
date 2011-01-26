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
/* Desc: Sphere geometry
 * Author: Nate Keonig
 * Date: 21 May 2009
 * SVN: $Id:$
 */

#ifndef BULLETSPHERESHAPE_HH
#define BULLETSPHERESHAPE_HH

#include "GazeboError.hh"
#include "BulletPhysics.hh"
#include "SphereShape.hh"

namespace gazebo
{
  /// \brief Bullet sphere geom
  class BulletSphereShape : public SphereShape
  {
    /// \brief Constructor
    public: BulletSphereShape(Geom *parent) : SphereShape(parent) {}

    /// \brief Destructor
    public: virtual ~BulletSphereShape() {}

    /// \brief Set the size
    public: void SetSize(const double &radius)
            {
              SphereShape::SetSize(radius);
              BulletGeom *bParent = (BulletGeom*)(this->parent);
              bParent->SetCollisionShape( new btSphereShape(radius) );
            }
  };
}

#endif
