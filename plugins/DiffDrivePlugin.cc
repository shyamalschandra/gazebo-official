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

#include "physics/physics.h"
#include "transport/transport.h"
#include "plugins/DiffDrivePlugin.hh"

using namespace gazebo;
GZ_REGISTER_MODEL_PLUGIN(DiffDrivePlugin)

enum {RIGHT, LEFT};

/////////////////////////////////////////////////
DiffDrivePlugin::DiffDrivePlugin()
{
  this->wheelSpeed[LEFT] = this->wheelSpeed[RIGHT] = 0;
}

/////////////////////////////////////////////////
void DiffDrivePlugin::Load(physics::ModelPtr _model,
                           sdf::ElementPtr _sdf)
{
  this->model = _model;

  this->node = transport::NodePtr(new transport::Node());
  this->node->Init(model->GetWorld()->GetName());

  this->velSub = this->node->Subscribe(std::string("~/") +
      this->model->GetName() + "/vel_cmd", &DiffDrivePlugin::OnVelMsg, this);

  if (!_sdf->HasElement("left_joint"))
    gzerr << "DiffDrive plugin missing <left_joint> element\n";

  if (!_sdf->HasElement("right_joint"))
    gzerr << "DiffDrive plugin missing <right_joint> element\n";

  this->leftJoint = _model->GetJoint(
      _sdf->GetElement("left_joint")->GetValueString());
  this->rightJoint = _model->GetJoint(
      _sdf->GetElement("right_joint")->GetValueString());

  if (_sdf->HasElement("torque"))
    this->torque = _sdf->GetElement("torque")->GetValueDouble();
  else
  {
    gzwarn << "No torque value set for the DiffDrive plugin.\n";
    this->torque = 5.0;
  }

  if (!this->leftJoint)
    gzerr << "Unable to find left joint["
          << _sdf->GetElement("left_joint")->GetValueString() << "]\n";
  if (!this->rightJoint)
    gzerr << "Unable to find right joint["
          << _sdf->GetElement("right_joint")->GetValueString() << "]\n";

  this->updateConnection = event::Events::ConnectWorldUpdateStart(
          boost::bind(&DiffDrivePlugin::OnUpdate, this));
}

/////////////////////////////////////////////////
void DiffDrivePlugin::Init()
{
  this->wheelSeparation = this->leftJoint->GetAnchor(0).Distance(
      this->rightJoint->GetAnchor(0));

  physics::EntityPtr parent = boost::shared_dynamic_cast<physics::Entity>(
      this->leftJoint->GetChild());

  math::Box bb = parent->GetBoundingBox();
  math::Vector3 size = bb.GetSize() * this->leftJoint->GetLocalAxis(0);

  this->wheelRadius = (bb.GetSize().GetSum() - size.GetSum()) * 0.5;

  this->leftJoint->SetMaxForce(0, this->torque);
  this->rightJoint->SetMaxForce(0, this->torque);
}

/////////////////////////////////////////////////
void DiffDrivePlugin::OnVelMsg(ConstPosePtr &_msg)
{
  double vr, va;

  vr = _msg->position().x();
  va =  msgs::Convert(_msg->orientation()).GetAsEuler().z;

  this->wheelSpeed[LEFT] = vr + va * this->wheelSeparation / 2;
  this->wheelSpeed[RIGHT] = vr - va * this->wheelSeparation / 2;
}

/////////////////////////////////////////////////
void DiffDrivePlugin::OnUpdate()
{
  /*double d1, d2;
  double dr, da;
  common::Time stepTime;
  common::Time currTime = this->model->GetWorld()->GetSimTime();

  stepTime = currTime - this->prevUpdateTime;
  this->prevUpdateTime = currTime; 
  */

  // Distance travelled by front wheels
  /* 
  d1 = stepTime.Double() * this->wheelRadius * this->leftJoint->GetVelocity(0);
  d2 = stepTime.Double() * this->wheelRadius * this->rightJoint->GetVelocity(0);

  dr = (d1 + d2) / 2;
  da = (d1 - d2) / this->wheelSeparation;
  std::cout << "DR[" << dr << "]\n";
  */
  // std::cout << "JS[" << this->leftJoint->GetVelocity(0) << " "
  //    << this->rightJoint->GetVelocity(0) << "]\n";
  /*
  this->leftJoint->SetVelocity(0, this->wheelSpeed[LEFT]/this->wheelRadius);
  this->rightJoint->SetVelocity(0, this->wheelSpeed[RIGHT]/this->wheelRadius);
  */
  double leftVel = this->leftJoint->GetVelocity(0);
  double rightVel = this->leftJoint->GetVelocity(0);

  double leftErr = (this->wheelSpeed[LEFT] / this->wheelRadius) - leftVel;
  double rightErr = (this->wheelSpeed[RIGHT] / this->wheelRadius) - rightVel;

  double leftForce = std::min(leftErr * 10, this->torque);
  double rightForce = std::min(rightErr * 10, this->torque);

  printf("Left Vel[%f] Err[%f] Force[%f]\n", leftVel, leftErr, leftForce);
  printf("Right Vel[%f] Err[%f] Force[%f]\n", rightVel, rightErr, rightForce);

  this->leftJoint->SetForce(0, leftForce);
  this->rightJoint->SetForce(0, rightForce);
}
