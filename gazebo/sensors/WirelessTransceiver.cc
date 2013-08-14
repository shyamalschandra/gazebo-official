/*
 * Copyright 2013 Open Source Robotics Foundation
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

#include "gazebo/math/Rand.hh"
#include "gazebo/msgs/msgs.hh"
#include "gazebo/sensors/SensorFactory.hh"
#include "gazebo/sensors/SensorManager.hh"
#include "gazebo/physics/physics.hh"
#include "gazebo/sensors/WirelessTransceiver.hh"
#include "gazebo/transport/Node.hh"
#include "gazebo/transport/Publisher.hh"

using namespace gazebo;
using namespace sensors;

GZ_REGISTER_STATIC_SENSOR("Wireless_transceiver", WirelessTransceiver)

/////////////////////////////////////////////////
WirelessTransceiver::WirelessTransceiver()
  : Sensor(sensors::OTHER)
{
  this->active = false;
  this->gain = 2.5;
  this->power = 14.5;
}

/////////////////////////////////////////////////
WirelessTransceiver::~WirelessTransceiver()
{
}

//////////////////////////////////////////////////
std::string WirelessTransceiver::GetTopic() const
{
  std::string topicName = "~/";
  topicName += this->parentName + "/" + this->GetName() + "/transceiver";
  boost::replace_all(topicName, "::", "/");

  return topicName;
}

/////////////////////////////////////////////////
void WirelessTransceiver::Load(const std::string &_worldName)
{
  Sensor::Load(_worldName);

  this->entity = this->world->GetEntity(this->parentName);
  GZ_ASSERT(this->entity != NULL, "Unable to get the parent entity.");

  if (this->sdf->HasElement("transceiver"))
  {
    sdf::ElementPtr transElem = this->sdf->GetElement("transceiver");

    if (transElem->HasElement("gain"))
    {
      this->gain = transElem->Get<double>("gain");
    }

    if (transElem->HasElement("power"))
    {
      this->power = transElem->Get<double>("power");
    }
  }
}

//////////////////////////////////////////////////
void WirelessTransceiver::Init()
{
  Sensor::Init();
}

/////////////////////////////////////////////////
double WirelessTransceiver::GetPower() const
{
  return this->power;
}

/////////////////////////////////////////////////
double WirelessTransceiver::GetGain() const
{
  return this->gain;
}
