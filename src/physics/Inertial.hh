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
#ifndef INERTIAL_HH
#define INERTIAL_HH

#include "sdf/sdf.h"
#include "math/Quaternion.hh"
#include "math/Vector3.hh"

namespace gazebo
{
  namespace physics
  {
    /// \addtogroup gazebo_physics
    /// \{

    /// \brief A class for inertial information about a link
    class Inertial
    {
      /// \brief Default Constructor
      public: Inertial();
  
      /// \brief Constructor
      public: Inertial(double mass);
  
      /// \brief Copy constructor
      public: Inertial(const Inertial &_inertial);
  
      /// \brief Destructor
      public: virtual ~Inertial();
  
      public: void Load( sdf::ElementPtr _sdf);

      /// \brief update the parameters using new sdf values
      public: void UpdateParameters( sdf::ElementPtr &_sdf );

      public: double GetLinearDamping();
      public: double GetAngularDamping();

      /// \brief Reset all the mass properties
      public: void Reset();

      /// \brief Set the mass
      public: void SetMass(double m);

      /// \brief Get the mass
      public: double GetMass() const;
  
      /// \brief Set the mass matrix
      public: void SetInertiaMatrix( double ixx, double iyy, double izz,
                                     double ixy, double ixz, double iyz);
  
      /// \brief Set the center of gravity
      public: void SetCoG(double cx, double cy, double cz);
  
      /// \brief Set the center of gravity
      public: void SetCoG(const math::Vector3 &c);
  
      /// \brief Get the center of gravity
      public: inline const math::Vector3 &GetCoG() const
              { return this->cog; }

      public: inline const math::Pose GetPose() const
              { return math::Pose(this->cog, math::Quaternion());}
  
      /// \brief Get the prinicpal moments of inertia (Ixx, Iyy, Izz)
      public: math::Vector3 GetPrincipalMoments() const;
  
      /// \brief Get the products of inertia (Ixy, Ixy, Iyz)
      public: math::Vector3 GetProductsofInertia() const;

      public: double GetIXX() const;
      public: double GetIYY() const;
      public: double GetIZZ() const;
      public: double GetIXY() const;
      public: double GetIXZ() const;
      public: double GetIYZ() const;
  
      /// \brief Rotate this mass
      public: void Rotate(const math::Quaternion &rot);
  
      /// \brief Equal operator
      public: void operator=(const Inertial &_inertial);
  
      public: Inertial operator+(const Inertial &_inertial ) const;
      public: const Inertial &operator+=(const Inertial &_inertial );
  
      public: friend std::ostream &operator<<(std::ostream &out, 
                                              const gazebo::physics::Inertial &_inertial)
              {
                out << "Mass[" << _inertial.mass << "] CoG[" 
                    << _inertial.cog << "]";
                return out;
              }
 
      /// Mass properties of the object
      private: double mass;
      private: math::Vector3 cog;
      private: math::Vector3 principals;
      private: math::Vector3 products;
      private: sdf::ElementPtr sdf;
    };
    /// \}
  }
}

#endif
