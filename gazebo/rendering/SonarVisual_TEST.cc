/*
 * Copyright (C) 2014-2015 Open Source Robotics Foundation
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

#include <gtest/gtest.h>
#include "gazebo/math/Rand.hh"
#include "gazebo/rendering/RenderingIface.hh"
#include "gazebo/rendering/Scene.hh"
#include "gazebo/rendering/SonarVisual.hh"
#include "test/ServerFixture.hh"

using namespace gazebo;
class SonarVisual_TEST : public ServerFixture
{
};

/////////////////////////////////////////////////
TEST_F(SonarVisual_TEST, SonarVisualTest)
{
  Load("worlds/empty.world");

  gazebo::rendering::ScenePtr scene = gazebo::rendering::get_scene("default");

  if (!scene)
      scene = gazebo::rendering::create_scene("default", false);

  EXPECT_TRUE(scene != NULL);

  // test calling constructor and Load functions and make sure
  // there are no segfaults
  gazebo::rendering::VisualPtr sonarVis(
      new gazebo::rendering::SonarVisual(
      "world_GUIONLY_sonar_vis", scene->GetWorldVisual(), ""));
  sonarVis->Load();
}

/////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
