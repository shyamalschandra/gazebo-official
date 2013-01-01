#include "common/Console.hh"
#include "common/Exception.hh"
#include "math/Vector3.hh"

#include "physics/PhysicsTypes.hh"
#include "physics/PhysicsFactory.hh"
#include "physics/World.hh"
#include "physics/Entity.hh"
#include "physics/Model.hh"
#include "physics/SurfaceParams.hh"
#include "physics/Collision.hh"
#include "physics/MapShape.hh"

#include "gazebo/physics/rtql8/RTQL8ScrewJoint.hh"
#include "gazebo/physics/rtql8/RTQL8HingeJoint.hh"
#include "gazebo/physics/rtql8/RTQL8Hinge2Joint.hh"
#include "gazebo/physics/rtql8/RTQL8SliderJoint.hh"
#include "gazebo/physics/rtql8/RTQL8BallJoint.hh"
#include "gazebo/physics/rtql8/RTQL8UniversalJoint.hh"

#include "physics/rtql8/RTQL8RayShape.hh"
#include "physics/rtql8/RTQL8BoxShape.hh"
#include "physics/rtql8/RTQL8SphereShape.hh"
#include "physics/rtql8/RTQL8CylinderShape.hh"
#include "physics/rtql8/RTQL8PlaneShape.hh"
#include "physics/rtql8/RTQL8TrimeshShape.hh"
#include "physics/rtql8/RTQL8MultiRayShape.hh"
#include "physics/rtql8/RTQL8HeightmapShape.hh"

#include "physics/rtql8/RTQL8Link.hh"


#include "RTQL8Physics.hh"

using namespace gazebo;
using namespace physics;

GZ_REGISTER_PHYSICS_ENGINE("rtql8", RTQL8Physics)

// extern ContactAddedCallback gContactAddedCallback;
// extern ContactProcessedCallback gContactProcessedCallback;
// 
// //////////////////////////////////////////////////
// bool ContactCallback(btManifoldPoint &/*_cp*/,
//     const btCollisionObjectWrapper * /*_obj0*/, int /*_partId0*/,
//     int /*_index0*/, const btCollisionObjectWrapper * /*_obj1*/,
//     int /*_partId1*/, int /*_index1*/)
// {
//   return true;
// }
// 
// //////////////////////////////////////////////////
// bool ContactProcessed(btManifoldPoint &/*_cp*/, void * /*_body0*/,
//                       void * /*_body1*/)
// {
//   return true;
// }
// 
//////////////////////////////////////////////////
RTQL8Physics::RTQL8Physics(WorldPtr _world)
    : PhysicsEngine(_world)
{
//   // Create the dynamics solver
//   this->solver = new btSequentialImpulseConstraintSolver;
// 
//   // Instantiate the world
//   this->dynamicsWorld = new btDiscreteDynamicsWorld(this->dispatcher,
//       this->broadPhase, this->solver, this->collisionConfig);

  this->rtql8World = new simulation::World;
}

//////////////////////////////////////////////////
RTQL8Physics::~RTQL8Physics()
{
//   delete this->broadPhase;
//   delete this->collisionConfig;
//   delete this->dispatcher;
//   delete this->solver;
// 
//   // TODO: Fix this line
//   // delete this->dynamicsWorld;
// 
//   this->broadPhase = NULL;
//   this->collisionConfig = NULL;
//   this->dispatcher = NULL;
//   this->solver = NULL;
//   this->dynamicsWorld = NULL;

  delete this->rtql8World;
  
  this->rtql8World = NULL;
}

//////////////////////////////////////////////////
void RTQL8Physics::Load(sdf::ElementPtr _sdf)
{
  PhysicsEngine::Load(_sdf);

  // Gravity
  math::Vector3 g = this->sdf->GetValueVector3("gravity");
  this->rtql8World->setGravity(Eigen::Vector3d(g.x, g.y, g.z));
  
  // Time step
  this->rtql8World->setTimeStep(this->sdf->GetValueDouble("time_step"));
  
  // TODO: Elements for rtql8 settings
  sdf::ElementPtr rtql8Elem = this->sdf->GetElement("rtql8");
  //this->stepTimeDouble = rtql8Elem->GetElement("dt")->GetValueDouble();
  
}
 
//////////////////////////////////////////////////
void RTQL8Physics::Init()
{
  this->rtql8World->init();
}

//////////////////////////////////////////////////
void RTQL8Physics::Fini()
{
}

//////////////////////////////////////////////////
void RTQL8Physics::Reset()
{
}

//////////////////////////////////////////////////
void RTQL8Physics::InitForThread()
{
}
 
//////////////////////////////////////////////////
void RTQL8Physics::UpdateCollision()
{
}

//////////////////////////////////////////////////
void RTQL8Physics::UpdatePhysics()
{
  // need to lock, otherwise might conflict with world resetting
  this->physicsUpdateMutex->lock();

  //common::Time currTime =  this->world->GetRealTime();
  this->rtql8World->updatePhysics();
  //this->lastUpdateTime = currTime;

  this->physicsUpdateMutex->unlock();
}

//////////////////////////////////////////////////
void RTQL8Physics::SetStepTime(double _value)
{
   // TODO: element of dt
//   this->sdf->GetElement("ode")->GetElement(
//       "solver")->GetAttribute("dt")->Set(_value);
   this->stepTimeDouble = _value;
   this->rtql8World->setTimeStep(_value);
}

//////////////////////////////////////////////////
double RTQL8Physics::GetStepTime()
{
  return this->stepTimeDouble;
  //return this->world->getTimeStep();
}

//////////////////////////////////////////////////
LinkPtr RTQL8Physics::CreateLink(ModelPtr _parent)
{
  if (_parent == NULL)
	gzthrow("Link must have a parent\n");

  RTQL8LinkPtr link(new RTQL8Link(_parent));

  link->SetWorld(_parent->GetWorld());

  return link;
}

//////////////////////////////////////////////////
CollisionPtr RTQL8Physics::CreateCollision(const std::string &_type,
                                            LinkPtr _body)
{
  RTQL8CollisionPtr collision(new RTQL8Collision(_body));
  
  ShapePtr shape = this->CreateShape(_type, collision);
  
  collision->SetShape(shape);
  
  shape->SetWorld(_body->GetWorld());
  
  return collision;
}

//////////////////////////////////////////////////
ShapePtr RTQL8Physics::CreateShape(const std::string &_type,
                                    CollisionPtr _collision)
{
  ShapePtr shape;
  
  RTQL8CollisionPtr collision =
    boost::shared_dynamic_cast<RTQL8Collision>(_collision);

  if (_type == "sphere")
    shape.reset(new RTQL8SphereShape(collision));
  else if (_type == "plane")
    shape.reset(new RTQL8PlaneShape(collision));
  else if (_type == "box")
    shape.reset(new RTQL8BoxShape(collision));
  else if (_type == "cylinder")
    shape.reset(new RTQL8CylinderShape(collision));
  else if (_type == "multiray")
    shape.reset(new RTQL8MultiRayShape(collision));
  else if (_type == "mesh" || _type == "trimesh")
    shape.reset(new RTQL8TrimeshShape(collision));
  else if (_type == "heightmap")
    shape.reset(new RTQL8HeightmapShape(collision));
  else if (_type == "map" || _type == "image")
    shape.reset(new MapShape(collision));
  else if (_type == "ray")
    if (_collision)
      shape.reset(new RTQL8RayShape(collision));
    else
      shape.reset(new RTQL8RayShape(this->world->GetPhysicsEngine()));
  else
    gzerr << "Unable to create collision of type[" << _type << "]\n";

  return shape;
}

//////////////////////////////////////////////////
JointPtr RTQL8Physics::CreateJoint(const std::string &_type, ModelPtr _parent)
{
  JointPtr joint;

  if (_type == "prismatic")
    joint.reset(new RTQL8SliderJoint(_parent));
  else if (_type == "screw")
    joint.reset(new RTQL8ScrewJoint(_parent));
  else if (_type == "revolute")
    joint.reset(new RTQL8HingeJoint(_parent));
  else if (_type == "revolute2")
    joint.reset(new RTQL8Hinge2Joint(_parent));
  else if (_type == "ball")
    joint.reset(new RTQL8BallJoint(_parent));
  else if (_type == "universal")
    joint.reset(new RTQL8UniversalJoint(_parent));
  else
    gzthrow("Unable to create joint of type[" << _type << "]");

  return joint;
}

//////////////////////////////////////////////////
void RTQL8Physics::SetGravity(const gazebo::math::Vector3& _gravity)
{
  this->sdf->GetElement("gravity")->GetAttribute("xyz")->Set(_gravity);
  this->rtql8World->setGravity(Eigen::Vector3d(_gravity.x, _gravity.y, _gravity.z));
}

//////////////////////////////////////////////////
void RTQL8Physics::DebugPrint() const
{
//   dBodyID b;
//   std::cout << "Debug Print[" << dWorldGetBodyCount(this->worldId) << "]\n";
//   for (int i = 0; i < dWorldGetBodyCount(this->worldId); ++i)
//   {
//     b = dWorldGetBody(this->worldId, i);
//     ODELink *link = static_cast<ODELink*>(dBodyGetData(b));
//     math::Pose pose = link->GetWorldPose();
//     const dReal *pos = dBodyGetPosition(b);
//     const dReal *rot = dBodyGetRotation(b);
//     math::Vector3 dpos(pos[0], pos[1], pos[2]);
//     math::Quaternion drot(rot[0], rot[1], rot[2], rot[3]);
// 
//     std::cout << "Body[" << link->GetScopedName() << "]\n";
//     std::cout << "  World: Pos[" << dpos << "] Rot[" << drot << "]\n";
//     if (pose.pos != dpos)
//       std::cout << "    Incorrect world pos[" << pose.pos << "]\n";
//     if (pose.rot != drot)
//       std::cout << "    Incorrect world rot[" << pose.rot << "]\n";
// 
//     dMass mass;
//     dBodyGetMass(b, &mass);
//     std::cout << "  Mass[" << mass.mass << "] COG[" << mass.c[0]
//               << " " << mass.c[1] << " " << mass.c[2] << "]\n";
// 
//     dGeomID g = dBodyGetFirstGeom(b);
//     while (g)
//     {
//       ODECollision *coll = static_cast<ODECollision*>(dGeomGetData(g));
// 
//       pose = coll->GetWorldPose();
//       const dReal *gpos = dGeomGetPosition(g);
//       const dReal *grot = dGeomGetRotation(g);
//       dpos.Set(gpos[0], gpos[1], gpos[2]);
//       drot.Set(grot[0], grot[1], grot[2], grot[3]);
// 
//       std::cout << "    Geom[" << coll->GetScopedName() << "]\n";
//       std::cout << "      World: Pos[" << dpos << "] Rot[" << drot << "]\n";
// 
//       if (pose.pos != dpos)
//         std::cout << "      Incorrect world pos[" << pose.pos << "]\n";
//       if (pose.rot != drot)
//         std::cout << "      Incorrect world rot[" << pose.rot << "]\n";
// 
//       g = dBodyGetNextGeom(g);
//     }
//   }
}


