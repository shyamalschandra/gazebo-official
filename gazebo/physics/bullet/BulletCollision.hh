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
/* Desc: Collision class
 * Author: Nate Koenig
 * Date: 13 Feb 2006
 */

#ifndef _BULLET_COLLISION_HH_
#define _BULLET_COLLISION_HH_

#include <string>

/*

#include "common/Param.hh"
#include "Entity.hh"
#include "math/Pose.hh"
#include "math/Vector3.hh"
#include "physics/Collision.hh"
*/

#include "physics/PhysicsTypes.hh"
#include "physics/Collision.hh"

class btCollisionShape;

namespace gazebo
{
  namespace physics
  {
    /// \ingroup gazebo_physics
    /// \addtogroup gazebo_physics_bullet Bullet Physics
    /// \{

    /// \brief Bullet collisions
    class BulletCollision : public Collision
    {
      /// \brief Constructor
      public: BulletCollision(LinkPtr _parent);

      /// \brief Destructor
      public: virtual ~BulletCollision();

      /// \brief Load the collision
      public: virtual void Load(sdf::ElementPtr _ptr);

      /// \brief On pose change
      public: virtual void OnPoseChange();

      /// \brief Set the category bits, used during collision detection
      /// \param bits The bits
      public: virtual void SetCategoryBits(unsigned int _bits);

      /// \brief Set the collide bits, used during collision detection
      /// \param bits The bits
      public: virtual void SetCollideBits(unsigned int _bits);

      /// \brief Get the category bits, used during collision detection
      /// \return The bits
      public: virtual unsigned int GetCategoryBits() const;

      /// \brief Get the collide bits, used during collision detection
      /// \return The bits
      public: virtual unsigned int GetCollideBits() const;

      /// \brief Get the bounding box, defined by the physics engine
      public: virtual math::Box GetBoundingBox() const;

      /// \brief Set the collision shape
      public: void SetCollisionShape(btCollisionShape *_shape);

      /// \brief Get the bullet collision shape
      public: btCollisionShape *GetCollisionShape() const;

      /// \brief Set the index of the compound shape
      public: void SetCompoundShapeIndex(int i_ndex);

      protected: btCollisionShape *collisionShape;

      /// \brief Category bits for collision detection
      private: unsigned int categoryBits;

      /// \brief Collide bits for collision detection
      private: unsigned int collideBits;
    };
    /// \}
  }
}
#endif
