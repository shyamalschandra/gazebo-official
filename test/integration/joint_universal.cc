/*
 * Copyright (C) 2014 Open Source Robotics Foundation
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
#include <string.h>

#include "gazebo/physics/physics.hh"
#include "ServerFixture.hh"
#include "helper_physics_generator.hh"

using namespace gazebo;

const double g_tolerance = 1e-4;

class JointTestUniversal : public ServerFixture,
                        public testing::WithParamInterface<const char*>
{
  /// \brief Test setting and enforcing joint limits.
  /// \param[in] _physicsEngine Type of physics engine to use.
  public: void Limits(const std::string &_physicsEngine);

  /// \brief Test universal joint implementation with SetWorldPose.
  /// Set links world poses then check joint angles.
  /// \param[in] _physicsEngine Type of physics engine to use.
  public: void UniversalJointSWP(const std::string &_physicsEngine);

  /// \brief Test universal joint implementation with forces.
  /// Apply force to universal joint links, check position and/velocity.
  /// \param[in] _physicsEngine Type of physics engine to use.
  public: void UniversalJointForce(const std::string &_physicsEngine);
};

/////////////////////////////////////////////////
void JointTestUniversal::Limits(const std::string &_physicsEngine)
{
  if (_physicsEngine == "dart")
  {
    gzerr << "DART Universal Joint is not yet working.  See issue #1011.\n";
    return;
  }

  // Load our universal joint test world
  Load("worlds/universal_joint_test.world", true, _physicsEngine);

  // Get a pointer to the world, make sure world loads
  physics::WorldPtr world = physics::get_world("default");
  ASSERT_TRUE(world != NULL);

  // Verify physics engine type
  physics::PhysicsEnginePtr physics = world->GetPhysicsEngine();
  ASSERT_TRUE(physics != NULL);
  EXPECT_EQ(physics->GetType(), _physicsEngine);

  // get model and joints
  physics::ModelPtr model = world->GetModel("model_1");
  ASSERT_TRUE(model);
  physics::JointPtr jointUpper = model->GetJoint("joint_00");
  physics::JointPtr jointLower = model->GetJoint("joint_01");
  ASSERT_TRUE(jointUpper);
  ASSERT_TRUE(jointLower);

  // freeze upper joint
  jointUpper->SetHighStop(0, 1e-6);
  jointUpper->SetHighStop(1, 1e-6);
  jointUpper->SetLowStop(0, -1e-6);
  jointUpper->SetLowStop(1, -1e-6);

  // set asymmetric limits on lower joints
  double hi0 =  0.4;
  double hi1 =  0.2;
  double lo0 = -0.1;
  double lo1 = -0.3;
  jointLower->SetHighStop(0, hi0);
  jointLower->SetHighStop(1, hi1);
  jointLower->SetLowStop(0, lo0);
  jointLower->SetLowStop(1, lo1);
  EXPECT_NEAR(hi0, jointLower->GetHighStop(0).Radian(), g_tolerance);
  EXPECT_NEAR(hi1, jointLower->GetHighStop(1).Radian(), g_tolerance);
  EXPECT_NEAR(lo0, jointLower->GetLowStop(0).Radian(), g_tolerance);
  EXPECT_NEAR(lo1, jointLower->GetLowStop(1).Radian(), g_tolerance);

  for (int i = 0; i < 4; ++i)
  {
    // toggle signs for gx, gy
    //     gx gy
    // i=0: +  +
    // i=1: +  -
    // i=2: -  +
    // i=3: -  -
    double gravityMag = 5.0;
    double gx = pow(-1, i / 2) * gravityMag;
    double gy = pow(-1, i % 2) * gravityMag;

    // Set gravity to push horizontally
    physics->SetGravity(math::Vector3(gx, gy, 0));
    world->Step(1000);

    // jointLower: axis[0] = {1, 0, 0}
    // jointLower: axis[1] = {0, 1, 0}
    // offset from anchor to c.g. is r = {0, 0, -L}
    // gravity moment r x g

    // a negative gy causes negative rotation about axis[0]
    double des0 = (gy < 0) ? lo0 : hi0;

    // a positive gx causes negative rotation about axis[1]
    double des1 = (gx > 0) ? lo1 : hi1;

    gzdbg << "Setting gravity "
          << "gx " << gx << ' '
          << "gy " << gy << ' '
          << std::endl;
    EXPECT_NEAR(des0, jointLower->GetAngle(0).Radian(), 1e-2);
    EXPECT_NEAR(des1, jointLower->GetAngle(1).Radian(), 1e-2);
  }
}

/////////////////////////////////////////////////
void JointTestUniversal::UniversalJointSWP(const std::string &_physicsEngine)
{
  if (_physicsEngine == "dart")
  {
    gzerr << "DART Universal Joint is not yet working.  See issue #1011.\n";
    return;
  }

  // Load our universal joint test world
  Load("worlds/universal_joint_test.world", true, _physicsEngine);

  // Get a pointer to the world, make sure world loads
  physics::WorldPtr world = physics::get_world("default");
  ASSERT_TRUE(world != NULL);

  // Verify physics engine type
  physics::PhysicsEnginePtr physics = world->GetPhysicsEngine();
  ASSERT_TRUE(physics != NULL);
  EXPECT_EQ(physics->GetType(), _physicsEngine);

  physics->SetGravity(math::Vector3(0, 0, 0));

  // simulate 1 step
  world->Step(1);
  double t = world->GetSimTime().Double();

  // get time step size
  double dt = world->GetPhysicsEngine()->GetMaxStepSize();
  EXPECT_GT(dt, 0);
  gzlog << "dt : " << dt << "\n";

  // verify that time moves forward
  EXPECT_DOUBLE_EQ(t, dt);
  gzlog << "t after one step : " << t << "\n";

  // get model, joint and links
  physics::ModelPtr model_1 = world->GetModel("model_1");
  physics::LinkPtr link_00 = model_1->GetLink("link_00");
  physics::LinkPtr link_01 = model_1->GetLink("link_01");
  physics::JointPtr joint_00 = model_1->GetJoint("joint_00");
  physics::JointPtr joint_01 = model_1->GetJoint("joint_01");

  // both initial angles should be zero
  EXPECT_EQ(joint_00->GetAngle(0), 0);
  EXPECT_EQ(joint_00->GetAngle(1), 0);

  // move child link to it's initial location
  link_00->SetWorldPose(math::Pose(0, 0, 2, 0, 0, 0));
  EXPECT_EQ(joint_00->GetAngle(0), 0);
  EXPECT_EQ(joint_00->GetAngle(1), 0);
  EXPECT_EQ(joint_00->GetGlobalAxis(0), math::Vector3(1, 0, 0));
  EXPECT_EQ(joint_00->GetGlobalAxis(1), math::Vector3(0, 1, 0));
  gzdbg << "joint angles [" << joint_00->GetAngle(0)
        << ", " << joint_00->GetAngle(1)
        << "] axis1 [" << joint_00->GetGlobalAxis(0)
        << "] axis2 [" << joint_00->GetGlobalAxis(1)
        << "]\n";

  // move child link 45deg about x
  link_00->SetWorldPose(math::Pose(0, 0, 2, 0.25*M_PI, 0, 0));
  EXPECT_EQ(joint_00->GetAngle(0), 0.25*M_PI);
  EXPECT_EQ(joint_00->GetAngle(1), 0);
  EXPECT_EQ(joint_00->GetGlobalAxis(0), math::Vector3(1, 0, 0));
  EXPECT_EQ(joint_00->GetGlobalAxis(1),
    math::Vector3(0, cos(0.25*M_PI), sin(0.25*M_PI)));
  gzdbg << "joint angles [" << joint_00->GetAngle(0)
        << ", " << joint_00->GetAngle(1)
        << "] axis1 [" << joint_00->GetGlobalAxis(0)
        << "] axis2 [" << joint_00->GetGlobalAxis(1)
        << "]\n";

  // move child link 45deg about y
  link_00->SetWorldPose(math::Pose(0, 0, 2, 0, 0.25*M_PI, 0));
  EXPECT_EQ(joint_00->GetAngle(0), 0);
  EXPECT_EQ(joint_00->GetAngle(1), 0.25*M_PI);
  EXPECT_EQ(joint_00->GetGlobalAxis(0), math::Vector3(1, 0, 0));
  EXPECT_EQ(joint_00->GetGlobalAxis(1), math::Vector3(0, 1, 0));
  gzdbg << "joint angles [" << joint_00->GetAngle(0)
        << ", " << joint_00->GetAngle(1)
        << "] axis1 [" << joint_00->GetGlobalAxis(0)
        << "] axis2 [" << joint_00->GetGlobalAxis(1)
        << "]\n";

  // move child link 90deg about both x and "rotated y axis" (z)
  link_00->SetWorldPose(math::Pose(0, 0, 2, 0.5*M_PI, 0, 0.5*M_PI));
  EXPECT_EQ(joint_00->GetAngle(1), 0.5*M_PI);
  EXPECT_EQ(joint_00->GetGlobalAxis(0), math::Vector3(1, 0, 0));
  EXPECT_EQ(joint_00->GetGlobalAxis(1),
    math::Vector3(0, cos(0.5*M_PI), sin(0.5*M_PI)));

  if (_physicsEngine == "bullet")
  {
    // Bullet is off by one step. See issue 1081
    world->Step(1);
    EXPECT_EQ(joint_00->GetAngle(0), 0.5*M_PI);
  }
  else
    EXPECT_EQ(joint_00->GetAngle(0), 0.5*M_PI);

  gzdbg << "joint angles [" << joint_00->GetAngle(0)
        << ", " << joint_00->GetAngle(1)
        << "] axis1 [" << joint_00->GetGlobalAxis(0)
        << "] axis2 [" << joint_00->GetGlobalAxis(1)
        << "]\n";
}

//////////////////////////////////////////////////
void JointTestUniversal::UniversalJointForce(const std::string &_physicsEngine)
{
  if (_physicsEngine == "dart")
  {
    gzerr << "DART Universal Joint is not yet working.  See issue #1011.\n";
    return;
  }

  // Load our universal joint test world
  Load("worlds/universal_joint_test.world", true, _physicsEngine);

  // Get a pointer to the world, make sure world loads
  physics::WorldPtr world = physics::get_world("default");
  ASSERT_TRUE(world != NULL);

  // Verify physics engine type
  physics::PhysicsEnginePtr physics = world->GetPhysicsEngine();
  ASSERT_TRUE(physics != NULL);
  EXPECT_EQ(physics->GetType(), _physicsEngine);

  physics->SetGravity(math::Vector3(0, 0, 0));

  // simulate 1 step
  world->Step(1);
  double t = world->GetSimTime().Double();

  // get time step size
  double dt = world->GetPhysicsEngine()->GetMaxStepSize();
  EXPECT_GT(dt, 0);
  gzlog << "dt : " << dt << "\n";

  // verify that time moves forward
  EXPECT_DOUBLE_EQ(t, dt);
  gzlog << "t after one step : " << t << "\n";

  // get model, joints and get links
  physics::ModelPtr model_1 = world->GetModel("model_1");
  physics::LinkPtr link_00 = model_1->GetLink("link_00");
  physics::LinkPtr link_01 = model_1->GetLink("link_01");
  physics::JointPtr joint_00 = model_1->GetJoint("joint_00");
  physics::JointPtr joint_01 = model_1->GetJoint("joint_01");

  // both initial angles should be zero
  EXPECT_EQ(joint_00->GetAngle(0), 0);
  EXPECT_EQ(joint_00->GetAngle(1), 0);

  // set new upper limit for joint_00
  joint_00->SetHighStop(0, 0.3);
  // push joint_00 till it hits new upper limit
  while(joint_00->GetAngle(0) < 0.3)
  {
    joint_00->SetForce(0, 0.1);
    world->Step(1);
    // check link pose
    double angle_00_angular = joint_00->GetAngle(0).Radian();
    EXPECT_EQ(link_00->GetWorldPose(),
      math::Pose(0, 0, 2, angle_00_angular, 0, 0));
  }
  // lock joint at this location by setting lower limit here too
  joint_00->SetLowStop(0, 0.3);

  // set joint_01 upper limit to 1.0
  joint_01->SetHighStop(0, 1.0);
  // push joint_01 until limit is reached
  while(joint_01->GetAngle(0) < 1.0)
  {
    joint_01->SetForce(0, 0.1);
    world->Step(1);

    // check link pose
    math::Pose pose_00 = link_00->GetWorldPose();
    math::Pose pose_01 = link_01->GetWorldPose();
    double angle_00_angular = joint_00->GetAngle(0).Radian();
    double angle_00_linear = joint_00->GetAngle(1).Radian();
    double angle_01_angular = joint_01->GetAngle(0).Radian();
    double angle_01_linear = joint_01->GetAngle(1).Radian();

    EXPECT_EQ(pose_00, math::Pose(0, 0, 2, angle_00_angular, 0, 0));

    EXPECT_NEAR(pose_01.pos.x, angle_00_linear + angle_01_linear, 1e-8);

    EXPECT_NEAR(pose_01.pos.x, 0, 1e-8);
    EXPECT_NEAR(pose_01.rot.GetAsEuler().x,
      angle_00_angular + angle_01_angular, 1e-8);
  }

  // push joint_01 the other way until -1 is reached
  while(joint_01->GetAngle(0) > -1.0)
  {
    joint_01->SetForce(0, -0.1);
    world->Step(1);

    // check link pose
    math::Pose pose_00 = link_00->GetWorldPose();
    math::Pose pose_01 = link_01->GetWorldPose();
    double angle_00_angular = joint_00->GetAngle(0).Radian();
    double angle_00_linear = joint_00->GetAngle(1).Radian();
    double angle_01_angular = joint_01->GetAngle(0).Radian();
    double angle_01_linear = joint_01->GetAngle(1).Radian();

    EXPECT_EQ(pose_00, math::Pose(0, 0, 2, angle_00_angular, 0, 0));

    EXPECT_NEAR(pose_01.pos.x, angle_00_linear + angle_01_linear, 1e-8);

    EXPECT_NEAR(pose_01.pos.x, 0, 1e-8);
    EXPECT_NEAR(pose_01.rot.GetAsEuler().x,
      angle_00_angular + angle_01_angular, 1e-8);
  }
}

/////////////////////////////////////////////////
TEST_P(JointTestUniversal, Limits)
{
  Limits(GetParam());
}

/////////////////////////////////////////////////
TEST_P(JointTestUniversal, UniversalJointSWP)
{
  UniversalJointSWP(GetParam());
}

/////////////////////////////////////////////////
TEST_P(JointTestUniversal, UniversalJointForce)
{
  UniversalJointForce(GetParam());
}

INSTANTIATE_TEST_CASE_P(PhysicsEngines, JointTestUniversal,
                        PHYSICS_ENGINE_VALUES);

/////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
