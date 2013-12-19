/*
 * Copyright 2013 Open Source Robotics Foundation
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

using namespace gazebo;
class GzJoint : public ServerFixture
{
};

/////////////////////////////////////////////////
/// \brief Test application of force to a joint ('gz joint -f')
TEST_F(GzJoint, Force)
{
  Load("worlds/single_revolute_test.world");

  // Get a pointer to the world
  physics::WorldPtr world = physics::get_world("default");
  ASSERT_TRUE(world);

  // Get a pointer to the model
  physics::ModelPtr model = world->GetModel("model");
  ASSERT_TRUE(model);

  // Get a pointer to the joint
  physics::JointPtr joint = model->GetJoint("joint");
  ASSERT_TRUE(joint);

  // Make sure the sensor is at the correct initial pose
  EXPECT_NEAR(joint->GetAngle(0).Radian(), 0.0, 1e-3);

  world->SetPaused(true);

  // Tell the camera to follow the box. The camera should move toward the
  // box.
  custom_exec("gz joint -w default -m model -j joint -f 10.0");

  world->Step(100);

  // Make sure the joint has moved.
  EXPECT_GT(joint->GetAngle(0).Radian(), 0.1);
}

/////////////////////////////////////////////////
/// \brief Test application of position PID to a joint ('gz joint --pos-*')
TEST_F(GzJoint, PositionPID)
{
  Load("worlds/single_revolute_test.world");

  // Get a pointer to the world
  physics::WorldPtr world = physics::get_world("default");
  ASSERT_TRUE(world);

  // Get a pointer to the model
  physics::ModelPtr model = world->GetModel("model");
  ASSERT_TRUE(model);

  // Get a pointer to the joint
  physics::JointPtr joint = model->GetJoint("joint");
  ASSERT_TRUE(joint);

  // Make sure the sensor is at the correct initial pose
  EXPECT_NEAR(joint->GetAngle(0).Radian(), 0.0, 1e-3);

  world->SetPaused(true);

  // Tell the joint to hold a position using a PID controller.
  custom_exec("gz joint -w default -m model -j joint --pos-t 3.1415 "
      "--pos-p 0.3 --pos-i 0.0 --pos-d 1.8");

  world->Step(1000);

  // Make sure the sensor is at the correct initial pose
  EXPECT_NEAR(joint->GetAngle(0).Radian(), 3.1415, 0.2);
}

/////////////////////////////////////////////////
/// \brief Test application of velocity PID to a joint ('gz joint --vel-*')
TEST_F(GzJoint, VelocityPID)
{
  Load("worlds/single_revolute_test.world");

  // Get a pointer to the world
  physics::WorldPtr world = physics::get_world("default");
  ASSERT_TRUE(world);

  // Get a pointer to the model
  physics::ModelPtr model = world->GetModel("model");
  ASSERT_TRUE(model);

  // Get a pointer to the joint
  physics::JointPtr joint = model->GetJoint("joint");
  ASSERT_TRUE(joint);

  // Make sure the sensor is at the correct initial pose
  EXPECT_NEAR(joint->GetAngle(0).Radian(), 0.0, 1e-3);

  world->SetPaused(true);

  // Tell the joint to hold a velocity using a PID controller.
  custom_exec("gz joint -w default -m model -j joint --vel-t 0.5 "
      "--vel-p 100.0 --vel-i 10.0 --vel-d 0.01");

  world->Step(800);

  // Make sure the sensor is at the correct initial pose
  EXPECT_NEAR(joint->GetVelocity(0), 0.5, 0.1);
}

/////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
