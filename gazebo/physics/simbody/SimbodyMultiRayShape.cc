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
#include "physics/World.hh"
#include "physics/simbody/SimbodyTypes.hh"
#include "physics/simbody/SimbodyLink.hh"
#include "physics/simbody/SimbodyCollision.hh"
#include "physics/simbody/SimbodyPhysics.hh"
#include "physics/simbody/SimbodyRayShape.hh"
#include "physics/simbody/SimbodyMultiRayShape.hh"

using namespace gazebo;
using namespace physics;

//////////////////////////////////////////////////
SimbodyMultiRayShape::SimbodyMultiRayShape(CollisionPtr _parent)
: MultiRayShape(_parent)
{
  this->SetName("Simbody Multiray Shape");
  this->physicsEngine = boost::shared_static_cast<SimbodyPhysics>(
      this->collisionParent->GetWorld()->GetPhysicsEngine());
}

//////////////////////////////////////////////////
SimbodyMultiRayShape::~SimbodyMultiRayShape()
{
}

//////////////////////////////////////////////////
void SimbodyMultiRayShape::UpdateRays()
{
  std::vector< RayShapePtr >::iterator iter;
  for (iter = this->rays.begin(); iter != this->rays.end(); ++iter)
  {
    (*iter)->Update();
  }
}

//////////////////////////////////////////////////
void SimbodyMultiRayShape::AddRay(const math::Vector3 &_start,
    const math::Vector3 &_end)
{
  MultiRayShape::AddRay(_start, _end);
  SimbodyRayShapePtr ray(new SimbodyRayShape(this->physicsEngine));
  ray->SetPoints(_start, _end);

  this->rays.push_back(ray);
}
