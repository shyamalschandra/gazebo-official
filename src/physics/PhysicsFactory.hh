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
/*
 * Desc: Factory for creating physics engine
 * Author: Nate Koenig
 * Date: 21 May 2009
 * SVN info:$
 */

#ifndef PHYSICSFACTORY_HH
#define PHYSICSFACTORY_HH

#include <string>
#include <map>

namespace gazebo
{
	namespace physics
{
  
  // Forward declarations
  class PhysicsEngine;
  class World;
  
  // Prototype for sensor factory functions
  typedef PhysicsEngine* (*PhysicsFactoryFn) (World *world);
  
  /// \addtogroup gazebo_physics
  /// \brief The physics factory
  /// \{
  
  /// \brief The physics factory
  class PhysicsFactory
  {
    /// \brief Register everything
    public: static void RegisterAll();

    /// \brief Register a physics class.
    public: static void RegisterPhysicsEngine(std::string classname,
                                        PhysicsFactoryFn factoryfn);
  
    /// \brief Create a new instance of a physics engine.  
    public: static PhysicsEngine *NewPhysicsEngine(const std::string &classname, World *world);
  
    /// \brief A list of registered physics classes
    private: static std::map<std::string, PhysicsFactoryFn> engines;
  };
  
  
  /// \brief Static sensor registration macro
  ///
  /// Use this macro to register sensors with the server.
  /// @param name Physics type name, as it appears in the world file.
  /// @param classname C++ class name for the sensor.
#define GZ_REGISTER_PHYSICS_ENGINE(name, classname) \
PhysicsEngine *New##classname(World *world) \
{ \
  return new classname(world); \
} \
void Register##classname() \
{\
  PhysicsFactory::RegisterPhysicsEngine(name, New##classname);\
}
  
  /// \}
}

}
#endif
