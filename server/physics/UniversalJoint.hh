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
/* Desc: A universal joint
 * Author: Nate Keonig, Andrew Howard
 * Date: 21 May 2003
 * CVS: $Id$
 */

#ifndef UNIVERSALJOINT_HH
#define UNIVERSALJOINT_HH

#include "Joint.hh"

namespace gazebo
{
/// \addtogroup gazebo_physics_joints
/// \{
/** \defgroup gazebo_universal_joint Universal Joint
 
  \brief A universal joint

  \par Attributes
  - body1 (string)
    - Name of the first body to attach to the joint
  - body2 (string)
    - Name of the second body to attach to the joint
  - anchor (string)
    - Name of the body which will act as the anchor to the joint
  - axis1 (float, tuple)
    - Defines the axis of rotation for the first degree of freedom
    - Default: 0 0 1
  - axis2 (float, tuple)
    - Defines the axis of rotation for the second degree of freedom
    - Default: 0 0 1
  - lowStop1 (float, degrees)
    - The low stop angle for the first degree of freedom
    - Default: infinity
  - highStop1 (float, degrees)
    - The high stop angle for the first degree of freedom
    - Default: infinity
  - lowStop2 (float, degrees)
    - The low stop angle for the second degree of freedom
    - Default: infinity
  - highStop2 (float, degrees)
    - The high stop angle for the second degree of freedom
    - Default: infinity
  - erp (double)
    - Error reduction parameter. 
    - Default = 0.4
  - cfm (double)
    - Constraint force mixing. 
    - Default = 0.8


  \par Example
  \verbatim
  <joint:universal name="universal_joint>
    <body1>body1_name</body1>
    <body2>body2_name</body2>
    <anchor>anchor_body</anchor>
    <axis1>0 0 1</axis1>
    <axis2>0 1 0</axis2>
    <lowStop1>0</lowStop1>
    <highStop1>30</highStop1>
    <lowStop2>0</lowStop2>
    <highStop2>30</highStop2>
  </joint:universal>
  \endverbatim
*/
/// \}

/// \defgroup gazebo_universal_joint Universal Joint
/// \{

/// \brief A universal joint
template<class T>
class UniversalJoint : public T
{
  /// \brief Constructor
  public: UniversalJoint() : T()
          {
            this->AddType(UNIVERSAL_JOINT);

            Param::Begin(&this->parameters);
            this->axis1P = new ParamT<Vector3>("axis1",Vector3(0,0,1),0);
            this->axis2P = new ParamT<Vector3>("axis2",Vector3(0,0,1),0);

            this->loStop1P = new ParamT<Angle>("lowStop1",-M_PI,0);
            this->hiStop1P = new ParamT<Angle>("highStop1",M_PI,0);
            this->loStop2P = new ParamT<Angle>("lowStop2",-M_PI,0);
            this->hiStop2P = new ParamT<Angle>("highStop2",M_PI,0);
            Param::End();
          }

  /// \brief Destuctor
  public: virtual ~UniversalJoint()
          {
            delete this->axis1P;
            delete this->axis2P;
            delete this->loStop1P;
            delete this->hiStop1P;
            delete this->loStop2P;
            delete this->hiStop2P;
          }

  /// \brief Load the joint
  protected: virtual void Load(XMLConfigNode *node)
             {
               this->axis1P->Load(node);
               this->axis2P->Load(node);

               this->loStop1P->Load(node);
               this->hiStop1P->Load(node);
               this->loStop2P->Load(node);
               this->hiStop2P->Load(node);

               T::Load(node);

               this->SetAxis(0,**(this->axis1P));
               this->SetAxis(1,**(this->axis1P));

               // Perform this three step ordering to ensure the parameters 
               // are set properly. This is taken from the ODE wiki.
               this->SetHighStop(0,**this->hiStop1P);
               this->SetLowStop(0,**this->loStop1P);
               this->SetHighStop(0,**this->hiStop1P);

               // Perform this three step ordering to ensure the parameters 
               // are set properly. This is taken from the ODE wiki.
               this->SetHighStop(1,**this->hiStop2P);
               this->SetLowStop(1,**this->loStop2P);
               this->SetHighStop(1,**this->hiStop2P);
             }

  /// \brief Save a joint to a stream in XML format
  protected: virtual void SaveJoint(std::string &prefix, std::ostream &stream)
             {
               T::SaveJoint(prefix, stream);

               stream << prefix << *(this->axis1P) << "\n";
               stream << prefix << *(this->loStop1P) << "\n";
               stream << prefix << *(this->hiStop1P) << "\n";

               stream << prefix << *(this->axis2P) << "\n";
               stream << prefix << *(this->loStop2P) << "\n";
               stream << prefix << *(this->hiStop2P) << "\n";
             }

  protected: ParamT<Vector3> *axis1P;
  protected: ParamT<Vector3> *axis2P;
  protected: ParamT<Angle> *loStop1P;
  protected: ParamT<Angle> *hiStop1P;
  protected: ParamT<Angle> *loStop2P;
  protected: ParamT<Angle> *hiStop2P;

};

/// \}
}
#endif

