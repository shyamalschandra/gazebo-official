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
/* Desc: Position Interface for Player
 * Author: Nate Koenig
 * Date: 2 March 2006
 * CVS: $Id$
 */

/**
@addtogroup player
@par Power Interface
*/

#include <math.h>

#include "gz.h"
#include "GazeboDriver.hh"
#include "PowerInterface.hh"

///////////////////////////////////////////////////////////////////////////////
// Constructor
PowerInterface::PowerInterface(player_devaddr_t addr,
                               GazeboDriver *driver, ConfigFile *cf, int section)
    : GazeboInterface(addr, driver, cf, section)
{
  // Get the ID of the interface
  this->gz_id = (char*) calloc(1024, sizeof(char));
  strcat(this->gz_id, GazeboClient::prefixId);
  strcat(this->gz_id, cf->ReadString(section, "gz_id", ""));

  // Allocate a Position Interface
  this->iface = gz_power_alloc();

  this->datatime = -1;
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
PowerInterface::~PowerInterface()
{
  // Release this interface
  gz_power_free(this->iface);
}

///////////////////////////////////////////////////////////////////////////////
// Handle all messages. This is called from GazeboDriver
int PowerInterface::ProcessMessage(QueuePointer &respQueue,
                                   player_msghdr_t *hdr, void *data)
{
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Update this interface, publish new info. This is
// called from GazeboDriver::Update
void PowerInterface::Update()
{
  player_power_data_t data;
  struct timeval ts;

  gz_power_lock(this->iface, 1);

  // Only Update when new data is present
  if (this->iface->data->head.time > this->datatime)
  {
    this->datatime = this->iface->data->head.time;

    ts.tv_sec = (int) (this->iface->data->head.time);
    ts.tv_usec = (int) (fmod(this->iface->data->head.time, 1) * 1e6);

    data.percent = this->iface->data->levels[0];

    this->driver->Publish( this->device_addr, NULL,
                           PLAYER_MSGTYPE_DATA,
                           PLAYER_POWER_DATA_STATE,
                           (void*)&data, sizeof(data), &this->datatime );
  }

  gz_power_unlock(this->iface);
}


///////////////////////////////////////////////////////////////////////////////
// Open a SHM interface when a subscription is received. This is called from
// GazeboDriver::Subscribe
void PowerInterface::Subscribe()
{
  // Open the interface
  if (gz_power_open(this->iface, GazeboClient::client, this->gz_id) != 0)
  {
    printf("Error Subscribing to Gazebo Power Interface\n");
  }
}

///////////////////////////////////////////////////////////////////////////////
// Close a SHM interface. This is called from GazeboDriver::Unsubscribe
void PowerInterface::Unsubscribe()
{
  gz_power_close(this->iface);
}
