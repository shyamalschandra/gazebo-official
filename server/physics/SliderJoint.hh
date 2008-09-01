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
/* Desc: A slider or primastic joint
 * Author: Nate Keonig, Andrew Howard
 * Date: 21 May 2003
 * CVS: $Id$
 */

#ifndef SLIDERJOINT_HH
#define SLIDERJOINT_HH

#include "Param.hh"
#include "Joint.hh"

class JointGroup;

namespace gazebo
{

/// \addtogroup gazebo_physics_joints
/// \{
/** \defgroup gazebo_slider_joint Slider Joint
  
  \brief A slider joint

  \par Attributes
  - body1 (string)
    - Name of the first body to attach to the joint
  - body2 (string)
    - Name of the second body to attach to the joint
  - anchor (string)
    - Name of the body which will act as the anchor to the joint
  - axis (float, tuple)
    - Defines the axis of movement
    - Default: 0 0 1
  - lowStop (float, meters)
    - The low stop position
    - Default: infinity
  - highStop (float, meters)
    - The high stop position
    - Default: infinity
  - erp (double)
    - Error reduction parameter. 
    - Default = 0.4
  - cfm (double)
    - Constraint force mixing. 
    - Default = 0.8


  \par Example
  \verbatim
  <joint:slider name="slider_joint>
    <body1>body1_name</body1>
    <body2>body2_name</body2>
    <anchor>anchor_body</anchor>
    <axis>0 0 1</axis>
    <lowStop>0</lowStop>
    <highStop>30</highStop>
  </joint:slider>
  \endverbatim
*/
/// \}


/// \addtogroup gazebo_slider_joint Slider Joint
/// \{

  /// \brief A slider joint
  class SliderJoint : public Joint
  {
    /// \brief Constructor
    public: SliderJoint( dWorldID worldId );
  
    /// \brief Destructor
    public: virtual ~SliderJoint();
  
    /// \brief Load the joint
    protected: virtual void LoadChild(XMLConfigNode *node);
  
    /// \brief Save a joint to a stream in XML format
    protected: virtual void SaveChild(std::string &prefix, std::ostream &stream);
  
    /// \brief Get the axis of rotation
    public: Vector3 GetAxis() const;
  
    /// \brief Get the position of the joint
    public: double GetPosition() const;
  
    /// \brief Get the rate of change
    public: double GetPositionRate() const;
  
    /// \brief Get the _parameter
    public: virtual double GetParam( int parameter ) const;
  
    /// \brief Set the axis of motion
    public: void SetAxis( const Vector3 &axis );
  
    /// \brief Set the _parameter
    public: virtual void SetParam( int parameter, double value);
  
    /// \brief Set the anchor
    public: virtual void SetAnchor( const Vector3 &anchor) {}
  
    /// \brief Set the slider force
    public: void SetSliderForce(double force);
  
    private: ParamT<Vector3> *axisP;
    private: ParamT<double> *loStopP;
    private: ParamT<double> *hiStopP; 
  };
  
/// \}
}
#endif
