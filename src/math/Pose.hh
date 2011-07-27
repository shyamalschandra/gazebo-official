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
/* Desc: External interfaces for Gazebo
 * Author: Nate Koenig
 * Date: 03 Apr 2007
 */

#ifndef POSE3D_HH
#define POSE3D_HH

#include <iostream>

#include "math/Vector3.hh"
#include "math/Quaternion.hh"

namespace gazebo
{
	namespace math
  {
    /// \addtogroup gazebo_math
    /// \{
    
    /// \brief Encapsulates a position and rotation in three space
    class Pose
    {
      /// \brief Default constructors
      public: Pose();
    
      /// \brief Constructor
      /// \param pos A position
      /// \param rot A rotation
      public: Pose( const Vector3 &pos, const Quaternion &rot);
    
      /// \brief Copy constructor
      /// \param pose Pose to copy
      public: Pose( const Pose &pose );
    
      /// \brief Destructor
      public: virtual ~Pose();
    
      /// \brief See if a pose is finite (e.g., not nan)
      public: bool IsFinite() const;
    
      /// \brief Fix any nan values
      public: void Correct();
    
      /// \brief Get the inverse of this pose
      public: Pose GetInverse() const;
    
      /// \brief Addition operator
      /// \param pose Pose to add to this pose
      /// \return The resulting pose
      public: Pose operator+(const Pose &pose) const;
    
      /// \brief Add-Equals operator
      /// \param pose Pose to add to this pose
      /// \return The resulting pose
      public: const Pose &operator+=(const Pose &pose);
    
      /// \brief Subtraction operator
      /// \param pose Pose to subtract from this one
      /// \return The resulting pose
      public: Pose operator-(const Pose &pose) const;
    
      /// \brief Subtraction operator
      /// \param pose Pose to subtract from this one
      /// \return The resulting pose
      public: const Pose &operator-=(const Pose &_pose);

      /// \brief Equality operator
      /// \param _pose Pose for comparison
      /// \return True if equal
      public: bool operator==(const Pose &_pose) const;

      /// \brief Inequality operator
      /// \param _pose Pose for comparison
      /// \return True if not equal
      public: bool operator!=(const Pose &_pose) const;
    
      /// \brief Multiplication operator
      public: Pose operator*(const Pose &pose);
              
      /// \brief Add one point to a vector: result = this + pos
      /// \param pos Position to add to this pose
      /// \return The resulting position
      public: Vector3 CoordPositionAdd(const Vector3 &pos) const;
    
      /// \brief Add one point to another: result = this + pose
      /// \param pose The Pose to add
      /// \return The resulting position
      public: Vector3 CoordPositionAdd(const Pose &pose) const;
    
      /// \brief Subtract one position from another: result = this - pose
      /// \param pose Pose to subtract
      /// \return The resulting position
      public: Vector3 CoordPositionSub(const Pose &pose) const;
    
      /// \brief Add one rotation to another: result =  this->rot + rot
      /// \param rot Rotation to add
      /// \return The resulting rotation
      public: Quaternion CoordRotationAdd(const Quaternion &rot) const;
    
      /// \brief Subtract one rotation from another: result = this->rot - rot
      /// \param rot The rotation to subtract
      /// \return The resulting rotation
      public: Quaternion CoordRotationSub(const Quaternion &rot) const;
    
      /// \brief Find the inverse of a pose; i.e., if b = this + a, given b and 
      ///        this, find a
      public: Pose CoordPoseSolve(const Pose &b) const;
    
      /// \brief Reset the pose
      public: void Reset();
    
      /// \brief Rotate vector part of a pose about the origin
      public: Pose RotatePositionAboutOrigin(const Quaternion &rot) const;
    
      /// \brief The position
      public: Vector3 pos;
    
      /// \brief The rotation
      public: Quaternion rot;
    
      /// \brief Ostream operator
      /// \param out Ostream
      /// \param pose Pose to output
      /// \return the Ostream
      public: friend std::ostream &operator<<(std::ostream &out, 
                                              const gazebo::math::Pose &pose)
              {
                out << pose.pos << " " << pose.rot;
                return out;
              }

    public: friend std::istream &operator>>( std::istream &in, 
                gazebo::math::Pose &pose )
            {
              // Skip white spaces
              in.setf( std::ios_base::skipws );
              in >> pose.pos >> pose.rot;
              return in;
            }
    };
    
    /// \}
  }

}
#endif

