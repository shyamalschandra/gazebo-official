/*
 * Copyright 2012 Nate Koenig
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
/* Desc: A universal joint
 * Author: Nate Koenig, Andrew Howard
 * Date: 21 May 2003
 */

#ifndef _UNIVERSALJOINT_HH_
#define _UNIVERSALJOINT_HH_

#include "gazebo/physics/Joint.hh"

namespace gazebo
{
  namespace physics
  {
    /// \addtogroup gazebo_physics
    /// \{

    /// \class UniversalJoint UniversalJoint.hh physics/physics.hh
    /// \brief A universal joint.
    template<class T>
    class UniversalJoint : public T
    {
      /// \brief Constructor.
      /// \param[in] _parent Parent link of the univeral joint.
      public: explicit UniversalJoint(BasePtr _parent) : T(_parent)
              {this->AddType(Base::UNIVERSAL_JOINT);}

      /// \brief Destuctor.
      public: virtual ~UniversalJoint()
              {}

      /// \brief Load a UniversalJoint.
      /// \param[in] _sdf SDF values to load from.
      public: virtual void Load(sdf::ElementPtr _sdf)
                 {
                   T::Load(_sdf);
                   sdf::ElementPtr axisElem = this->sdf->GetElement("axis");
                   sdf::ElementPtr axis2Elem = this->sdf->GetElement("axis2");

                   math::Vector3 axis =
                     axisElem->Get<gazebo::math::Vector3>("xyz");

                   math::Vector3 axis2 =
                     axis2Elem->Get<gazebo::math::Vector3>("xyz");


                   this->SetAxis(0, axis);
                   this->SetAxis(1, axis2);
                 }
    };
    /// \}
  }
}
#endif
