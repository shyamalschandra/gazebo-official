/*
 * Copyright (C) 2012-2015 Open Source Robotics Foundation
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
#include <sdf/sdf.hh>

#include "gazebo/msgs/msgs.hh"
#include "gazebo/common/Exception.hh"
#include "gazebo/common/Console.hh"
#include "gazebo/common/Events.hh"

#include "gazebo/transport/TransportIface.hh"
#include "gazebo/transport/Node.hh"

#include "gazebo/math/Rand.hh"

#include "gazebo/physics/ContactManager.hh"
#include "gazebo/physics/Link.hh"
#include "gazebo/physics/Model.hh"
#include "gazebo/physics/World.hh"
#include "gazebo/physics/PhysicsEngine.hh"
#include "gazebo/physics/PresetManager.hh"

using namespace gazebo;
using namespace physics;

//////////////////////////////////////////////////
PhysicsEngine::PhysicsEngine(WorldPtr _world)
  : world(_world)
{
  this->sdf.reset(new sdf::Element);
  sdf::initFile("physics.sdf", this->sdf);

  this->targetRealTimeFactor = 0;
  this->realTimeUpdateRate = 0;
  this->maxStepSize = 0;

  this->node = transport::NodePtr(new transport::Node());
  this->node->Init(this->world->GetName());
  this->physicsSub = this->node->Subscribe("~/physics",
      &PhysicsEngine::OnPhysicsMsg, this);

  this->responsePub =
    this->node->Advertise<msgs::Response>("~/response");

  this->requestSub = this->node->Subscribe("~/request",
                                           &PhysicsEngine::OnRequest, this);

  this->physicsUpdateMutex = new boost::recursive_mutex();

  // Create and initialized the contact manager.
  this->contactManager = new ContactManager();
  this->contactManager->Init(this->world);
}

void PhysicsEngine::ParamsFromSDFElement(sdf::ElementPtr _elem)
{
  if (_elem == NULL)
    return;

  // Call this function on child elements (not just params)
  for (sdf::ElementPtr temp = _elem->GetFirstElement(); temp != NULL;
       temp = temp->GetNextElement())
  {
    if (temp->GetName() == "param")
    {
      std::string name = temp->Get<std::string>("name");
      std::string type = temp->Get<std::string>("type");
      std::string value = temp->Get<std::string>();
      if (type == "double")
      {
        double v = std::stod(value);
        this->SetParam(name, v);
      }
      else if (type == "int")
      {
        int v = std::stoi(value);
        this->SetParam(name, v);
      }
      else if (type == "string" || type == "")
      {
        this->SetParam(name, value);
      }
      else if (type == "vector3")
      {
        math::Vector3 v = temp->Get<math::Vector3>();
        this->SetParam(name, v);
        // TODO
      }
      else if (type == "bool")
      {
        // TODO: except
        bool v = boost::lexical_cast<bool>(value);
        this->SetParam(name, v);
      }
    }
    this->ParamsFromSDFElement(temp);
  }
}

//////////////////////////////////////////////////
void PhysicsEngine::Load(sdf::ElementPtr _sdf)
{
  this->sdf->Copy(_sdf);

  this->realTimeUpdateRate =
      this->sdf->GetElement("real_time_update_rate")->Get<double>();
  this->targetRealTimeFactor =
      this->sdf->GetElement("real_time_factor")->Get<double>();
  this->maxStepSize =
      this->sdf->GetElement("max_step_size")->Get<double>();

  // Generalized parameter reading
  this->ParamsFromSDFElement(this->sdf);
}

//////////////////////////////////////////////////
void PhysicsEngine::Fini()
{
  this->world.reset();
  this->node->Fini();
}

//////////////////////////////////////////////////
PhysicsEngine::~PhysicsEngine()
{
  this->sdf->Reset();
  this->sdf.reset();
  delete this->physicsUpdateMutex;
  this->physicsUpdateMutex = NULL;
  this->responsePub.reset();
  this->requestSub.reset();
  this->node.reset();

  delete this->contactManager;
}

//////////////////////////////////////////////////
math::Vector3 PhysicsEngine::GetGravity() const
{
  return this->sdf->Get<math::Vector3>("gravity");
}

//////////////////////////////////////////////////
CollisionPtr PhysicsEngine::CreateCollision(const std::string &_shapeType,
                                            const std::string &_linkName)
{
  CollisionPtr result;
  LinkPtr link =
    boost::dynamic_pointer_cast<Link>(this->world->GetEntity(_linkName));

  if (!link)
    gzerr << "Unable to find link[" << _linkName << "]\n";
  else
    result = this->CreateCollision(_shapeType, link);

  return result;
}

//////////////////////////////////////////////////
double PhysicsEngine::GetUpdatePeriod()
{
  double updateRate = this->GetRealTimeUpdateRate();
  if (updateRate > 0)
    return 1.0/updateRate;
  else
    return 0;
}

//////////////////////////////////////////////////
ModelPtr PhysicsEngine::CreateModel(BasePtr _base)
{
  ModelPtr ret(new Model(_base));
  return ret;
}

//////////////////////////////////////////////////
double PhysicsEngine::GetTargetRealTimeFactor() const
{
  return this->targetRealTimeFactor;
}

//////////////////////////////////////////////////
double PhysicsEngine::GetRealTimeUpdateRate() const
{
  return this->realTimeUpdateRate;
}

//////////////////////////////////////////////////
double PhysicsEngine::GetMaxStepSize() const
{
  return this->maxStepSize;
}

//////////////////////////////////////////////////
void PhysicsEngine::SetTargetRealTimeFactor(double _factor)
{
  this->sdf->GetElement("real_time_factor")->Set(_factor);
  this->targetRealTimeFactor = _factor;
}

//////////////////////////////////////////////////
void PhysicsEngine::SetRealTimeUpdateRate(double _rate)
{
  this->sdf->GetElement("real_time_update_rate")->Set(_rate);
  this->realTimeUpdateRate = _rate;
}

//////////////////////////////////////////////////
void PhysicsEngine::SetMaxStepSize(double _stepSize)
{
  this->sdf->GetElement("max_step_size")->Set(_stepSize);
  this->maxStepSize = _stepSize;
}

//////////////////////////////////////////////////
void PhysicsEngine::SetAutoDisableFlag(bool /*_autoDisable*/)
{
}

//////////////////////////////////////////////////
void PhysicsEngine::SetMaxContacts(unsigned int /*_maxContacts*/)
{
}

//////////////////////////////////////////////////
void PhysicsEngine::OnRequest(ConstRequestPtr &/*_msg*/)
{
}

//////////////////////////////////////////////////
void PhysicsEngine::OnPhysicsMsg(ConstPhysicsPtr &_msg)
{
  if (_msg->has_gravity())
    this->SetGravity(msgs::Convert(_msg->gravity()));

  if (_msg->has_real_time_factor())
    this->SetTargetRealTimeFactor(_msg->real_time_factor());

  if (_msg->has_real_time_update_rate())
  {
    this->SetRealTimeUpdateRate(_msg->real_time_update_rate());
  }

  if (_msg->has_max_step_size())
  {
    this->SetMaxStepSize(_msg->max_step_size());
  }

  boost::any value;
  for (int i = 0; i < _msg->parameters_size(); i++)
  {
    if (ConvertMessageParam(_msg->parameters(i), value))
    {
      this->SetParam(_msg->parameters(i).name(), value);
    }
    else
    {
      gzerr << "Couldn't set parameter from msg: "
            << _msg->parameters(i).name() << std::endl;
    }
    if (_msg->parameters(i).children_size() > 0)
    {
       this->SetFromGenericMsgParams(_msg->parameters(i));
    }
  }

  this->world->GetPresetManager()->CurrentProfile(_msg->profile_name());
}

//////////////////////////////////////////////////
void PhysicsEngine::SetFromGenericMsgParams(const gazebo::msgs::Param &_msg)
{
  for (int i = 0; i < _msg.children_size(); i++)
  {
    boost::any value;
    if (ConvertMessageParam(_msg.children(i), value))
    {
      this->SetParam(_msg.children(i).name(), value);
    }
    else
    {
      gzerr << "Couldn't set parameter from msg: "
            << _msg.children(i).name() << std::endl;
    }
    SetFromGenericMsgParams(_msg.children(i));
  }
}

//////////////////////////////////////////////////
bool PhysicsEngine::SetParam(const std::string &_key,
    const boost::any &_value)
{
  try
  {
    if (_key == "type")
    {
      // Cannot set physics engine type from SetParam
      return false;
    }
    if (_key == "max_step_size")
      this->SetMaxStepSize(boost::any_cast<double>(_value));
    else if (_key == "real_time_update_rate")
      this->SetRealTimeUpdateRate(boost::any_cast<double>(_value));
    else if (_key == "real_time_factor")
      this->SetTargetRealTimeFactor(boost::any_cast<double>(_value));
    else if (_key == "gravity")
    {
      boost::any copy = _value;
      if (_value.type() == typeid(sdf::Vector3))
      {
        copy = boost::lexical_cast<math::Vector3>
            (boost::any_cast<sdf::Vector3>(_value));
      }
      this->SetGravity(boost::any_cast<math::Vector3>(copy));
    }
    else if (_key == "magnetic_field")
    {
      boost::any copy = _value;
      if (_value.type() == typeid(sdf::Vector3))
      {
        copy = boost::lexical_cast<math::Vector3>
            (boost::any_cast<sdf::Vector3>(_value));
      }
      this->sdf->GetElement("magnetic_field")->
          Set(boost::any_cast<math::Vector3>(copy));
    }
    else
    {
      gzwarn << "SetParam failed for [" << _key << "] in physics engine "
             << this->GetType() << std::endl;
      return false;
    }
  }
  catch(boost::bad_any_cast &_e)
  {
    gzerr << "Caught bad any_cast in PhysicsEngine::SetParam: " << _e.what()
          << std::endl;
    return false;
  }
  catch(boost::bad_lexical_cast &_e)
  {
    gzerr << "Caught bad lexical_cast in PhysicsEngine::SetParam: " << _e.what()
          << std::endl;
    return false;
  }
  return true;
}

//////////////////////////////////////////////////
boost::any PhysicsEngine::GetParam(const std::string &/*_key*/) const
{
  return 0;
}

//////////////////////////////////////////////////
bool PhysicsEngine::GetParam(const std::string &_key,
    boost::any &_value) const
{
  if (_key == "type")
    _value = this->GetType();
  else if (_key == "max_step_size")
    _value = this->GetMaxStepSize();
  else if (_key == "real_time_update_rate")
    _value = this->GetRealTimeUpdateRate();
  else if (_key == "real_time_factor")
    _value = this->GetTargetRealTimeFactor();
  else if (_key == "gravity")
    _value = this->GetGravity();
  else if (_key == "magnetic_field")
    _value = this->sdf->Get<math::Vector3>("magnetic_field");
  else
  {
    gzwarn << "GetParam failed for [" << _key << "] in physics engine "
           << this->GetType() << std::endl;
    return false;
  }

  return true;
}

//////////////////////////////////////////////////
ContactManager *PhysicsEngine::GetContactManager() const
{
  return this->contactManager;
}

//////////////////////////////////////////////////
sdf::ElementPtr PhysicsEngine::GetSDF() const
{
  return this->sdf;
}
