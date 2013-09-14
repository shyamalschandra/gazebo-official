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

#include "ServerFixture.hh"
#include "gazebo/physics/physics.hh"
#include "gazebo/physics/Gripper.hh"
#include "gazebo/physics/World.hh"
#include "gazebo/physics/Model.hh"
#include "gazebo/physics/Joint.hh"

class GripperTest : public ServerFixture
{
};

/////////////////////////////////////////////////
// \brief Test to make sure the gripper forms a joint when grasping an object
TEST_F(GripperTest, Grasp)
{
  Load("worlds/gripper.world");
  physics::WorldPtr world = physics::get_world("default");
  ASSERT_TRUE(world);

  physics::ModelPtr model = world->GetModel("simple_gripper");
  ASSERT_TRUE(model);

  physics::JointPtr leftJoint = model->GetJoint("palm_left_finger");
  ASSERT_TRUE(leftJoint);

  physics::JointPtr rightJoint = model->GetJoint("palm_right_finger");
  ASSERT_TRUE(rightJoint);

  physics::GripperPtr gripper = model->GetGripper(0);
  ASSERT_TRUE(gripper);

  // The gripper should not be attached to anything
  EXPECT_FALSE(gripper->IsAttached());

  // Close the gripper.
  leftJoint->SetForce(0, -0.5);
  rightJoint->SetForce(0, 0.5);

  transport::PublisherPtr jointPub = this->node->Advertise<msgs::JointCmd>(
        "~/simple_gripper/joint_cmd");

  msgs::JointCmd msg;
  msg.set_name("simple_gripper::palm_right_finger");
  msg.set_force(0.6);
  jointPub->Publish(msg);

  msg.set_name("simple_gripper::palm_left_finger");
  msg.set_force(-0.6);
  jointPub->Publish(msg);

  int i = 0;
  while (!gripper->IsAttached() && i < 100)
  {
    common::Time::MSleep(200);
    ++i;
  }
  EXPECT_LT(i, 100);

  // The gripper should be attached.
  EXPECT_TRUE(gripper->IsAttached());

  // Open the gripper.
  msg.set_name("simple_gripper::palm_right_finger");
  msg.set_force(-0.6);
  jointPub->Publish(msg);

  msg.set_name("simple_gripper::palm_left_finger");
  msg.set_force(0.6);
  jointPub->Publish(msg);

  i = 0;
  while (gripper->IsAttached() && i < 100)
  {
    common::Time::MSleep(200);
    ++i;
  }
  EXPECT_LT(i, 100);

  // The gripper should release the box.
  EXPECT_FALSE(gripper->IsAttached());
}

/////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
