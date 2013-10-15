/*
 * Copyright 2013 Open Source Robotics Foundation
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
/* Desc: Bullet motion state class.
 * Author: Nate Koenig
 * Date: 25 May 2009
 */

#ifndef _BULLETMOTIONSTATE_HH_
#define _BULLETMOTIONSTATE_HH_

#include "gazebo/physics/bullet/bullet_inc.h"
#include "gazebo/physics/PhysicsTypes.hh"
#include "gazebo/math/MathTypes.hh"
#include "gazebo/math/Pose.hh"

namespace gazebo
{
  namespace physics
  {
    class Link;

    /// \ingroup gazebo_physics
    /// \addtogroup gazebo_physics_bullet Bullet Physics
    /// \{

    /// \brief Bullet btMotionState encapsulation
    class BulletMotionState : public btMotionState
    {
      /// \brief Constructor
      public: BulletMotionState(LinkPtr _link);

      /// \brief Destructor
      public: virtual ~BulletMotionState();

      /// \brief Get the world transform of the body at the center of gravity.
      /// \param[out] _cogWorldTrans Pose of body center of gravity.
      public: virtual void getWorldTransform(btTransform &_cogWorldTrans) const;

      /// \brief Set the world transform of the body at the center of gravity.
      /// \param[in] _cogWorldTrans Pose of body center of gravity.
      public: virtual void setWorldTransform(const btTransform &_cogWorldTrans);

      /// \brief Pointer to parent link.
      private: LinkPtr link;
    };
    /// \}
  }
}
#endif
