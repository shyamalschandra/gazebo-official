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

#ifndef _COLLISION_HH_
#define _COLLISION_HH_

#include <string>
#include <vector>

#include "common/Event.hh"
#include "common/CommonTypes.hh"

#include "physics/PhysicsTypes.hh"
#include "physics/CollisionState.hh"
#include "physics/Entity.hh"

namespace gazebo
{
  namespace physics
  {
    /// \addtogroup gazebo_physics
    /// \{

    /// \brief Base class for all collision entities
    class Collision : public Entity
    {
      /// \brief Constructor.
      /// \param[in] _link Link that contains this collision object.
      public: explicit Collision(LinkPtr _link);

      /// \brief Destructor.
      public: virtual ~Collision();

      /// \brief Finalize the collision.
      public: virtual void Fini();

      /// \brief Load the collision.
      /// \param[in] _sdf SDF to load from.
      public: virtual void Load(sdf::ElementPtr _sdf);

      /// \brief Initialize the collision.
      public: virtual void Init();

      /// \brief Update the parameters using new sdf values.
      /// \param[in] _sdf SDF values to update from.
      public: virtual void UpdateParameters(sdf::ElementPtr _sdf);

      /// \brief Set the encapsulated collsion object.
      /// \param[in] _placeable True to make the object m.
      public: void SetCollision(bool _placeable);

      /// \brief Return whether this collision is movable.
      /// Example on an immovable object is a ray.
      /// \return True if the object is immovable.
      public: bool IsPlaceable() const;

      /// \brief Set the category bits, used during collision detection.
      /// \param[in] _bits The bits to set.
      public: virtual void SetCategoryBits(unsigned int _bits) = 0;

      /// \brief Set the collide bits, used during collision detection.
      /// \param[in] _bits The bits to set.
      public: virtual void SetCollideBits(unsigned int _bits) = 0;

      /// \brief Set the laser retro reflectiveness.
      /// \param[in] _retro The laser retro value.
      public: void SetLaserRetro(float _retro);

      /// \brief Get the laser retro reflectiveness
      /// \return The laser retro value.
      public: float GetLaserRetro() const;

      /// \brief Get the link this collision belongs to.
      /// \return The parent Link.
      public: LinkPtr GetLink() const;

      /// \brief Get the model this collision belongs to.
      /// \return The parent model.
      public: ModelPtr GetModel() const;

      /// \brief Get the bounding box for this collision.
      /// \return The bounding box.
      public: virtual math::Box GetBoundingBox() const = 0;

      /// \brief Get the shape type.
      /// \return The shape type.
      /// \sa EntityType
      public: unsigned int GetShapeType();

      /// \brief Set the shape for this collision.
      /// \param[in] _shape The shape for this collision object.
      public: void SetShape(ShapePtr _shape);

      /// \brief Get the collision shape.
      /// \return The collision shape.
      public: ShapePtr GetShape() const;

      /// \brief Turn contact recording on or off.
      /// \param[in] _enable True to enable collision contacts.
      public: void SetContactsEnabled(bool _enable);

      /// \brief Return true if contact recording is on
      /// \return True of contact are on.
      public: bool GetContactsEnabled() const;

      /// \brief Add an occurance of a contact to this collision.
      /// \param[in] _contact The contact which was detected by a collision
      /// engine.
      public: void AddContact(const Contact &_contact);

      /// \brief Get the linear velocity of the collision.
      /// \return The linear velocity relative to the parent model.
      public: virtual math::Vector3 GetRelativeLinearVel() const;

      /// \brief Get the linear velocity of the collision in the world
      /// frame.
      /// \return The linear velocity of the collision in the world frame.
      public: virtual math::Vector3 GetWorldLinearVel() const;

      /// \brief Get the angular velocity of the collision.
      /// \return The angular velocity of the collision.
      public: virtual math::Vector3 GetRelativeAngularVel() const;

      /// \brief Get the angular velocity of the collision in the world frame.
      /// \return The angular velocity of the collision in the world frame.
      public: virtual math::Vector3 GetWorldAngularVel() const;

      /// \brief Get the linear acceleration of the collision.
      /// \return The linear acceleration of the collision.
      public: virtual math::Vector3 GetRelativeLinearAccel() const;

      /// \brief Get the linear acceleration of the collision in the world
      /// frame.
      /// \return The linear acceleration of the collision in the world frame.
      public: virtual math::Vector3 GetWorldLinearAccel() const;

      /// \brief Get the angular acceleration of the collision.
      /// \return The angular acceleration of the collision.
      public: virtual math::Vector3 GetRelativeAngularAccel() const;

      /// \brief Get the angular acceleration of the collision in the
      /// world frame.
      /// \return The angular acceleration of the collision in the world frame.
      public: virtual math::Vector3 GetWorldAngularAccel() const;

      /// \brief Get the collision state.
      /// \return The collision state.
      public: CollisionState GetState();

      /// \brief Set the current collision state.
      /// \param[in] The collision state.
      public: void SetState(const CollisionState &_state);

      /// Deprecated.
      public: template<typename T>
              event::ConnectionPtr ConnectContact(T _subscriber)
              {return contact.Connect(_subscriber);}

      /// Deprecated.
      public: void DisconnectContact(event::ConnectionPtr &_conn)
              {contact.Disconnect(_conn);}

      /// \brief Fill a collision message.
      /// \param[out] _msg The message to fill with this collision's data.
      public: void FillMsg(msgs::Collision &_msg);

      /// \brief Update parameters from a message.
      /// \param[in] _msg Message to update from.
      public: void ProcessMsg(const msgs::Collision &_msg);

      /// \brief Get the surface parameters.
      /// \return The surface parameters.
      public: inline SurfaceParamsPtr GetSurface() const
              {return this->surface;}

      /// \brief Helper function used to create a collision visual message.
      /// \return Visual message for a collision.
      private: msgs::Visual CreateCollisionVisual();

      /// \brief The link this collision belongs to
      protected: LinkPtr link;

      /// \brief Flag for placeable.
      protected: bool placeable;

      /// \brief Pointer to physics::Shape.
      protected: ShapePtr shape;

      /// \brief True if contacts are enabled.
      private: bool contactsEnabled;

      /// \brief The contact event.
      private: event::EventT<void (const std::string &,
                                   const Contact &)> contact;

      /// \brief The surface parameters.
      private: SurfaceParamsPtr surface;

      /// \brief The laser retro value.
      private: float laserRetro;

      private: CollisionState state;
    };
    /// \}
  }
}
#endif
