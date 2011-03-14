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
/* Desc: A slider or primastic joint
 * Author: Nate Keonig, Andrew Howard
 * Date: 21 May 2003
 * CVS: $Id: SliderJoint.hh 7039 2008-09-24 18:06:29Z natepak $
 */

#ifndef ODESLIDERJOINT_HH
#define ODESLIDERJOINT_HH

#include "common/Param.hh"
#include "SliderJoint.hh"
#include "ODEJoint.hh"

namespace gazebo
{
	namespace physics
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
    class ODESliderJoint : public SliderJoint<ODEJoint>
    {
      /// \brief Constructor
      public: ODESliderJoint( dWorldID worldId );
    
      /// \brief Destructor
      public: virtual ~ODESliderJoint();
    
      /// \brief Load the joint
      protected: virtual void Load(XMLConfigNode *node);
  
      /// \brief Get the axis of rotation
      public: virtual Vector3 GetAxis(int index) const;
  
      /// \brief Set the axis of motion
      public: virtual void SetAxis( int index, const Vector3 &axis );
    
      /// \brief Set joint damping, not yet implemented
      public: virtual void SetDamping(int index, const double damping);
  
      /// \brief callback to apply damping force to joint
      public: void ApplyDamping();
  
      /// \brief Get the position of the joint
      public: virtual Angle GetAngle(int index) const;
  
      /// \brief Get the rate of change
      public: virtual double GetVelocity(int index) const;
  
      /// \brief Set the velocity of an axis(index).
      public: virtual void SetVelocity(int index, double angle);
  
      /// \brief Set the slider force
      public: virtual void SetForce(int index, double force);
  
      /// \brief Set the max allowed force of an axis(index).
      public: virtual void SetMaxForce(int index, double t);
  
      /// \brief Get the max allowed force of an axis(index).
      public: virtual double GetMaxForce(int index);
  
      /// \brief Get the _parameter
      public: virtual double GetParam( int parameter ) const;
  
      /// \brief Set the _parameter
      public: virtual void SetParam( int parameter, double value);
    };
    
  /// \}
  }
}
#endif
