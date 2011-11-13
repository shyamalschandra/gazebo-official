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
/* Desc: Base class for all models
 * Author: Nathan Koenig and Andrew Howard
 * Date: 8 May 2003
 */

#ifndef MODEL_HH
#define MODEL_HH

#include <string>

#include "common/CommonTypes.hh"
#include "physics/PhysicsTypes.hh"

#include "physics/Entity.hh"

namespace boost
{
  class recursive_mutex;
}

namespace gazebo
{
  namespace physics
  {
    /// \addtogroup gazebo_physics
    /// \{

    /// \brief A model
    class Model : public Entity
    {
      /// \brief Constructor
      /// \param parent Parent object
      public: Model(BasePtr parent);
    
      /// \brief Destructor
      public: virtual ~Model();
    
      /// \brief Load the model
      /// \param _sdf SDF parameters
      public: void Load( sdf::ElementPtr &_sdf );
    
      /// \brief Initialize the model
      public: virtual void Init();
    
      /// \brief Update the model
      public: void Update();

      /// \brief Finalize the model
      public: virtual void Fini();

      /// \brief update the parameters using new sdf values
      public: virtual void UpdateParameters( sdf::ElementPtr &_sdf );

      /// \brief Get the SDF values for the model
      public: virtual const sdf::ElementPtr &GetSDF();

      /// \brief Remove a child
      /// \param child Remove a child entity
      public: virtual void RemoveChild(EntityPtr child);
  
      /// \brief Reset the model
      public: void Reset();
    
      /// \brief Set the linear velocity of the model
      /// \param vel The new linear velocity
      public: void SetLinearVel( const math::Vector3 &vel );
  
      /// \brief Set the angular velocity of the model
      /// \param vel The new angular velocity
      public: void SetAngularVel( const math::Vector3 &vel );
  
      /// \brief Set the linear acceleration of the model
      /// \param vel The new linear acceleration
      public: void SetLinearAccel( const math::Vector3 &vel );
  
      /// \brief Set the angular acceleration of the model
      /// \param vel The new angular acceleration
      public: void SetAngularAccel( const math::Vector3 &vel );
  
      /// \brief Get the linear velocity of the entity
      /// \return math::Vector3, set to 0,0,0 if the model has no body
      public: virtual math::Vector3 GetRelativeLinearVel() const;
  
      /// \brief Get the linear velocity of the entity in the world frame
      /// \return math::Vector3, set to 0,0,0 if the model has no body
      public: virtual math::Vector3 GetWorldLinearVel() const;
  
      /// \brief Get the angular velocity of the entity
      /// \return math::Vector3, set to 0,0,0 if the model has no body
      public: virtual math::Vector3 GetRelativeAngularVel() const;
  
      /// \brief Get the angular velocity of the entity in the world frame
      /// \return math::Vector3, set to 0,0,0 if the model has no body
      public: virtual math::Vector3 GetWorldAngularVel() const;
  
      /// \brief Get the linear acceleration of the entity
      /// \return math::Vector3, set to 0,0,0 if the model has no body
      public: virtual math::Vector3 GetRelativeLinearAccel() const;
  
      /// \brief Get the linear acceleration of the entity in the world frame
      /// \return math::Vector3, set to 0,0,0 if the model has no body
      public: virtual math::Vector3 GetWorldLinearAccel() const;
  
      /// \brief Get the angular acceleration of the entity 
      /// \return math::Vector3, set to 0,0,0 if the model has no body
      public: virtual math::Vector3 GetRelativeAngularAccel() const;
  
      /// \brief Get the angular acceleration of the entity in the world frame
      /// \return math::Vector3, set to 0,0,0 if the model has no body
      public: virtual math::Vector3 GetWorldAngularAccel() const;
  
      /// \brief Get the size of the bounding box
      /// \return The bounding box
      public: virtual math::Box GetBoundingBox() const;
    
      /// \brief Get the number of joints
      /// \return Get the number of joints
      public: unsigned int GetJointCount() const;
  
      /// \brief Get a joing by index
      /// \param index Index of the joint
      /// \return A pointer to the joint
      public: JointPtr GetJoint( unsigned int index ) const;
  
      /// \brief Get a joint
      /// \param name The name of the joint, specified in the world file
      /// \return Pointer to the joint
      public: JointPtr GetJoint(const std::string &name);
 
      /// \brief Get a body by name
      /// \return Pointer to the body
      public: LinkPtr GetLink(const std::string &name="canonical") const;
 
      /// \brief Set the gravity mode of the model
      public: void SetGravityMode( const bool &v );
  
      /// \brief Set the collide mode of the model
      /// \param m The collision mode
      public: void SetCollideMode( const std::string &m );
  
      /// \brief Set the laser retro reflectiveness of the model
      /// \param retro Retro reflectance value
      public: void SetLaserRetro( const float &retro );

      /// \brief Fill a model message
      /// \param _msg Message to fill
      public: void FillModelMsg( msgs::Model &_msg );

      /// \brief Set the positions of a set of joints
      public: void SetJointPositions(
                  const std::map<std::string, double> &_jointPositions);

      public: void SetJointAnimation(
                  const std::map<std::string, common::NumericAnimationPtr> anims);

      private: void RotateBodyAndChildren(LinkPtr _body1, 
                   const math::Vector3 &_anchor, const math::Vector3 &_axis, 
                   double _dangle, bool _updateChildren);

      private: void GetAllChildrenBodies(std::vector<LinkPtr> &_bodies, 
                   const LinkPtr &_body);

      private: void GetAllParentBodies(std::vector<LinkPtr> &_bodies, 
                   const LinkPtr &_body, const LinkPtr &_origParentBody);

      private: bool InBodies(const LinkPtr &_body, 
                             const std::vector<LinkPtr> &_bodies);

      /// \brief Load a joint helper function
      /// \param _sdf SDF parameter
      private: void LoadJoint( sdf::ElementPtr &_sdf );
   
      /// \brief Load a plugin helper function 
      /// \param _sdf SDF parameter
      private: void LoadPlugin( sdf::ElementPtr &_sdf );

      private: LinkPtr canonicalLink;

      private: Joint_V joints;

      private: std::vector<ModelPluginPtr> plugins;

      private: transport::PublisherPtr jointPub;
      private: std::map<std::string, common::NumericAnimationPtr> jointAnimations;
      private: common::Time prevAnimationTime;

      private: boost::recursive_mutex *updateMutex;
    };
    /// \}
  }
}
#endif
