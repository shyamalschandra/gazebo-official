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
/* Desc: common::Parameters for a surface
 * Author: Nate Koenig
 * Date: 30 July 2003
 */

#ifndef _SURFACEPARAMS_HH_
#define _SURFACEPARAMS_HH_

#include "gazebo/msgs/msgs.hh"
#include "gazebo/sdf/sdf.hh"

namespace gazebo
{
  namespace physics
  {
    /// \addtogroup gazebo_physics
    /// \{

    /// \class SurfaceParams SurfaceParams.hh physics/physics.hh
    /// \brief SurfaceParams defines various Surface contact parameters.
    /// These parameters defines the properties of a
    /// physics::Contact constraint.
    class SurfaceParams
    {
      /// \brief Constructor.
      public: SurfaceParams();

      /// \brief Destructor.
      public: virtual ~SurfaceParams();

      /// \brief Load the contact params.
      /// \param[in] _sdf SDF values to load from.
      public: virtual void Load(sdf::ElementPtr _sdf);

      /// \brief Deprecated.
      public: void FillSurfaceMsg(msgs::Surface &_msg) GAZEBO_DEPRECATED;

      /// \brief Fill in a surface message.
      /// \param[in] _msg Message to fill with this object's values.
      public: void FillMsg(msgs::Surface &_msg);

      public: virtual void ProcessMsg(const msgs::Surface &_msg);

      /// \brief bounce restitution coefficient [0,1], with 0 being inelastic,
      ///        and 1 being perfectly elastic.
      /// \sa    http://www.ode.org/ode-latest-userguide.html#sec_7_3_7
      public: double bounce;

      /// \brief minimum contact velocity for bounce to take effect, otherwise
      ///        the collision is treated as an inelastic collision.
      /// \sa    http://www.ode.org/ode-latest-userguide.html#sec_7_3_7
      public: double bounceThreshold;

      /// \brief spring constant equivalents of a contact as a function of
      ///        SurfaceParams::cfm and SurfaceParams::erp.
      /// \sa    See for example
      ///        http://www.ode.org/ode-latest-userguide.html#sec_3_8_2
      ///        for more details.
      public: double kp;

      /// \brief spring damping constant equivalents of a contact as a
      ///        function of SurfaceParams::cfm and SurfaceParams::erp.
      /// \sa    See for example
      ///        http://www.ode.org/ode-latest-userguide.html#sec_3_8_2
      ///        for more details.
      public: double kd;

      /// \brief Constraint Force Mixing parameter.
      ///        See for example
      ///        http://www.ode.org/ode-latest-userguide.html#sec_3_8_0
      ///        for more details.
      public: double cfm;

      /// \brief Error Reduction Parameter.
      /// \sa    See for example
      ///        http://www.ode.org/ode-latest-userguide.html#sec_3_8_0
      ///        for more details.
      public: double erp;

      /// \brief Maximum interpenetration error correction velocity.  If
      ///        set to 0, two objects interpenetrating each other
      ///        will not be pushed apart.
      /// \sa    See dWroldSetContactMaxCorrectingVel
      ///        (http://www.ode.org/ode-latest-userguide.html#sec_5_2_0)
      public: double maxVel;

      /// \brief Minimum depth before ERP takes effect.
      /// \sa    See dWorldSetContactSurfaceLayer
      ///        (http://www.ode.org/ode-latest-userguide.html#sec_5_2_0)
      public: double minDepth;

      /// \brief Dry friction coefficient in the primary friction direction
      ///        as defined by the friction pyramid.  This is fdir1 if defined,
      ///        otherwise, a vector consstrained to be perpendicular to the
      ///        contact normal in the global y-z plane is used.
      /// \sa    http://www.ode.org/ode-latest-userguide.html#sec_7_3_7
      public: double mu1;

      /// \brief Dry friction coefficient in the second friction direction
      ///        as defined by the friction pyramid.  This is fdir1 if defined,
      ///        otherwise, a vector consstrained to be perpendicular to the
      ///        contact normal in the global y-z plane is used.
      /// \sa    http://www.ode.org/ode-latest-userguide.html#sec_7_3_7
      public: double mu2;

      /// \brief Artificial contact slip in the primary friction direction.
      /// \sa    See dContactSlip1 in
      ///        http://www.ode.org/ode-latest-userguide.html#sec_7_3_7
      public: double slip1;

      /// \brief Artificial contact slip in the secondary friction dirction.
      /// \sa    See dContactSlip2 in
      ///        http://www.ode.org/ode-latest-userguide.html#sec_7_3_7
      public: double slip2;

      /// \brief Primary friction direction for dry friction coefficient
      ///        (SurfaceParams::mu1) of the friction pyramid.
      ///        If undefined, a vector consstrained to be perpendicular
      ///        to the contact normal in the global y-z plane is used.
      /// \sa    http://www.ode.org/ode-latest-userguide.html#sec_7_3_7
      public: math::Vector3 fdir1;
    };
    /// \}
  }
}
#endif
