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
/* Desc: ODE Body class
 * Author: Nate Koenig
 * Date: 15 May 2009
 * SVN: $Id$
 */

#ifndef ODEBODY_HH
#define ODEBODY_HH

#include "ODEPhysics.hh"
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
    public: virtual void Load(XMLConfigNode *node);

    /// \brief Initialize the body
    public: virtual void Init();

    /// \brief Finalize the body
    public: virtual void Fini();

    /// \brief Update the body
    public: virtual void Update();

    /// \brief Attach a geom to this body
    /// \param geom Geometery to attach to this body
    public: virtual void AttachGeom( Geom *geom );

    /// \brief Called when the pose of the entity (or one of its parents) has
    /// changed
    public: virtual void OnPoseChange();

    /// \brief Return the ID of this body
    /// \return ODE body id
    public: dBodyID GetODEId() const;

    /// \brief Set whether this body is enabled
    public: virtual void SetEnabled(bool enable) const;

    /// \brief Get whether this body is enabled in the physics engine
    public: virtual bool GetEnabled() const;

    /// \brief Update the center of mass
    public: virtual void UpdateCoM();

    /// \brief Set the linear velocity of the body
    public: virtual void SetLinearVel(const Vector3 &vel);

    /// \brief Set the angular velocity of the body
    public: virtual void SetAngularVel(const Vector3 &vel);

    /// \brief Set the force applied to the body
    public: virtual void SetForce(const Vector3 &force);

    /// \brief Set the torque applied to the body
    public: virtual void SetTorque(const Vector3 &force);

    /// \brief Get the linear velocity of the body in the world frame
    public: virtual Vector3 GetWorldLinearVel() const;

    /// \brief Get the angular velocity of the body in the world frame
    public: virtual Vector3 GetWorldAngularVel() const;

    /// \brief Get the force applied to the body in the world frame
    public: virtual Vector3 GetWorldForce() const;

    /// \brief Get the torque applied to the body in the world frame
    public: virtual Vector3 GetWorldTorque() const;

    /// \brief Set whether gravity affects this body
    public: virtual void SetGravityMode(bool mode);

    /// \brief Get the gravity mode
    public: virtual bool GetGravityMode();

    /// \brief Set whether this body will collide with others in the model
    public: void SetSelfCollide(bool collide);

    /// \brief Get the body's space ID
    public: dSpaceID GetSpaceId() const;

    /// \brief Set the body's space ID
    public: void SetSpaceId(dSpaceID spaceid);

    /// \brief Set the linear damping factor
    public: virtual void SetLinearDamping(double damping);

    /// \brief Set the angular damping factor
    public: virtual void SetAngularDamping(double damping);

    public: static void MoveCallback(dBodyID id);

    /// \brief Set whether this body is in the kinematic state
    public: virtual void SetKinematic(const bool &state);

    /// \brief Get whether this body is in the kinematic state
    public: virtual bool GetKinematic() const;

    protected: Pose3d pose;

    /// ODE body handle
    private: dBodyID bodyId;

    private: ODEPhysics *odePhysics;

    private: dSpaceID spaceId;
  };

  /// \}
}

#endif
