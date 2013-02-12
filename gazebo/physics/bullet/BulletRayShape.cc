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
/* Desc: A ray
 * Author: Nate Koenig
 * Date: 24 May 2009
 */

#include "gazebo/physics/World.hh"
#include "gazebo/physics/bullet/BulletLink.hh"
#include "gazebo/physics/bullet/BulletPhysics.hh"
#include "gazebo/physics/bullet/BulletTypes.hh"
#include "gazebo/physics/bullet/BulletCollision.hh"
#include "gazebo/physics/bullet/BulletRayShape.hh"

using namespace gazebo;
using namespace physics;

//////////////////////////////////////////////////
BulletRayShape::BulletRayShape(PhysicsEnginePtr _physicsEngine)
  : RayShape(_physicsEngine),
    rayCallback(btVector3(0, 0, 0), btVector3(0, 0, 0))
{
  this->SetName("Bullet Ray Shape");

  this->physicsEngine =
    boost::shared_static_cast<BulletPhysics>(_physicsEngine);
}

//////////////////////////////////////////////////
BulletRayShape::BulletRayShape(CollisionPtr _parent)
    : RayShape(_parent),
    rayCallback(btVector3(0, 0, 0), btVector3(0, 0, 0))
{
  this->SetName("Bullet Ray Shape");
  this->physicsEngine = boost::shared_static_cast<BulletPhysics>(
      this->collisionParent->GetWorld()->GetPhysicsEngine());
}

//////////////////////////////////////////////////
BulletRayShape::~BulletRayShape()
{
}

//////////////////////////////////////////////////
void BulletRayShape::Update()
{
  if (this->collisionParent)
  {
    BulletCollisionPtr collision =
      boost::shared_static_cast<BulletCollision>(this->collisionParent);

    this->globalStartPos =
      this->collisionParent->GetLink()->GetWorldPose().CoordPositionAdd(
          this->relativeStartPos);

    this->globalEndPos =
      this->collisionParent->GetLink()->GetWorldPose().CoordPositionAdd(
          this->relativeEndPos);
  }

  this->rayCallback.m_rayFromWorld.setX(this->globalStartPos.x);
  this->rayCallback.m_rayFromWorld.setY(this->globalStartPos.y);
  this->rayCallback.m_rayFromWorld.setZ(this->globalStartPos.z);

  this->rayCallback.m_rayToWorld.setX(this->globalEndPos.x);
  this->rayCallback.m_rayToWorld.setY(this->globalEndPos.y);
  this->rayCallback.m_rayToWorld.setZ(this->globalEndPos.z);

  this->physicsEngine->GetDynamicsWorld()->rayTest(
      this->rayCallback.m_rayFromWorld, this->rayCallback.m_rayToWorld,
      this->rayCallback);

  if (this->rayCallback.hasHit())
  {
    math::Vector3 result(this->rayCallback.m_hitPointWorld.getX(),
                         this->rayCallback.m_hitPointWorld.getY(),
                         this->rayCallback.m_hitPointWorld.getZ());
    this->SetLength(this->globalStartPos.Distance(result));
  }
}

//////////////////////////////////////////////////
void BulletRayShape::GetIntersection(double &_dist, std::string &_entity)
{
  _dist = 0;
  _entity = "";

  if (this->physicsEngine && this->collisionParent)
  {
    this->physicsEngine->GetDynamicsWorld()->rayTest(
        this->rayCallback.m_rayFromWorld, this->rayCallback.m_rayToWorld,
        this->rayCallback);
    if (this->rayCallback.hasHit())
    {
      math::Vector3 result(this->rayCallback.m_hitPointWorld.getX(),
                           this->rayCallback.m_hitPointWorld.getY(),
                           this->rayCallback.m_hitPointWorld.getZ());
      _dist = this->globalStartPos.Distance(result);
      _entity = static_cast<BulletLink*>(
          this->rayCallback.m_collisionObject->getUserPointer())->GetName();
    }
  }
}

//////////////////////////////////////////////////
void BulletRayShape::SetPoints(const math::Vector3 &_posStart,
                                   const math::Vector3 &_posEnd)
{
  RayShape::SetPoints(_posStart, _posEnd);

  this->rayCallback.m_rayFromWorld.setX(this->globalStartPos.x);
  this->rayCallback.m_rayFromWorld.setY(this->globalStartPos.y);
  this->rayCallback.m_rayFromWorld.setZ(this->globalStartPos.z);

  this->rayCallback.m_rayToWorld.setX(this->globalEndPos.x);
  this->rayCallback.m_rayToWorld.setY(this->globalEndPos.y);
  this->rayCallback.m_rayToWorld.setZ(this->globalEndPos.z);
}
