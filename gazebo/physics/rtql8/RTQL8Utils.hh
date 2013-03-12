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

#ifndef _RTQL8UTILS_HH_
#define _RTQL8UTILS_HH_

#include "gazebo/math/Pose.hh"
#include "gazebo/physics/rtql8/rtql8_inc.h"

namespace gazebo
{
  namespace physics
  {
    /// \ingroup gazebo_physics
    /// \addtogroup gazebo_physics_rtql8 RTQL8 Physics
    /// \brief rtql8 utilities
    /// \{

    /// \brief RTQL8 Utils class
    class RTQL8Utils
    {
      /// \brief Convert from gazebo::math::Pose to Eigen::Matrix4d.
      public: static Eigen::Matrix4d ConvPose(const math::Pose& _pose);

      /// \brief Convert from gazebo::math::Pose to Eigen::Matrix4d.
      public: static bool ConvPose(Eigen::Matrix4d* _mat,
                                   const math::Pose& _pose);
    };
    /// \}
  }
}
#endif
