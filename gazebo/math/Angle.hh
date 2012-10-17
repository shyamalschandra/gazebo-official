/*
 * Copyright 2011 Nate Koenig
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
/* Desc: Angle class
 * Author: Nate Koenig
 * Date: 18 Aug 2008
 */

#ifndef ANGLE_HH
#define ANGLE_HH

#include <math.h>
#include <iostream>

/// \brief Macro that converts radians to degrees
/// \param[in] radians
/// \return degrees
#define GZ_RTOD(r) ((r) * 180 / M_PI)

/// \brief Converts degrees to radians
/// \param[in] degrees
/// \return radians
#define GZ_DTOR(d) ((d) * M_PI / 180)

/// \brief Macro tha normalizes an angle in the range -Pi to Pi
/// \param[in] angle
/// \return the angle, in range
#define GZ_NORMALIZE(a) (atan2(sin(a), cos(a)))

namespace gazebo
{
  /// \ingroup gazebo_math

  /// \brief Math namespace
  namespace math
  {
  /// \addtogroup gazebo_math Math
  /// \{

  /// \brief An angle and related functions.
  class Angle
  {
    /// \brief Constructor
    public: Angle();

    /// \brief Copy Constructor
    /// \param[in] _radian Radians
    public: Angle(double _radian);

    /// \brief Copy constructor
    /// \param[in] _angle Angle to copy
    public: Angle(const Angle &_angle);

    /// \brief Destructor
    public: virtual ~Angle();

    /// \brief Set the value from an angle in radians
    /// \param[in] _radian Radian value
    public: void SetFromRadian(double _radian);

    /// \brief Set the value from an angle in degrees
    /// \param[in] _degree Degree value
    public: void SetFromDegree(double _degree);

    /// \brief Get the angle in radians
    /// \return Double containing the angle's radian value
    public: double GetAsRadian() const __attribute__((deprecated));

    /// \brief Get the angle in radians
    /// \return Double containing the angle's radian value
    public: double Radian() const;

    /// \brief Get the angle in degrees
    /// \return Double containing the angle's degree value
    public: double GetAsDegree() const __attribute__((deprecated));

    /// \brief Get the angle in degrees
    /// \return Double containing the angle's degree value
    public: double Degree() const;

    /// \brief Normalize the angle in the range -Pi to Pi
    public: void Normalize();

    /// \brief Dereference operator
    /// \return Double containing the angle's radian value
    public: inline double operator*() const { return value; }
    /// \brief Substraction, result = this - _angle
    /// \param[in] _angle Angle for substraction
    /// \return The new angle
    public: Angle operator-(const Angle &_angle) const;

    /// \brief Addition operator, result = this + _angle
    /// \param[in] _angle Angle for addition
    /// \return The new angle
    public: Angle operator+(const Angle &_angle) const;

    /// \brief Multiplication operator, result = this * _angle
    /// \param[in] _angle Angle for multiplication
    /// \return The new angle
    public: Angle operator*(const Angle &_angle) const;

    /// \brief Division, result = this / _angle
    /// \param[in] _angle Angle for division
    /// \return The new angle
    public: Angle operator/(const Angle &_angle) const;

    /// \brief Subtraction set, this = this - _angle
    /// \param[in] _angle Angle for subtraction
    /// \return This angle
    public: Angle operator-=(const Angle &_angle);

    /// \brief Addition set, this = this + _angle
    /// \param[in] _angle Angle for addition
    /// \return This angle
    public: Angle operator+=(const Angle &_angle);

    /// \brief Multiplication set, this = this * _angle
    /// \param[in] _angle Angle for multiplication
    /// \return This angle
    public: Angle operator*=(const Angle &_angle);

    /// \brief Division set, this = this / _angle
    /// \param[in] _angle Angle for division
    /// \return This angle
    public: Angle operator/=(const Angle &_angle);

    /// \brief Equality operator, result = this == _angle
    /// \param[in] _angle Angle to check for equality
    /// \return True if this == _angle
    public: bool operator ==(const Angle &_angle) const;

    /// \brief Inequality
    /// \param[in] _angle Angle to check for inequality
    /// \return True if this != _angle
    public: bool operator!=(const Angle &_angle) const;

    /// \brief Less than operator
    /// \param[in] _angle Angle to check
    /// \return True if this < _angle
    public: bool operator<(const Angle &_angle) const;

    /// \brief Less or equal operator
    /// \param[in] _angle Angle to check
    /// \return True if this <= _angle
    public: bool operator<=(const Angle &_angle) const;

    /// \brief Greater than operator
    /// \param[in] _angle Angle to check
    /// \return True if this > _angle
    public: bool operator>(const Angle &_angle) const;

    /// \brief Greater or equal operator
    /// \param[in] _angle Angle to check
    /// \return True if this >= _angle
    public: bool operator>=(const Angle &_angle) const;

    /// \brief Stream insertion operator. Outputs in degrees
    /// \param[in] _out output stream
    /// \param[in] _a angle to output
    /// \return The Ostream
    public: friend std::ostream &operator<<(std::ostream &_out,
                                            const gazebo::math::Angle &_a)
    {
      _out << _a.Radian();
      return _out;
    }

    /// \brief Stream extraction operator. Assumes input is in degrees
    /// \param in input stream
    /// \param pt angle to read value into
    /// \return The input stream
    public: friend std::istream &operator>>(std::istream &_in,
                                            gazebo::math::Angle &_a)
    {
      // Skip white spaces
      _in.setf(std::ios_base::skipws);
      _in >> _a.value;
      return _in;
    }

    /// The angle in radians
    private: double value;
  };

  /// \}
  }
}

#endif



