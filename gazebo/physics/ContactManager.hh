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

#ifndef _CONTACTMANAGER_HH_
#define _CONTACTMANAGER_HH_

#include <vector>
#include <string>

#include <boost/unordered/unordered_set.hpp>
#include <boost/unordered/unordered_map.hpp>

#include "gazebo/transport/TransportTypes.hh"

#include "gazebo/physics/PhysicsTypes.hh"
#include "gazebo/physics/Contact.hh"

namespace gazebo
{
  namespace physics
  {
    /// \brief A custom contact publisher created for each contact filter
    /// in the Contact Manager.
    class ContactPublisher
    {
      /// \brief Contact message publisher
      public: transport::PublisherPtr publisher;

      /// \brief Names of collisions monitored by contact manager for contacts.
      public: boost::unordered_set<std::string> collisions;

      /// \brief A list of contacts associated to the collisions.
      public: std::vector<Contact *> contacts;
    };

    /// \addtogroup gazebo_physics
    /// \{

    /// \class ContactManager ContactManager.hh physics/physics.hh
    /// \brief Aggregates all the contact information generated by the
    /// collision detection engine.
    class ContactManager
    {
      /// \brief Constructor.
      public: ContactManager();

      /// \brief Destructor.
      public: virtual ~ContactManager();

      /// \brief Initialize the ContactManager. This is required in order to
      /// publish contact messages via the ContactManager::PublishContacts
      /// method.
      /// \param[in] _world Pointer to the world that is initializing the
      /// contact manager.
      public: void Init(WorldPtr _world);

      /// \brief Add a new contact.
      ///
      /// Noramlly this is only used by a Physics/Collision engine when
      /// a new contact is generated. All other users should just make use
      /// of the accessor functions.
      ///
      /// If no one is listening, then the return value will be NULL.
      /// This is a signal to the Physics engine that it can skip the extra
      /// processing necessary to get back contact information.
      /// \return The new contact. The physics engine should populate the
      /// contact's parameters. NULL will be returned if there are no
      /// subscribers to the contact topic.
      public: Contact *NewContact(Collision *_collision1,
                                  Collision *_collision2,
                                  const common::Time &_time);

      /// \brief Return the number of valid contacts.
      public: unsigned int GetContactCount() const;

      /// \brief Get a single contact by index. The index must be between
      /// 0 and ContactManager::GetContactCount.
      /// \param[in] _index Index of the Contact to return.
      /// \return Pointer to a contact, NULL If index is invalid.
      public: Contact *GetContact(unsigned int _index) const;

      /// \brief Get all the contacts.
      ///
      /// The return vector may have invalid contacts. Only use contents of
      /// the vector between 0 and ContactManager::GetContactCount
      /// \return Vector of contact pointers.
      public: const std::vector<Contact *> &GetContacts() const;

      /// \brief Clear all stored contacts.
      public: void Clear();

      /// \brief Publish all contacts in a msgs::Contacts message.
      public: void PublishContacts();

      /// \brief Set the contact count to zero.
      public: void ResetCount();

      /// \brief Create a filter for contacts. A new publisher will be created
      /// that publishes contacts associated to the input collisions.
      /// param[in] _name Filter name.
      /// param[in] _collisions A list of collision names used for filtering.
      /// \return New topic where filtered messages will be published to.
      public: std::string CreateFilter(const std::string &_topic,
                  const std::vector<std::string> &_collisions);

      /// \brief Create a filter for contacts. A new publisher will be created
      /// that publishes contacts associated to the input collision.
      /// param[in] _name Filter name.
      /// param[in] _collision A collision name used for filtering.
      /// \return New topic where filtered messages will be published to.
      public: std::string CreateFilter(const std::string &_topic,
                  const std::string &_collision);

      private: std::vector<Contact*> contacts;

      private: unsigned int contactIndex;

      /// \brief Node for communication.
      private: transport::NodePtr node;

      /// \brief Contact publisher.
      private: transport::PublisherPtr contactPub;

      /// \brief Pointer to the world.
      private: WorldPtr world;

      /// \brief A list of custom publishers that publish filtered contact
      /// messages to the specified topic
      private: boost::unordered_map<std::string, ContactPublisher *>
          customContactPublishers;
    };
    /// \}
  }
}
#endif
