/*
 * Copyright (C) 2012-2014 Open Source Robotics Foundation
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
#ifndef _GAZEBO_ODEBALLJOINT_HH_
#define _GAZEBO_ODEBALLJOINT_HH_

#include "gazebo/physics/BallJoint.hh"
#include "gazebo/physics/ode/ODEJoint.hh"
#include "gazebo/util/system.hh"

namespace gazebo
{
  namespace physics
  {
    /// \brief An ODEBallJoint
    class GAZEBO_VISIBLE ODEBallJoint : public BallJoint<ODEJoint>
    {
      /// \brief Constructor
      /// \param[in] _worldID ODE id of the world.
      /// \param[in] _parent Parent of the Joint
      public: ODEBallJoint(dWorldID _worldId, BasePtr _parent);

      /// \brief Destructor.
      public: virtual ~ODEBallJoint();

      // Documentation inherited
      public: virtual ignition::math::Vector3d GetAnchor(
                  unsigned int _index) const;

      // Documentation inherited
      public: virtual void SetAnchor(unsigned int _index,
                                     const ignition::math::Vector3d &_anchor);

      // Documentation inherited
      public: virtual ignition::math::Vector3d GetGlobalAxis(
                  unsigned int _index) const;

      // Documentation inherited
      public: virtual void SetVelocity(unsigned int _index, double _angle);

      // Documentation inherited
      public: virtual double GetVelocity(unsigned int _index) const;

      // Documentation inherited
      public: virtual double GetMaxForce(unsigned int _index);

      // Documentation inherited
      public: virtual void SetMaxForce(unsigned int _index, double _t);

      // Documentation inherited
      public: virtual ignition::math::Angle GetAngleImpl(
                  unsigned int _index) const;

      // Documentation inherited.
      public: virtual void SetAxis(unsigned int _index,
                                   const ignition::math::Vector3d &_axis);

      // Documentation inherited.
      public: virtual ignition::math::Angle GetHighStop(unsigned int _index);

      // Documentation inherited.
      public: virtual ignition::math::Angle GetLowStop(unsigned int _index);

      // Documentation inherited.
      public: virtual bool SetHighStop(unsigned int _index,
                                       const ignition::math::Angle &_angle);

      // Documentation inherited.
      public: virtual bool SetLowStop(unsigned int _index,
                                      const ignition::math::Angle &_angle);

      // Documentation inherited.
      protected: virtual void SetForceImpl(unsigned int _index, double _torque);
    };
  }
}
#endif
