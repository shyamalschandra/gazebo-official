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

 #include "common/Exception.hh"

#include "physics/World.hh"
#include "physics/bullet/BulletTypes.hh"
#include "physics/bullet/BulletLink.hh"
#include "physics/bullet/BulletCollision.hh"
#include "physics/bullet/BulletPhysics.hh"
#include "physics/bullet/BulletRayShape.hh"
#include "physics/bullet/BulletMultiRayShape.hh"

using namespace gazebo;
using namespace physics;

//////////////////////////////////////////////////
BulletMultiRayShape::BulletMultiRayShape(CollisionPtr _parent)
: MultiRayShape(_parent)
{
  this->SetName("Bullet Multiray Shape");
  this->physicsEngine = boost::shared_static_cast<BulletPhysics>(
      this->collisionParent->GetWorld()->GetPhysicsEngine());
}

//////////////////////////////////////////////////
BulletMultiRayShape::~BulletMultiRayShape()
{
}

//////////////////////////////////////////////////
void BulletMultiRayShape::UpdateRays()
{
  BulletPhysicsPtr bullet = boost::shared_dynamic_cast<BulletPhysics>(
      this->GetWorld()->GetPhysicsEngine());

  if (bullet == NULL)
    gzthrow("Invalid physics engine.");

  bullet->GetPhysicsUpdateMutex()->lock();
  std::vector< RayShapePtr >::iterator iter;
  for (iter = this->rays.begin(); iter != this->rays.end(); ++iter)
  {
    (*iter)->Update();
  }
  bullet->GetPhysicsUpdateMutex()->unlock();
}

//////////////////////////////////////////////////
void BulletMultiRayShape::AddRay(const math::Vector3 &_start,
    const math::Vector3 &_end)
{
  MultiRayShape::AddRay(_start, _end);

  BulletCollisionPtr bulletCollision(new BulletCollision(
        this->collisionParent->GetLink()));
  bulletCollision->SetName("bullet_ray_collision");

  BulletRayShapePtr ray(new BulletRayShape(bulletCollision));
  bulletCollision->SetShape(ray);
  ray->SetPoints(_start, _end);

  this->rays.push_back(ray);
}
