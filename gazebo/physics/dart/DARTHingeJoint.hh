/*
 * Copyright 2014 Open Source Robotics Foundation
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

#ifndef _GAZEBO_DARTHINGEJOINT_HH_
#define _GAZEBO_DARTHINGEJOINT_HH_

#include <ignition/math/Angle.hh>
#include <ignition/math/Vector3.hh>

#include "gazebo/physics/HingeJoint.hh"
#include "gazebo/physics/dart/DARTJoint.hh"
#include "gazebo/util/system.hh"

namespace gazebo
{
  namespace physics
  {
    /// \brief A single axis hinge joint.
    class GAZEBO_VISIBLE DARTHingeJoint : public HingeJoint<DARTJoint>
    {
      /// \brief Constructor
      /// \param[in] _parent Parent of the Joint
      public: DARTHingeJoint(BasePtr _parent);

      /// \brief Destructor
      public: virtual ~DARTHingeJoint();

      // Documentation inherited.
      public: virtual void Load(sdf::ElementPtr _sdf);

      // Documentation inherited.
      public: virtual void Init();

      // Documentation inherited
      public: virtual ignition::math::Vector3d GetAnchor(
                  unsigned int _index) const;

      // Documentation inherited
      public: virtual ignition::math::Vector3d GetGlobalAxis(
                  unsigned int _index) const;

      // Documentation inherited
      public: virtual void SetAxis(unsigned int _index,
                  const ignition::math::Vector3d &_axis);

      // Documentation inherited
      public: virtual ignition::math::Angle GetAngleImpl(
                  unsigned int _index) const;

      // Documentation inherited
      public: virtual void SetVelocity(unsigned int _index, double _vel);

      // Documentation inherited
      public: virtual double GetVelocity(unsigned int _index) const;

      // Documentation inherited
      public: virtual void SetMaxForce(unsigned int _index, double _force);

      // Documentation inherited
      public: virtual double GetMaxForce(unsigned int _index);

      // Documentation inherited
      protected: virtual void SetForceImpl(unsigned int _index, double _effort);

      /// \brief Revolute joint of DART
      protected: dart::dynamics::RevoluteJoint *dtRevoluteJoint;
    };
  }
}
#endif
