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
/* Desc: Parameters for each update cycle
 * Author: Nate Koenig
 * Date: 8 May 2007
 * SVN: $Id$
 */

#ifndef UPDATEPARAMS_HH
#define UPDATEPARAMS_HH

namespace gazebo
{
/// \addtogroup gazebo_server
/// \brief Parameters used during update cycle
/// \{

/// \brief Parameters used during update cycle
class UpdateParams
{
  /// \brief Constructor
  public: UpdateParams();

  /// \brief Destructor
  public: virtual ~UpdateParams();

  /// \brief Elapsed time since last update
  public: double stepTime;
};

/// \}
}

#endif
