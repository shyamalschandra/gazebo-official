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

#ifndef _DARTPHYSICS_HH_
#define _DARTPHYSICS_HH_

#include <string>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

#include "gazebo/physics/PhysicsEngine.hh"
#include "gazebo/physics/Collision.hh"
#include "gazebo/physics/Shape.hh"

#include "gazebo/physics/dart/dart_inc.h"
#include "gazebo/physics/dart/DARTTypes.hh"

namespace gazebo
{
  namespace physics
  {
    //class Entity;

    /// \ingroup gazebo_physics
    /// \addtogroup gazebo_physics_dart DART Physics
    /// \{

    /// \brief DART physics engine
    class DARTPhysics : public PhysicsEngine
    {
      /// \brief Constructor
      public: DARTPhysics(WorldPtr _world);
 
      /// \brief Destructor
      public: virtual ~DARTPhysics();
       
      /// \brief Load the DART engine
      public: virtual void Load(sdf::ElementPtr _sdf);
 
      /// \brief Initialize the DART engine
      public: virtual void Init();

      /// \brief Finilize the DART engine.
      public: virtual void Fini();
       
      /// \brief Rest the DART engine.
      public: virtual void Reset();
 
      // Documentation inherited
      public: virtual void InitForThread();

      // Documentation inherited
      public: virtual void UpdateCollision();

      // Documentation inherited
      public: virtual void UpdatePhysics();

      // Documentation inherited
      public: virtual std::string GetType() const;

      // Documentation inherited
      public: virtual void SetSeed(uint32_t _seed);

      //      /// \brief Set the simulation step time
      //      public: virtual void SetStepTime(double _value);

      //      /// \brief Get the simulation step time
      //      public: virtual double GetStepTime();

      // Documentation inherited
      public: virtual ModelPtr CreateModel(BasePtr _parent);
      
      // Documentation inherited
      public: virtual LinkPtr CreateLink(ModelPtr _parent);

      // Documentation inherited
      public: virtual CollisionPtr CreateCollision(const std::string& _type,
                                                   LinkPtr _body);

      // Documentation inherited
      public: virtual JointPtr CreateJoint(const std::string& _type,
                                           ModelPtr _parent);
      
      // Documentation inherited
      public: virtual ShapePtr CreateShape(const std::string& _shapeType,
                                           CollisionPtr _collision);

      // Documentation inherited
      public: virtual void SetGravity(const gazebo::math::Vector3& gravity);

      // Documentation inherited
      public: virtual void DebugPrint() const;

      /// \brief Store the value of the stepTime parameter to improve efficiency
      //private: double stepTimeDouble;

      // Documentation inherited
      protected: virtual void OnRequest(ConstRequestPtr &_msg);

      // Documentation inherited
      protected: virtual void OnPhysicsMsg(ConstPhysicsPtr &_msg);

      // Documentation inherited
      void virtual SetMaxStepSize(double _stepSize);

      /// \brief
      public: simulation::World* GetDARTWorld() {return dartWorld;}

      /// \brief 
      private: simulation::World* dartWorld;
      
    };

  /// \}
  }
}
#endif
