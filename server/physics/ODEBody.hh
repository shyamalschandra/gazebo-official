/*
 *  Gazebo - Outdoor Multi-Robot Simulator
 *  Copyright (C) 2003
 *     Nate Koenig & Andrew Howard
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/* Desc: Body class
 * Author: Nate Koenig
 * Date: 13 Feb 2006
 * SVN: $Id: Body.hh 7640 2009-05-13 02:06:08Z natepak $
 */

#ifndef BODY_HH
#define BODY_HH

#include <ode/ode.h>

#include "Body.hh"

namespace gazebo
{
  class XMLConfigNode;

  /// \addtogroup gazebo_physics
  /// \brief The body class
  /// \{

  /// Body class
  class ODEBody : public Body
  {
    /// \brief Constructor
    public: ODEBody(Entity *parent);

    /// \brief Destructor
    public: virtual ~ODEBody();

    /// \brief Load the body based on an XMLConfig node
    /// \param node XMLConfigNode pointer
    public: virtual void LoadChild(XMLConfigNode *node);

    /// \brief Initialize the body
    public: virtual void InitChild();

    /// \brief Finalize the body
    public: virtual void FiniChild();

    /// \brief Update the body
    public: virtual void UpdateChild();

    /// \brief Attach a geom to this body
    /// \param geom Geometery to attach to this body
    public: virtual void AttachGeom( Geom *geom );

    /// \brief Set the position of the body
    /// \param pos Vector position
    public: virtual void SetPosition(const Vector3 &pos);

    /// \brief Set the rotation of the body
    /// \param rot Quaternion rotation
    public: virtual void SetRotation(const Quatern &rot);

    /// \brief Return the position of the body
    /// \return Position vector
    public: virtual Vector3 GetPosition() const;

    /// \brief Return the rotation
    /// \return Rotation quaternion
    public: virtual Quatern GetRotation() const;

    /// \brief Return the velocity of the body
    /// \return Velocity vector
    public: virtual Vector3 GetPositionRate() const;

    /// \brief Return the rotation rates
    /// \return Rotation Rate quaternion
    public: virtual Quatern GetRotationRate() const;

    /// \brief Return the rotation rates
    /// \return Rotation Rate Euler Angles RPY
    public: virtual Vector3 GetEulerRate() const;

    /// \brief Return the ID of this body
    /// \return ODE body id
    public: dBodyID GetId() const;

    /// \brief Set whether this body is enabled
    public: virtual void SetEnabled(bool enable) const;

    /// \brief Update the center of mass
    public: virtual void UpdateCoM();

    /// \brief Set the linear velocity of the body
    public: virtual void SetLinearVel(const Vector3 &vel);

    /// \brief Get the linear velocity of the body
    public: virtual Vector3 GetLinearVel() const;

    /// \brief Set the angular velocity of the body
    public: virtual void SetAngularVel(const Vector3 &vel);

    /// \brief Get the angular velocity of the body
    public: virtual Vector3 GetAngularVel() const;

    /// \brief Set the force applied to the body
    public: virtual void SetForce(const Vector3 &force);

    /// \brief Get the force applied to the body
    public: virtual Vector3 GetForce() const;

    /// \brief Set the angular acceleration of the body
    public: virtual void SetAngularAccel(const Vector3 &accel);

    /// \brief Get the angular acceleration of the body
    public: virtual Vector3 GetAngularAccel() const;

    /// \brief Set the torque applied to the body
    public: virtual void SetTorque(const Vector3 &force);

    /// \brief Get the torque applied to the body
    public: virtual Vector3 GetTorque() const;

    /// \brief Set whether gravity affects this body
    public: virtual void SetGravityMode(bool mode);

    /// ODE body handle
    private: dBodyID bodyId;

    /// Mass properties of the object
    private: dMass mass;
  };

  /// \}
}

#endif
