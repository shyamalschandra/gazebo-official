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
/* Desc: Laser Interface for Player
 * Author: Nate Koenig
 * Date: 2 March 2006
 * SVN: $Id: LaserInterface.hh 226 2008-01-16 19:01:23Z natepak $
 */

#ifndef IRINTERFACE_HH
#define IRINTERFACE_HH

#include "GazeboInterface.hh"

namespace boost
{
  class recursive_mutex;
}

namespace gazebo
{

/// \addtogroup player_iface Interfaces
/// \{
/// \defgroup laser_player Laser Interface
/// \brief Plugin Player interface for a Gazebo laser
/// \{

// Forward declarations
class IRIface;

/// \brief Plugin Player interface for a Gazebo laser
class IRInterface : public GazeboInterface
{
  /// \brief Constructor
  /// \param addr Plaer device address
  /// \param driver The Gazebo driver
  /// \param cf Player config file
  /// \param section Section of the config
  public: IRInterface(player_devaddr_t addr, GazeboDriver *driver,
                              ConfigFile *cf, int section);

  /// \brief Destructor
  public: virtual ~IRInterface();

  /// \brief Handle all messages. This is called from GazeboDriver
  /// \param respQueue Response queue
  /// \param hdr Message header
  /// \param data Pointer to the message data
  public: virtual int ProcessMessage(QueuePointer &respQueue,
                                     player_msghdr_t *hdr, void *data);

  /// \brief Update this interface, publish new info.
  public: virtual void Update();

  /// \brief Open a SHM interface when a subscription is received.
  ///        This is called fromGazeboDriver::Subscribe
  public: virtual void Subscribe();

  /// \brief Close a SHM interface. This is called from
  ///        GazeboDriver::Unsubscribe
  public: virtual void Unsubscribe();

  /// \brief The gazebo laser interface
  private: IRIface *iface;

  /// \brief Gazebo id. This needs to match and ID in a Gazebo WorldFile
  private: char *gz_id;

  /// \brief Timestamp on last data update
  private: double datatime;


  private: player_ir_data_t data;
  private: static boost::recursive_mutex *mutex;
};

/// \} 
/// \} 


}
#endif
