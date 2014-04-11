/*
 * Copyright (C) 2012-2014 Open Source Robotics Foundation
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

#include <gazebo/gazebo.hh>
#include <gazebo/common/common.hh>
#include <gazebo/physics/physics.hh>

/////////////////////////////////////////////////
int main(int _argc, char **_argv)
{
  std::vector<std::string> v;
  v.push_back("arg1");
  v.push_back("arg2");

  // Initialize gazebo.
  gazebo::setupServer(v);
  //gazebo::setupServer(_argc, _argv);

  // Load a world
  gazebo::physics::WorldPtr world = gazebo::loadWorld("worlds/empty.world");

  // This is your custom main loop. In this example the main loop is just a
  // for loop with 2 iterations.
  for (unsigned int i = 0; i < 2; ++i)
  {
    // Run simulation for 100 steps.
    gazebo::runWorld(world, 100);
  }

  // Close everything.
  gazebo::shutdown();
}
