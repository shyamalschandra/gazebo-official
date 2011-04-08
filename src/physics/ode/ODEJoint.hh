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
/* Desc: The ODE base joint class
 * Author: Nate Keonig, Andrew Howard
 * Date: 12 Oct 2009
 * SVN: $Id$
 */

#ifndef ODEJOINT_HH
#define ODEJOINT_HH

#include "physics/ode/ODEPhysics.hh"
#include "physics/Joint.hh"

namespace gazebo
{
	namespace physics
  {
    /// \brief ODE joint interface
    class ODEJoint : public Joint
    {
      /// \brief Constructor
      public: ODEJoint();
    
      /// \brief Destructor
      public: virtual ~ODEJoint();
  
      /// \brief Load a joint
      public: virtual void Load(common::XMLConfigNode *node);
  
      /// \brief Get the body to which the joint is attached according the _index
      public: virtual BodyPtr GetJointBody( int index ) const;
  
      /// \brief Determines of the two bodies are connected by a joint
      public: bool virtual AreConnected( BodyPtr one, BodyPtr two ) const;
  
      /// \brief The default function does nothing. This should be overriden in 
      ///        the child classes where appropriate
      public: virtual double GetParam( int /*parameter*/ ) const;
  
      /// \brief Attach the two bodies with this joint
      public: virtual void Attach( BodyPtr one, BodyPtr two );
  
      /// \brief Detach this joint from all bodies
      public: virtual void Detach();
  
      /// \brief By default this does nothing. It should be overridden in child 
      ///        classes where appropriate
      public: virtual void SetParam(int /*parameter*/, double /*value*/);
  
      /// \brief Set the ERP of this joint
      public: void SetERP(double newERP);
  
      /// \brief Get the ERP of this joint
      public: double GetERP();
  
      /// \brief Set the CFM of this joint
      public: void SetCFM(double newCFM);
  
      /// \brief Get the ERP of this joint
      public: double GetCFM();
  
      /// \brief Get the feedback data structure for this joint, if set
      public: dJointFeedback *GetFeedback();
  
      /// \brief Set the high stop of an axis(index).
      public: virtual void SetHighStop(int index, common::Angle angle);
  
      /// \brief Set the low stop of an axis(index).
      public: virtual void SetLowStop(int index, common::Angle angle);
   
      /// \brief Get the high stop of an axis(index).
      public: virtual common::Angle GetHighStop(int index);
  
      /// \brief Get the low stop of an axis(index).
      public: virtual common::Angle GetLowStop(int index);
  
      /// \brief Get the force the joint applies to the first body
      /// \param index The index of the body( 0 or 1 )
      public: virtual common::Vector3 GetBodyForce(unsigned int index) const;
  
      /// \brief Get the torque the joint applies to the first body
      /// \param index The index of the body( 0 or 1 )
      public: virtual common::Vector3 GetBodyTorque(unsigned int index) const;
  
      /// \brief Set a parameter for the joint
      public: virtual void SetAttribute( Attribute, int index, double value);
   
      /// This is our id
      protected: dJointID jointId;
    
      /// Feedback data for this joint
      private: dJointFeedback *feedback;
    
    };
  }
}
#endif
