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

#include "test/ServerFixture.hh"
#include "gazebo/msgs/msgs.hh"

using namespace gazebo;

class PhysicsEngineTest : public ServerFixture
{
  public: void OnPhysicsMsgResponse(ConstResponsePtr &_msg);
  public: void PhysicsEngineParam(const std::string &_physicsEngine);
  public: void PhysicsEngineStability(const std::string &_physics);
  public: static msgs::Physics physicsPubMsg;
  public: static msgs::Physics physicsResponseMsg;
};

msgs::Physics PhysicsEngineTest::physicsPubMsg;
msgs::Physics PhysicsEngineTest::physicsResponseMsg;

/////////////////////////////////////////////////
void PhysicsEngineTest::OnPhysicsMsgResponse(ConstResponsePtr &_msg)
{
  if (_msg->type() == physicsPubMsg.GetTypeName())
    physicsResponseMsg.ParseFromString(_msg->serialized_data());
}

/////////////////////////////////////////////////
void PhysicsEngineTest::PhysicsEngineParam(const std::string &_physicsEngine)
{
  physicsPubMsg.Clear();
  physicsResponseMsg.Clear();

  Load("worlds/empty.world", false);
  physics::WorldPtr world = physics::get_world("default");
  ASSERT_TRUE(world != NULL);

  transport::NodePtr physicsNode;
  physicsNode = transport::NodePtr(new transport::Node());
  physicsNode->Init();

  transport::PublisherPtr physicsPub
       = physicsNode->Advertise<msgs::Physics>("~/physics");
  transport::PublisherPtr requestPub
      = physicsNode->Advertise<msgs::Request>("~/request");
  transport::SubscriberPtr responsePub = physicsNode->Subscribe("~/response",
      &PhysicsEngineTest::OnPhysicsMsgResponse, this);

  msgs::Physics_Type type;
  if (_physicsEngine == "ode")
    type = msgs::Physics::ODE;
  else if (_physicsEngine == "bullet")
    type = msgs::Physics::BULLET;
  else
    type = msgs::Physics::ODE;
  physicsPubMsg.set_type(type);
  physicsPubMsg.set_max_step_size(0.01);
  physicsPubMsg.set_real_time_update_rate(500);
  physicsPubMsg.set_real_time_factor(1.2);

  physicsPub->Publish(physicsPubMsg);

  msgs::Request *requestMsg = msgs::CreateRequest("physics_info", "");
  requestPub->Publish(*requestMsg);

  int waitCount = 0, maxWaitCount = 3000;
  while (physicsResponseMsg.ByteSize() == 0 && ++waitCount < maxWaitCount)
    common::Time::MSleep(10);

  ASSERT_LT(waitCount, maxWaitCount);

  EXPECT_DOUBLE_EQ(physicsResponseMsg.max_step_size(),
      physicsPubMsg.max_step_size());
  EXPECT_DOUBLE_EQ(physicsResponseMsg.real_time_update_rate(),
      physicsPubMsg.real_time_update_rate());
  EXPECT_DOUBLE_EQ(physicsResponseMsg.real_time_factor(),
      physicsPubMsg.real_time_factor());

  physicsNode->Fini();
}

TEST_F(PhysicsEngineTest, PhysicsEngineParamODE)
{
  PhysicsEngineParam("ode");
}

#ifdef HAVE_BULLET
TEST_F(PhysicsEngineTest, PhysicsEngineParamBullet)
{
  PhysicsEngineParam("bullet");
}
#endif  // HAVE_BULLET

/////////////////////////////////////////////////
// PhysicsEngineStability: simulate harmonic oscillators at various time steps
// and verify stability of system energy.
void PhysicsEngineTest::PhysicsEngineStability(const std::string &_physics)
{
  Load("worlds/blank.world", true, _physics);
  physics::WorldPtr world = physics::get_world("default");
  ASSERT_TRUE(world != NULL);

  // Verify physics engine type
  physics::PhysicsEnginePtr physics = world->GetPhysicsEngine();
  ASSERT_TRUE(physics != NULL);
  EXPECT_EQ(physics->GetType(), _physics);

  // Turn off gravity
  physics->SetGravity(math::Vector3::Zero);

  // Spawn a box up in the air
  math::Pose pose0(2, 2, 2, 0, 0, 0);
  std::string name = "test_box";
  SpawnBox(name, math::Vector3(1, 1, 1), pose0.pos, pose0.rot.GetAsEuler());
  physics::ModelPtr model = world->GetModel(name);
  ASSERT_TRUE(model != NULL);
  physics::LinkPtr link = model->GetLink();
  ASSERT_TRUE(link != NULL);
  double mass = link->GetInertial()->GetMass();

  // Make a PID controller for translational position
  double fMax = 1e10;
  math::Vector3 offset(1, 1, 1);
  offset *= 0.001;
  math::Pose poseDes(pose0);
  poseDes.pos = offset + poseDes.pos;
  math::Vector3 controllerHz(300, 310, 320);
  math::Vector3 gainP;
  gainP = controllerHz * 2.0 * M_PI;
  gainP = gainP*gainP;
  gzerr << "gainP " << gainP << '\n';
  // math::Vector3 gainP(100.0, 1000.0, 10000.0);
  common::PID pidX = common::PID(gainP.x, 0.0, 0.0, 0.0, 0.0, fMax, -fMax);
  common::PID pidY = common::PID(gainP.y, 0.0, 0.0, 0.0, 0.0, fMax, -fMax);
  common::PID pidZ = common::PID(gainP.z, 0.0, 0.0, 0.0, 0.0, fMax, -fMax);
  pidX.SetCmd(poseDes.pos.x);
  pidY.SetCmd(poseDes.pos.y);
  pidZ.SetCmd(poseDes.pos.z);

  math::Vector3 error0 = pose0.pos - poseDes.pos;
  math::Vector3 energy0 = 1/2 * gainP * error0 * error0;


  unsigned int steps = 200;
  unsigned int i;
  math::Vector3 error;
  double timeLast = world->GetSimTime().Double();
  double timeNow;
  math::Vector3 force, energy, velocity;
  for(i = 0; i < steps; ++i)
  {
    world->StepWorld(1);
    timeNow = world->GetSimTime().Double();
    error = link->GetWorldPose().pos - poseDes.pos;
    force.x = pidX.Update(error.x, timeNow-timeLast);
    force.y = pidY.Update(error.y, timeNow-timeLast);
    force.z = pidZ.Update(error.z, timeNow-timeLast);
    link->AddForce(force);
    timeLast = timeNow;
    velocity = link->GetWorldLinearVel();
    energy = 0.5 * (gainP * error*error  +  mass * velocity*velocity);
    gzerr << "energy " << energy
          << " velocity " << velocity << '\n';
    // getchar();

  }
  gzerr << "mass " << mass << '\n';
}

TEST_F(PhysicsEngineTest, PhysicsEngineStabilityODE)
{
  PhysicsEngineStability("ode");
}

#ifdef HAVE_BULLET
TEST_F(PhysicsEngineTest, PhysicsEngineStabilityBullet)
{
  PhysicsEngineStability("bullet");
}
#endif  // HAVE_BULLET

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
