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
/* Desc: Parameters for contact joints
 * Author: Nate Koenig
 * Date: 30 July 2003
 * SVN: $Id$
 */

#ifndef CONTACTPARAMS_HH
#define CONTACTPARAMS_HH

namespace gazebo
{
  class Geom;

  class XMLConfigNode;

  /// \brief Surface params
  class SurfaceParams
  {
    /// Constructor
    public: SurfaceParams();
  
    /// \brief Load the contact params
    public: void Load(XMLConfigNode *node);
 
    /// Spring constant
    public: double kp;   
  
    /// Damping constant
    public: double kd;
  
    /// 0..1, 0=no bounciness
    public: double bounce;
    
    /// first coefficient of friction 
    public: double mu1;
  
    /// second coefficient of friction 
    public: double mu2;
  
    /// Force-dependent-slip direction 1
    public: double slip1;
  
    /// Force-dependent-slip direction 2
    public: double slip2;   
  
    /// \brief bounce vel
    public: double bounceVel;
  
    /// \brief friction direction for mu1
    ///        defined in the coordinate system of the body
    ///        true fdir1 is taken as a vector perpendicular to contact normal
    ///        in the plane spanned by contact normal and fdir1 provided here
    ///        in the event, friciton normal is parallel to fdir1 provided here
    ///        mu1 is found by uisng body frame axis x, y or z as fdir1 in the
    ///        order of x, y then z
    public: Vector3 fdir1;

    /// \brief soft constraint force mixing
    public: double softCfm;

    public: bool enableFriction;

  };
}

#endif
