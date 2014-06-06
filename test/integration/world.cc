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
#include "ServerFixture.hh"
#include "gazebo/physics/physics.hh"

class WorldTest : public ServerFixture
{
};
/*
/////////////////////////////////////////////////
TEST_F(WorldTest, ClearEmptyWorld)
{
  Load("worlds/blank.world");
  physics::WorldPtr world = physics::get_world("default");
  ASSERT_TRUE(world);

  EXPECT_EQ(world->GetModelCount(), 0u);

  world->Clear();

  // Wait some bit of time since World::Clear is not immediate.
  for (unsigned int i = 0; i < 20; ++i)
    common::Time::MSleep(500);

  EXPECT_EQ(world->GetModelCount(), 0u);

  // Now spawn something, and the model count should increase
  SpawnSphere("sphere", math::Vector3(0, 0, 1), math::Vector3(0, 0, 0));
  EXPECT_EQ(world->GetModelCount(), 1u);
}

/////////////////////////////////////////////////
TEST_F(WorldTest, Clear)
{
  Load("worlds/pioneer2dx.world");
  physics::WorldPtr world = physics::get_world("default");
  ASSERT_TRUE(world);

  EXPECT_EQ(world->GetModelCount(), 2u);

  world->Clear();
  while (world->GetModelCount() > 0u)
    common::Time::MSleep(1000);

  EXPECT_EQ(world->GetModelCount(), 0u);

  SpawnSphere("sphere", math::Vector3(0, 0, 1), math::Vector3(0, 0, 0));

  EXPECT_EQ(world->GetModelCount(), 1u);
}

/////////////////////////////////////////////////
TEST_F(WorldTest, ModifyLight)
{
  Load("worlds/empty.world");
  physics::WorldPtr world = physics::get_world("default");
  ASSERT_TRUE(world);
  msgs::Scene sceneMsg = world->GetSceneMsg();

  // Make sure there is only one light, and it is named "sun"
  EXPECT_EQ(sceneMsg.light_size(), 1);
  EXPECT_STREQ(sceneMsg.light(0).name().c_str(), "sun");

  transport::PublisherPtr lightPub = this->node->Advertise<msgs::Light>(
        "~/light");

  // Set the light to be green
  msgs::Light lightMsg;
  lightMsg.set_name("sun");
  msgs::Set(lightMsg.mutable_diffuse(), common::Color(0, 1, 0));
  lightPub->Publish(lightMsg);

  // Allow the world time to process the messages
  world->Step(10);

  // Get the new scene, and make sure the color of the "sun" light is
  // correct.
  msgs::Scene sceneMsg2 = world->GetSceneMsg();
  EXPECT_EQ(sceneMsg2.light_size(), 1);
  EXPECT_STREQ(sceneMsg2.light(0).name().c_str(), "sun");
  EXPECT_EQ(sceneMsg2.light(0).diffuse().r(), 0);
  EXPECT_EQ(sceneMsg2.light(0).diffuse().g(), 1);
  EXPECT_EQ(sceneMsg2.light(0).diffuse().b(), 0);

  // Add a new light
  lightMsg.set_name("test_light");
  msgs::Set(lightMsg.mutable_diffuse(), common::Color(1, 0, 1));
  lightMsg.set_type(msgs::Light::POINT);
  lightPub->Publish(lightMsg);

  // Allow the world time to process the messages
  world->Step(10);

  sceneMsg2 = world->GetSceneMsg();
  EXPECT_EQ(sceneMsg2.light_size(), 2);
  EXPECT_STREQ(sceneMsg2.light(1).name().c_str(), "test_light");
  EXPECT_EQ(sceneMsg2.light(1).diffuse().r(), 1);
  EXPECT_EQ(sceneMsg2.light(1).diffuse().g(), 0);
  EXPECT_EQ(sceneMsg2.light(1).diffuse().b(), 1);
  EXPECT_EQ(sceneMsg2.light(1).type(), msgs::Light::POINT);
}
*/

/////////////////////////////////////////////////
TEST_F(WorldTest, RemoveModelPaused)
{
  Load("worlds/shapes.world", true);
  physics::WorldPtr world = physics::get_world("default");
  ASSERT_TRUE(world);

  physics::ModelPtr sphereModel = world->GetModel("sphere");
  physics::ModelPtr boxModel = world->GetModel("box");

  EXPECT_TRUE(sphereModel);
  EXPECT_TRUE(boxModel);

  world->RemoveModel(sphereModel);
  world->RemoveModel("box");

  sphereModel = world->GetModel("sphere");
  boxModel = world->GetModel("box");

  EXPECT_FALSE(sphereModel);
  EXPECT_FALSE(boxModel);
}

/////////////////////////////////////////////////
TEST_F(WorldTest, RemoveModelUnPaused)
{
  Load("worlds/shapes.world");
  physics::WorldPtr world = physics::get_world("default");
  ASSERT_TRUE(world);

  physics::ModelPtr sphereModel = world->GetModel("sphere");
  physics::ModelPtr boxModel = world->GetModel("box");

  EXPECT_TRUE(sphereModel);
  EXPECT_TRUE(boxModel);

  world->RemoveModel(sphereModel);
  world->RemoveModel("box");

  sphereModel = world->GetModel("sphere");
  boxModel = world->GetModel("box");

  EXPECT_FALSE(sphereModel);
  EXPECT_FALSE(boxModel);
}

/////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
