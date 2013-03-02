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
/* Desc: The Bullet physics engine wrapper
 * Author: Nate Koenig
 * Date: 11 June 2007
 */

#include "gazebo/physics/bullet/BulletTypes.hh"
#include "gazebo/physics/bullet/BulletLink.hh"
#include "gazebo/physics/bullet/BulletCollision.hh"

#include "gazebo/physics/bullet/BulletPlaneShape.hh"
#include "gazebo/physics/bullet/BulletSphereShape.hh"
#include "gazebo/physics/bullet/BulletHeightmapShape.hh"
#include "gazebo/physics/bullet/BulletMultiRayShape.hh"
#include "gazebo/physics/bullet/BulletBoxShape.hh"
#include "gazebo/physics/bullet/BulletCylinderShape.hh"
#include "gazebo/physics/bullet/BulletTrimeshShape.hh"
#include "gazebo/physics/bullet/BulletRayShape.hh"

#include "gazebo/physics/bullet/BulletHingeJoint.hh"
#include "gazebo/physics/bullet/BulletUniversalJoint.hh"
#include "gazebo/physics/bullet/BulletBallJoint.hh"
#include "gazebo/physics/bullet/BulletSliderJoint.hh"
#include "gazebo/physics/bullet/BulletHinge2Joint.hh"
#include "gazebo/physics/bullet/BulletScrewJoint.hh"

#include "gazebo/transport/Publisher.hh"

#include "gazebo/physics/PhysicsTypes.hh"
#include "gazebo/physics/PhysicsFactory.hh"
#include "gazebo/physics/World.hh"
#include "gazebo/physics/Entity.hh"
#include "gazebo/physics/Model.hh"
#include "gazebo/physics/SurfaceParams.hh"
#include "gazebo/physics/Collision.hh"
#include "gazebo/physics/MapShape.hh"
#include "gazebo/physics/ContactManager.hh"

#include "gazebo/common/Assert.hh"
#include "gazebo/common/Console.hh"
#include "gazebo/common/Exception.hh"
#include "gazebo/math/Vector3.hh"
#include "gazebo/math/Rand.hh"

#include "gazebo/physics/bullet/BulletPhysics.hh"

using namespace gazebo;
using namespace physics;

GZ_REGISTER_PHYSICS_ENGINE("bullet", BulletPhysics)

extern ContactAddedCallback gContactAddedCallback;
extern ContactProcessedCallback gContactProcessedCallback;

//////////////////////////////////////////////////
struct CollisionFilter : public btOverlapFilterCallback
{
  // return true when pairs need collision
  virtual bool needBroadphaseCollision(btBroadphaseProxy *_proxy0,
      btBroadphaseProxy *_proxy1) const
    {
      GZ_ASSERT(_proxy0 != NULL && _proxy1 != NULL,
          "Bullet broadphase overlapping pair proxies are NULL");

      bool collide = (_proxy0->m_collisionFilterGroup
          & _proxy1->m_collisionFilterMask) != 0;
      collide = collide && (_proxy1->m_collisionFilterGroup
          & _proxy0->m_collisionFilterMask);

      btRigidBody *rb0 = btRigidBody::upcast(
              static_cast<btCollisionObject *>(_proxy0->m_clientObject));
      if (!rb0)
        return collide;

      btRigidBody *rb1 = btRigidBody::upcast(
              static_cast<btCollisionObject *>(_proxy1->m_clientObject));
      if (!rb1)
         return collide;

      BulletLink *link0 = static_cast<BulletLink *>(
          rb0->getUserPointer());
      GZ_ASSERT(link0 != NULL, "Link0 in collision pair is NULL");

      BulletLink *link1 = static_cast<BulletLink *>(
          rb1->getUserPointer());
      GZ_ASSERT(link1 != NULL, "Link1 in collision pair is NULL");

      if (!link0->GetSelfCollide() || !link1->GetSelfCollide())
      {
        if (link0->GetModel() == link1->GetModel())
          collide = false;
      }
      return collide;
    }
};
/*
//////////////////////////////////////////////////
void BulletPhysics::InternalTickCallback(btDynamicsWorld *_world, btScalar _timeStep)
{
  int numManifolds = _world->getDispatcher()->getNumManifolds();
  for (int i = 0; i < numManifolds; ++i)
  {
    btPersistentManifold *contactManifold =
        _world->getDispatcher()->getManifoldByIndexInternal(i);
    const btCollisionObject *obA =
        static_cast<const btCollisionObject *>(contactManifold->getBody0());
    const btCollisionObject *obB =
        static_cast<const btCollisionObject *>(contactManifold->getBody1());

    BulletLink *link0 = static_cast<BulletLink *>(
        obA->getUserPointer());
    GZ_ASSERT(link0 != NULL, "Link0 in collision pair is NULL");

    BulletLink *link1 = static_cast<BulletLink *>(
        obB->getUserPointer());
    GZ_ASSERT(link1 != NULL, "Link1 in collision pair is NULL");

    // Add a new contact to the manager. This will return NULL if no one is
    // listening for contact information.
    unsigned int index = 0;
    Contact *contactFeedback = this->contactManager->NewContact(
        link0->GetCollision(index), link1->GetCollision(index),
        this->world->GetSimTime());

    int numContacts = contactManifold->getNumContacts();

    for (int j = 0; j < numContacts; ++j)
    {
      btManifoldPoint& pt = contactManifold->getContactPoint(j);
      if (pt.getDistance() < 0.f)
      {
        const btVector3& ptA = pt.getPositionWorldOnA();
        const btVector3& ptB = pt.getPositionWorldOnB();
        const btVector3& normalOnB = pt.m_normalWorldOnB;

        contactFeedback->positions[j] = BulletTypes::ConvertVector3(ptA);
        contactFeedback->normal[j] = BulletTypes::ConvertVector3(normalOnB);
        contactFeedback->count++;
      }
    }
  }
}*/

//////////////////////////////////////////////////
bool BulletPhysics::ContactCallback(btManifoldPoint &_cp,
    const btCollisionObjectWrapper *_obj0, int /*_partId0*/,
    int /*_index0*/, const btCollisionObjectWrapper *_obj1,
    int /*_partId1*/, int /*_index1*/)
{

  if (_cp.getDistance() >= 0.f)
    return false;

  const btCollisionObject *col0 = _obj0->getCollisionObject();
  const btCollisionObject *col1 = _obj1->getCollisionObject();

  BulletLink *link0 = static_cast<BulletLink *>(
      col0->getUserPointer());
  GZ_ASSERT(link0 != NULL, "Link0 in collision pair is NULL");

  BulletLink *link1 = static_cast<BulletLink *>(
      col1->getUserPointer());
  GZ_ASSERT(link1 != NULL, "Link1 in collision pair is NULL");


  unsigned int colIndex = 0;
  CollisionPtr collisionPtr0 = link0->GetCollision(colIndex);
  CollisionPtr collisionPtr1 = link1->GetCollision(colIndex);
  if (!collisionPtr0 || !collisionPtr1)
    return false;

  PhysicsEnginePtr engine = collisionPtr0->GetWorld()->GetPhysicsEngine();
  BulletPhysicsPtr bulletPhysics =
        boost::shared_static_cast<BulletPhysics>(engine);



  /*Contact *contactFeedback = NULL;
  boost::unordered_map<CollisionPtr, Contact *> &contactMap
      = bulletPhysics->GetContactMap();
  gzerr << "contact feedback " << contactMap.size() << std::endl;


  if (contactMap.find(collisionPtr0) != contactMap.end())
  {
    contactFeedback = contactMap[collisionPtr0];
  }
  else if (contactMap.find(collisionPtr1) != contactMap.end())
  {
    contactFeedback = contactMap[collisionPtr1];
  }
  else
  {
    contactFeedback = bulletPhysics->GetContactManager()->
        NewContact(collisionPtr0.get(),
        collisionPtr1.get(), collisionPtr0->GetWorld()->GetSimTime());

    if (!contactFeedback)
      return false;

    contactMap[collisionPtr0] = contactFeedback;
    contactMap[collisionPtr1] = contactFeedback;
  }


//  const btVector3& ptA = _cp.getPositionWorldOnA();
  const btVector3& ptB = _cp.getPositionWorldOnB();
  const btVector3& normalOnB = _cp.m_normalWorldOnB;

  int contactIndex = std::max(0, contactFeedback->count - 1);
  contactFeedback->positions[contactIndex] = BulletTypes::ConvertVector3(ptB);
  contactFeedback->normals[contactIndex] = BulletTypes::ConvertVector3(normalOnB);
  contactFeedback->count++;*/

  const btVector3& ptB = _cp.getPositionWorldOnB();
  const btVector3& normalOnB = _cp.m_normalWorldOnB;

  bulletPhysics->AddContact(collisionPtr0, collisionPtr1,
    BulletTypes::ConvertVector3(ptB), BulletTypes::ConvertVector3(normalOnB));

//  gzerr << contactFeedback->count++ << std::endl;
  return false;
}

//////////////////////////////////////////////////
void BulletPhysics::AddContact(CollisionPtr _col0, CollisionPtr _col1,
    math::Vector3 _pos, math::Vector3 _normal)
{
  Contact *contactFeedback = NULL;
  if (contactMap.find(_col0) != contactMap.end())
  {
    contactFeedback = contactMap[_col0];
    gzerr << " f " << std::endl;
  }
  else if (contactMap.find(_col1) != contactMap.end())
  {
    contactFeedback = contactMap[_col1];
    gzerr << " f1 " << std::endl;
  }
  else
  {
    contactFeedback = this->contactManager->
        NewContact(_col0.get(),
        _col1.get(), _col0->GetWorld()->GetSimTime());

    if (!contactFeedback)
      return;

    contactMap[_col0] = contactFeedback;
    contactMap[_col1] = contactFeedback;

    gzerr << " new " << contactMap.size() << std::endl;
  }

  int contactIndex = std::max(0, contactFeedback->count - 1);
  contactFeedback->positions[contactIndex] = _pos;
  contactFeedback->normals[contactIndex] = _normal;
  contactFeedback->count++;
}

//////////////////////////////////////////////////
boost::unordered_map<CollisionPtr, Contact *> &BulletPhysics::GetContactMap()
{
  return contactMap;
}

//////////////////////////////////////////////////
bool ContactProcessed(btManifoldPoint &/*_cp*/, void * /*_body0*/,
                      void * /*_body1*/)
{
  return true;
}

//////////////////////////////////////////////////
BulletPhysics::BulletPhysics(WorldPtr _world)
    : PhysicsEngine(_world)
{
  // This function currently follows the pattern of bullet/Demos/HelloWorld

  // Default setup for memory and collisions
  this->collisionConfig = new btDefaultCollisionConfiguration();

  // Default collision dispatcher, a multi-threaded dispatcher may be available
  this->dispatcher = new btCollisionDispatcher(this->collisionConfig);

  // Broadphase collision detection uses axis-aligned bounding boxes (AABB)
  // to detect pairs of objects that may be in contact.
  // The narrow-phase collision detection evaluates each pair generated by the
  // broadphase.
  // "btDbvtBroadphase uses a fast dynamic bounding volume hierarchy based on
  // AABB tree" according to Bullet_User_Manual.pdf
  // "btAxis3Sweep and bt32BitAxisSweep3 implement incremental 3d sweep and
  // prune" also according to the user manual.
  // btCudaBroadphase can be used if GPU hardware is available
  // Here we are using btDbvtBroadphase.
  this->broadPhase = new btDbvtBroadphase();

  // Create btSequentialImpulseConstraintSolver, the default constraint solver.
  // Note that a multi-threaded solver may be available.
  this->solver = new btSequentialImpulseConstraintSolver;

  // Create a btDiscreteDynamicsWorld, which is used for discrete rigid bodies.
  // An alternative is btSoftRigidDynamicsWorld, which handles both soft and
  // rigid bodies.
  this->dynamicsWorld = new btDiscreteDynamicsWorld(this->dispatcher,
      this->broadPhase, this->solver, this->collisionConfig);

  btOverlapFilterCallback *filterCallback = new CollisionFilter();
  btOverlappingPairCache* pairCache = this->dynamicsWorld->getPairCache();
  GZ_ASSERT(pairCache != NULL,
      "Bullet broadphase overlapping pair cache is NULL");
  pairCache->setOverlapFilterCallback(filterCallback);

  // TODO: Enable this to do custom contact setting
  gContactAddedCallback = ContactCallback;
  gContactProcessedCallback = ContactProcessed;

//  this->dynamicsWorld->setInternalTickCallback(
//      BulletPhysics::InternalTickCallback, static_cast<void *>(this));

  // Set random seed for physics engine based on gazebo's random seed.
  // Note: this was moved from physics::PhysicsEngine constructor.
  this->SetSeed(math::Rand::GetSeed());
}

//////////////////////////////////////////////////
BulletPhysics::~BulletPhysics()
{
  // Delete in reverse-order of creation
  delete this->dynamicsWorld;
  delete this->solver;
  delete this->broadPhase;
  delete this->dispatcher;
  delete this->collisionConfig;

  this->dynamicsWorld = NULL;
  this->solver = NULL;
  this->broadPhase = NULL;
  this->dispatcher = NULL;
  this->collisionConfig = NULL;
}

//////////////////////////////////////////////////
void BulletPhysics::Load(sdf::ElementPtr _sdf)
{
  PhysicsEngine::Load(_sdf);

  sdf::ElementPtr bulletElem = this->sdf->GetElement("bullet");

  this->stepTimeDouble = bulletElem->GetElement("dt")->GetValueDouble();

  math::Vector3 g = this->sdf->GetValueVector3("gravity");
  // ODEPhysics checks this, so we will too.
  if (g == math::Vector3(0, 0, 0))
    gzwarn << "Gravity vector is (0, 0, 0). Objects will float.\n";
  this->dynamicsWorld->setGravity(btVector3(g.x, g.y, g.z));

  btContactSolverInfo& info = this->dynamicsWorld->getSolverInfo();

  // Split impulse feature. This reduces large bounces from deep penetrations,
  // but can lead to improper stacking of objects, see
  // http://bulletphysics.org/mediawiki-1.5.8/index.php/BtContactSolverInfo ...
  // ... #Split_Impulse
  info.m_splitImpulse = 1;
  info.m_splitImpulsePenetrationThreshold = -0.02;

  // Use multiple friction directions.
  // This is important for rolling without slip (see issue #480)
  info.m_solverMode |= SOLVER_USE_2_FRICTION_DIRECTIONS;

  if (bulletElem->HasElement("constraints"))
  {
    // the following are undocumented members of btContactSolverInfo, see
    // bulletphysics.org/mediawiki-1.5.8/index.php/BtContactSolverInfo ...
    // ... #Undocumented_members_of_btContactSolverInfo
    // m_globalCfm: constraint force mixing
    info.m_globalCfm =
      bulletElem->GetElement("constraints")->GetValueDouble("cfm");
    // m_erp: Baumgarte factor
    info.m_erp = bulletElem->GetElement("constraints")->GetValueDouble("erp");
  }
}

//////////////////////////////////////////////////
void BulletPhysics::Init()
{
}

//////////////////////////////////////////////////
void BulletPhysics::InitForThread()
{
}

/////////////////////////////////////////////////
void BulletPhysics::OnRequest(ConstRequestPtr &_msg)
{
  msgs::Response response;
  response.set_id(_msg->id());
  response.set_request(_msg->request());
  response.set_response("success");
  std::string *serializedData = response.mutable_serialized_data();

  if (_msg->request() == "physics_info")
  {
    msgs::Physics physicsMsg;
    physicsMsg.set_type(msgs::Physics::BULLET);
    physicsMsg.set_update_rate(this->GetUpdateRate());
    // This function was copied from ODEPhysics with portions commented out.
    // TODO: determine which of these should be implemented.
    // physicsMsg.set_solver_type(this->stepType);
    physicsMsg.set_dt(this->stepTimeDouble);
    // physicsMsg.set_iters(this->GetSORPGSIters());
    // physicsMsg.set_sor(this->GetSORPGSW());
    // physicsMsg.set_cfm(this->GetWorldCFM());
    // physicsMsg.set_erp(this->GetWorldERP());
    // physicsMsg.set_contact_max_correcting_vel(
    //     this->GetContactMaxCorrectingVel());
    // physicsMsg.set_contact_surface_layer(this->GetContactSurfaceLayer());
    physicsMsg.mutable_gravity()->CopyFrom(msgs::Convert(this->GetGravity()));

    response.set_type(physicsMsg.GetTypeName());
    physicsMsg.SerializeToString(serializedData);
    this->responsePub->Publish(response);
  }
}

/////////////////////////////////////////////////
void BulletPhysics::OnPhysicsMsg(ConstPhysicsPtr &_msg)
{
  if (_msg->has_dt())
    this->SetStepTime(_msg->dt());

  if (_msg->has_update_rate())
    this->SetUpdateRate(_msg->update_rate());

  // Like OnRequest, this function was copied from ODEPhysics.
  // TODO: change this when changing OnRequest.
  // if (_msg->has_solver_type())
  // {
  //   sdf::ElementPtr solverElem =
  //     this->sdf->GetElement("ode")->GetElement("solver");
  //   if (_msg->solver_type() == "quick")
  //   {
  //     solverElem->GetAttribute("type")->Set("quick");
  //     this->physicsStepFunc = &dWorldQuickStep;
  //   }
  //   else if (_msg->solver_type() == "world")
  //   {
  //     solverElem->GetAttribute("type")->Set("world");
  //     this->physicsStepFunc = &dWorldStep;
  //   }
  // }

  // if (_msg->has_iters())
  //   this->SetSORPGSIters(_msg->iters());

  // if (_msg->has_sor())
  //   this->SetSORPGSW(_msg->sor());

  // if (_msg->has_cfm())
  //   this->SetWorldCFM(_msg->cfm());

  // if (_msg->has_erp())
  //   this->SetWorldERP(_msg->erp());

  // if (_msg->has_contact_max_correcting_vel())
  //   this->SetContactMaxCorrectingVel(_msg->contact_max_correcting_vel());

  // if (_msg->has_contact_surface_layer())
  //   this->SetContactSurfaceLayer(_msg->contact_surface_layer());

  if (_msg->has_gravity())
    this->SetGravity(msgs::Convert(_msg->gravity()));

  /// Make sure all models get at least one update cycle.
  this->world->EnableAllModels();
}

//////////////////////////////////////////////////
void BulletPhysics::UpdateCollision()
{
  contactMap.clear();
  gzerr << " clear " << std::endl;
}

//////////////////////////////////////////////////
void BulletPhysics::UpdatePhysics()
{
  // need to lock, otherwise might conflict with world resetting
  boost::recursive_mutex::scoped_lock lock(*this->physicsUpdateMutex);

  // common::Time currTime =  this->world->GetRealTime();

  this->dynamicsWorld->stepSimulation(
      this->stepTimeDouble, 1, this->stepTimeDouble);
  // this->lastUpdateTime = currTime;
}

//////////////////////////////////////////////////
void BulletPhysics::Fini()
{
  PhysicsEngine::Fini();
}

//////////////////////////////////////////////////
void BulletPhysics::Reset()
{
  // See DemoApplication::clientResetScene() in
  // bullet/Demos/OpenGL/DemoApplication.cpp
  // this->physicsUpdateMutex->lock();
  // this->physicsUpdateMutex->unlock();
}

//////////////////////////////////////////////////
void BulletPhysics::SetStepTime(double _value)
{
  if (this->sdf->HasElement("bullet") &&
      this->sdf->GetElement("bullet")->HasElement("dt"))
    this->sdf->GetElement("bullet")->GetElement("dt")->Set(_value);
  else
    gzerr << "Unable to set bullet step time\n";

  this->stepTimeDouble = _value;
}

//////////////////////////////////////////////////
double BulletPhysics::GetStepTime()
{
  return this->stepTimeDouble;
}

// //////////////////////////////////////////////////
// void BulletPhysics::SetSORPGSIters(unsigned int _iters)
// {
//   // TODO: set SDF parameter
//   btContactSolverInfo& info = this->dynamicsWorld->getSolverInfo();
//   // Line below commented out because it wasn't helping pendulum test.
//   // info.m_numIterations = _iters;
// }

//////////////////////////////////////////////////
LinkPtr BulletPhysics::CreateLink(ModelPtr _parent)
{
  if (_parent == NULL)
    gzthrow("Link must have a parent\n");

  BulletLinkPtr link(new BulletLink(_parent));
  link->SetWorld(_parent->GetWorld());

  return link;
}

//////////////////////////////////////////////////
CollisionPtr BulletPhysics::CreateCollision(const std::string &_type,
                                            LinkPtr _parent)
{
  BulletCollisionPtr collision(new BulletCollision(_parent));
  ShapePtr shape = this->CreateShape(_type, collision);
  collision->SetShape(shape);
  shape->SetWorld(_parent->GetWorld());
  return collision;
}

//////////////////////////////////////////////////
ShapePtr BulletPhysics::CreateShape(const std::string &_type,
                                    CollisionPtr _collision)
{
  ShapePtr shape;
  BulletCollisionPtr collision =
    boost::shared_dynamic_cast<BulletCollision>(_collision);

  if (_type == "plane")
    shape.reset(new BulletPlaneShape(collision));
  else if (_type == "sphere")
    shape.reset(new BulletSphereShape(collision));
  else if (_type == "box")
    shape.reset(new BulletBoxShape(collision));
  else if (_type == "cylinder")
    shape.reset(new BulletCylinderShape(collision));
  else if (_type == "mesh" || _type == "trimesh")
    shape.reset(new BulletTrimeshShape(collision));
  else if (_type == "heightmap")
    shape.reset(new BulletHeightmapShape(collision));
  else if (_type == "multiray")
    shape.reset(new BulletMultiRayShape(collision));
  else if (_type == "ray")
    if (_collision)
      shape.reset(new BulletRayShape(_collision));
    else
      shape.reset(new BulletRayShape(this->world->GetPhysicsEngine()));
  else
    gzerr << "Unable to create collision of type[" << _type << "]\n";

  /*
  else if (_type == "map" || _type == "image")
    shape.reset(new MapShape(collision));
    */
  return shape;
}

//////////////////////////////////////////////////
JointPtr BulletPhysics::CreateJoint(const std::string &_type, ModelPtr _parent)
{
  JointPtr joint;

  if (_type == "revolute")
    joint.reset(new BulletHingeJoint(this->dynamicsWorld, _parent));
  else if (_type == "universal")
    joint.reset(new BulletUniversalJoint(this->dynamicsWorld, _parent));
  else if (_type == "ball")
    joint.reset(new BulletBallJoint(this->dynamicsWorld, _parent));
  else if (_type == "prismatic")
    joint.reset(new BulletSliderJoint(this->dynamicsWorld, _parent));
  else if (_type == "revolute2")
    joint.reset(new BulletHinge2Joint(this->dynamicsWorld, _parent));
  else if (_type == "screw")
    joint.reset(new BulletScrewJoint(this->dynamicsWorld, _parent));
  else
    gzthrow("Unable to create joint of type[" << _type << "]");

  return joint;
}

//////////////////////////////////////////////////
void BulletPhysics::ConvertMass(InertialPtr /*_inertial*/,
                                void * /*_engineMass*/)
{
}

//////////////////////////////////////////////////
void BulletPhysics::ConvertMass(void * /*_engineMass*/,
                                const InertialPtr /*_inertial*/)
{
}

//////////////////////////////////////////////////
double BulletPhysics::GetWorldCFM()
{
  sdf::ElementPtr elem = this->sdf->GetElement("bullet");
  elem = elem->GetElement("constraints");
  return elem->GetValueDouble("cfm");
}

//////////////////////////////////////////////////
void BulletPhysics::SetWorldCFM(double _cfm)
{
  sdf::ElementPtr elem = this->sdf->GetElement("bullet");
  elem = elem->GetElement("constraints");
  elem->GetElement("cfm")->Set(_cfm);

  btContactSolverInfo& info = this->dynamicsWorld->getSolverInfo();
  info.m_globalCfm = _cfm;
}

//////////////////////////////////////////////////
void BulletPhysics::SetGravity(const gazebo::math::Vector3 &_gravity)
{
  this->sdf->GetElement("gravity")->Set(_gravity);
  this->dynamicsWorld->setGravity(
    BulletTypes::ConvertVector3(_gravity));
}

//////////////////////////////////////////////////
void BulletPhysics::DebugPrint() const
{
}

/////////////////////////////////////////////////
void BulletPhysics::SetSeed(uint32_t /*_seed*/)
{
  // GEN_srand is defined in btRandom.h, but nothing in bullet uses it
  // GEN_srand(_seed);

  // The best bet is probably btSequentialImpulseConstraintSolver::setRandSeed,
  // but it's not a static function.
  // There's 2 other instances of random number generation in bullet classes:
  //  btSoftBody.cpp:1160
  //  btConvexHullComputer.cpp:2188

  // It's going to be blank for now.
  /// \todo Implement this function.
}
