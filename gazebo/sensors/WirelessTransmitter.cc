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
#include "gazebo/physics/physics.hh"
#include "gazebo/sensors/SensorFactory.hh"
#include "gazebo/sensors/WirelessTransmitter.hh"
#include "gazebo/transport/Node.hh"
#include "gazebo/transport/Publisher.hh"

using namespace gazebo;
using namespace sensors;
using namespace physics;

GZ_REGISTER_STATIC_SENSOR("wireless_transmitter", WirelessTransmitter)

const double WirelessTransmitter::NEmpty = 6;
const double WirelessTransmitter::NObstacle = 12.0;
const double WirelessTransmitter::ModelStdDesv = 6.0;
const double WirelessTransmitter::Step = 0.25;
const double WirelessTransmitter::MaxRadius = 10.0;

/////////////////////////////////////////////////
WirelessTransmitter::WirelessTransmitter()
: WirelessTransceiver()
{
  this->active = false;
  this->visualize = false;
  this->essid = "MyESSID";
  this->freq = 2442.0;
}

/////////////////////////////////////////////////
WirelessTransmitter::~WirelessTransmitter()
{
  this->testRay.reset();
}

/////////////////////////////////////////////////
void WirelessTransmitter::Load(const std::string &_worldName)
{
  WirelessTransceiver::Load(_worldName);

  this->parentEntity = boost::dynamic_pointer_cast<physics::Link>(
      this->world->GetEntity(this->parentName));

  if (!parentEntity)
  {
    gzthrow("WirelessTransmitter has invalid parent [" + this->parentName +
            "]. Must be a link\n");
  }

  this->referencePose = this->pose + this->parentEntity->GetWorldPose();

  this->visualize = this->sdf->Get<bool>("visualize");
  this->essid = transceiverElem->Get<std::string>("essid");
  this->freq = transceiverElem->Get<double>("frequency");

  if (this->essid.empty())
  {
    gzerr << "Wireless transmitter ESSID must be a non-empty string.\n";
    return;
  }

  if (this->freq < 0)
  {
    gzerr << "Wireless transmitter frequency must be > 0. Current value is ["
      << this->freq << "]\n";
    return;
  }

  this->pub = this->node->Advertise<msgs::PropagationGrid>(this->GetTopic(),
        30);
}

//////////////////////////////////////////////////
void WirelessTransmitter::Init()
{
  Sensor::Init();

  // This ray will be used in GetSignalStrength() for checking obstacles
  // between the transmitter and a given point.
  this->testRay = boost::dynamic_pointer_cast<RayShape>(
      world->GetPhysicsEngine()->CreateShape("ray", CollisionPtr()));
}

//////////////////////////////////////////////////
void WirelessTransmitter::UpdateImpl(bool /*_force*/)
{
  if (this->pub && this->visualize)
  {
    msgs::PropagationGrid msg;
    math::Pose pos;
    math::Pose worldPose;
    double strength;
    msgs::PropagationParticle *p;

    this->referencePose = this->pose + this->parentEntity->GetWorldPose();

    // Iterate using a rectangular grid, but only choose the points within
    // a circunference of radius MaxRadius
    for (double x = -this->MaxRadius; x <= this->MaxRadius; x += this->Step)
    {
      for (double y = -this->MaxRadius; y <= this->MaxRadius; y += this->Step)
      {
        pos.Set(x, y, 0.0, 0, 0, 0);

        worldPose = pos + this->referencePose;

        if (this->referencePose.pos.Distance(worldPose.pos) <= this->MaxRadius)
        {
          // For the propagation model assume the receiver antenna has the same
          // gain as the transmitter
          strength = this->GetSignalStrength(worldPose, this->GetGain());

          // Add a new particle to the grid
          p = msg.add_particle();
          p->set_x(x);
          p->set_y(y);
          p->set_signal_level(strength);
        }
      }
    }
    this->pub->Publish(msg);
  }
}

/////////////////////////////////////////////////
std::string WirelessTransmitter::GetESSID() const
{
  return this->essid;
}

/////////////////////////////////////////////////
double WirelessTransmitter::GetFreq() const
{
  return this->freq;
}

/////////////////////////////////////////////////
double WirelessTransmitter::GetSignalStrength(const math::Pose &_receiver,
    const double rxGain)
{
  std::string entityName;
  double dist;
  math::Vector3 end = _receiver.pos;
  math::Vector3 start = this->referencePose.pos;

  // Acquire the mutex for avoiding race condition with the physics engine
  boost::recursive_mutex::scoped_lock lock(*(world->GetPhysicsEngine()->
      GetPhysicsUpdateMutex()));

  // Compute the value of n depending on the obstacles between Tx and Rx
  double n = NEmpty;

  // Looking for obstacles between start and end points
  this->testRay->SetPoints(start, end);
  this->testRay->GetIntersection(dist, entityName);

  // ToDo: The ray intersects with my own collision model. Fix it.
  if (dist > 0 && entityName != "ground_plane::link::collision" &&
      entityName != "")
  {
    n = NObstacle;
  }

  double distance = std::max(1.0,
      this->referencePose.pos.Distance(_receiver.pos));
  double x = abs(math::Rand::GetDblNormal(0.0, ModelStdDesv));
  double wavelength = common::SpeedOfLight / (this->GetFreq() * 1000000);

  // Hata-Okumara propagation model
  double rxPower = this->GetPower() + this->GetGain() + rxGain - x +
      20 * log10(wavelength) - 20 * log10(4 * M_PI) - 10 * n * log10(distance);

  return rxPower;
}
