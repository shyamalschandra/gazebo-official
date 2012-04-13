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
/* Desc: Laser Interface for Player
 * Author: Nate Koenig
 * Date: 2 March 2006
 */

#include <math.h>
#include <iostream>

#include "transport/transport.h"
#include "GazeboDriver.hh"
#include "LaserInterface.hh"


/////////////////////////////////////////////////
LaserInterface::LaserInterface(player_devaddr_t _addr,
    GazeboDriver *_driver, ConfigFile *_cf, int _section)
: GazeboInterface(_addr, _driver, _cf, _section)
{
  this->datatime = -1;

  this->node = gazebo::transport::NodePtr(new gazebo::transport::Node());
  this->node->Init(this->worldName);
  this->laserName = _cf->ReadString(_section, "laser_name", "default");

  this->scanId = 0;
  memset(&this->data, 0, sizeof(this->data));
}

/////////////////////////////////////////////////
LaserInterface::~LaserInterface()
{
  player_laser_data_t_cleanup(&this->data);
}

/////////////////////////////////////////////////
int LaserInterface::ProcessMessage(QueuePointer &_respQueue,
    player_msghdr_t *_hdr, void *_data)
{
  int result = -1;

  // Is it a request to set the laser's config?
  if (Message::MatchMessage(_hdr, PLAYER_MSGTYPE_REQ,
                            PLAYER_LASER_REQ_SET_CONFIG,
                            this->device_addr))
  {
    // player_laser_config_t* plc = (player_laser_config_t*)_data;

    if (_hdr->size == sizeof(player_laser_config_t))
    {
      // TODO: Complete this
      this->driver->Publish(this->device_addr, _respQueue,
                            PLAYER_MSGTYPE_RESP_ACK,
                            PLAYER_LASER_REQ_SET_CONFIG);
      result = 0;
    }
    else
    {
      printf("config request len is invalid (%d != %d)",
          (int)_hdr->size, (int)sizeof(player_laser_config_t));
      result = -1;
    }
  }

  // Is it a request to get the laser's config?
  else if (Message::MatchMessage(_hdr, PLAYER_MSGTYPE_REQ,
                                 PLAYER_LASER_REQ_GET_CONFIG,
                                 this->device_addr))
  {
    if (_hdr->size == 0)
    {
      /* TODO:
      int intensity = 1;

      player_laser_config_t plc;
      memset(&plc, 0, sizeof(plc));

      plc.min_angle = this->iface->data->min_angle;
      plc.max_angle = this->iface->data->max_angle;
      plc.max_range = this->iface->data->max_range;
      plc.resolution = this->iface->data->res_angle;
      plc.range_res = this->iface->data->res_range;
      plc.intensity = intensity;

      this->driver->Publish(this->device_addr, _respQueue,
          PLAYER_MSGTYPE_RESP_ACK,
          PLAYER_LASER_REQ_GET_CONFIG,
          (void*)&plc, sizeof(plc), NULL);
          */
      result = 0;
    }
    else
    {
      printf("config request len is invalid (%d != %d)", (int)_hdr->size, 0);
      result = -1;
    }
  }
  else if (Message::MatchMessage(_hdr, PLAYER_MSGTYPE_REQ,
           PLAYER_LASER_REQ_GET_GEOM, this->device_addr))
  {
    player_laser_geom_t rep;

    // TODO: get geometry from somewhere
    /*memset(&rep.pose, 0, sizeof(rep.pose));
    memset(&rep.size, 0, sizeof(rep.size));

    rep.pose.px = this->iface->data->pose.pos.x;
    rep.pose.py = this->iface->data->pose.pos.y;
    rep.pose.pyaw = this->iface->data->pose.yaw;

    rep.size.sw = this->iface->data->size.x;
    rep.size.sl = this->iface->data->size.y;
        */

    this->driver->Publish(this->device_addr, _respQueue,
        PLAYER_MSGTYPE_RESP_ACK,
        PLAYER_LASER_REQ_GET_GEOM, &rep, sizeof(rep), NULL);
    result = 0;
  }

  return result;
}

/////////////////////////////////////////////////
void LaserInterface::Update()
{
}

/////////////////////////////////////////////////
void LaserInterface::Subscribe()
{
  std::string topic = "~/";
  topic += this->laserName + "/scan";
  boost::replace_all(topic,"::","/");

  this->laserScanSub = this->node->Subscribe(topic,
      &LaserInterface::OnScan, this);
}

/////////////////////////////////////////////////
void LaserInterface::Unsubscribe()
{
  this->laserScanSub->Unsubscribe();
  this->laserScanSub.reset();
}

/////////////////////////////////////////////////
void LaserInterface::OnScan(ConstLaserScanPtr &_msg)
{
  int i;
  float rangeRes;
  float angleRes;

  //TODO: fix the time to get the time the laser scan was generated
  this->datatime = gazebo::common::Time::GetWallTime().Double();

  double oldCount = this->data.ranges_count;

  this->data.min_angle = _msg->angle_min();
  this->data.max_angle = _msg->angle_max();
  this->data.resolution = _msg->angle_step();
  this->data.max_range = _msg->range_max();
  this->data.ranges_count =
    this->data.intensity_count = _msg->ranges_size();
  this->data.id = this->scanId++;

  if (oldCount != this->data.ranges_count)
  {
    delete [] this->data.ranges;
    delete [] this->data.intensity;

    this->data.ranges = new float[data.ranges_count];
    this->data.intensity = new uint8_t[data.intensity_count];
  }

  for (i = 0; i < _msg->ranges_size(); ++i)
    this->data.ranges[i] = _msg->ranges(i);

  for (i = 0; i < _msg->intensities_size(); ++i)
    this->data.intensity[i] = (uint8_t)_msg->intensities(i);

  if (this->data.ranges_count > 0)
  {
    this->driver->Publish(this->device_addr,
        PLAYER_MSGTYPE_DATA,
        PLAYER_LASER_DATA_SCAN,
        (void*)&this->data, sizeof(this->data), &this->datatime);
  }
}
