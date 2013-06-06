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

#include "gazebo/physics/World.hh"

#include "gazebo/physics/dart/DARTPhysics.hh"
#include "gazebo/physics/dart/DARTLink.hh"
#include "gazebo/physics/dart/DARTModel.hh"
#include "gazebo/physics/dart/DARTUtils.hh"

using namespace gazebo;
using namespace physics;

//////////////////////////////////////////////////
DARTModel::DARTModel(BasePtr _parent)
  : Model(_parent), dartSkeleton(NULL),
    dartCanonicalJoint(NULL)
{
}

//////////////////////////////////////////////////
DARTModel::~DARTModel()
{
  if (dartSkeleton)
    delete dartSkeleton;
}

//////////////////////////////////////////////////
void DARTModel::Load(sdf::ElementPtr _sdf)
{
  // create skeletonDynamics of DART
  this->dartSkeleton = new dart::dynamics::Skeleton();

  Model::Load(_sdf);

  // Name
  std::string modelName = this->GetName();
  this->dartSkeleton->setName(modelName.c_str());

  //  if (this->IsStatic())
  //    dartSkeletonDynamics->setImmobileState(true);
  //  else
  //    dartSkeletonDynamics->setImmobileState(false);
  dartSkeleton->setImmobileState(this->IsStatic());
}

//////////////////////////////////////////////////
void DARTModel::Init()
{
  Model::Init();

  // add skeleton to world
  this->GetDARTWorld()->addSkeleton(dartSkeleton);
}


//////////////////////////////////////////////////
void DARTModel::Update()
{
  Model::Update();
  
}

//////////////////////////////////////////////////
void DARTModel::Fini()
{
  Model::Fini();
  
}

//////////////////////////////////////////////////
//void DARTModel::Reset()
//{
//  Model::Reset();

//}

//////////////////////////////////////////////////
DARTPhysicsPtr DARTModel::GetDARTPhysics(void) const {
  return boost::shared_dynamic_cast<DARTPhysics>(this->GetWorld()->GetPhysicsEngine());
}

//////////////////////////////////////////////////
dart::simulation::World* DARTModel::GetDARTWorld(void) const
{
  return GetDARTPhysics()->GetDARTWorld();
}
