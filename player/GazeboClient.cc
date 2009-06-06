/*
 *  Gazebo - Outdoor Multi-Robot Simulator
 *  Copyright (C) 2003
 *     Nate Koenig & Andrew Howard
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/* Desc: Gazebo (simulator) client functions
 * Author: Nate Koenig, Andrew Howard
 * Date: 2 March 2006
 * CVS: $Id$
 */

#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <libplayercore/playercore.h>

#include "GazeboTime.hh"
#include "GazeboClient.hh"

using namespace gazebo;

Client *GazeboClient::client = NULL;
SimulationIface *GazeboClient::sim = NULL;
const char *GazeboClient::prefixId = "";

extern PlayerTime* GlobalTime;

////////////////////////////////////////////////////////////////////////////////
// Initialize
void GazeboClient::Init(int serverid, const char *prefixid)
{

  if (prefixid != NULL)
    GazeboClient::prefixId = prefixid;

  GazeboClient::client = new Client();

  try
  {
  GazeboClient::client->ConnectWait(serverid, GZ_CLIENT_ID_PLAYER);
  }
  catch( std::string e )
  {
    std::cerr << "Error[" << e << "]\n";
    exit(0);
  }

  GazeboClient::sim = new SimulationIface();

  try
  {
    GazeboClient::sim->Open( GazeboClient::client, "default");
  }
  catch (std::string e)
  {
    std::cout << "Error: " << e << "\n";
    exit(0);
  }

  // steal the global clock - a bit aggressive, but a simple approach
  if (GlobalTime)
  {
    delete GlobalTime;
    GlobalTime = NULL;
  }

  GlobalTime = new GazeboTime();
  assert(GlobalTime != 0);

}


////////////////////////////////////////////////////////////////////////////////
// Finalize
void GazeboClient::Fini()
{
  GazeboClient::sim->Close();

  GazeboClient::client->Disconnect();
}

