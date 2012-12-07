/*
 * Copyright 2012 Open Source Robotics Foundation
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

#include <math.h>
#include "math/Helpers.hh"
#include "math/Angle.hh"

using namespace gazebo;
using namespace math;

//////////////////////////////////////////////////
Angle::Angle()
{
  this->value = 0;
}

//////////////////////////////////////////////////
Angle::Angle(double _radian)
{
  this->value = _radian;
}

//////////////////////////////////////////////////
Angle::Angle(const Angle &_angle)
{
  this->value = _angle.value;
}

//////////////////////////////////////////////////
Angle::~Angle()
{
}

//////////////////////////////////////////////////
void Angle::SetFromRadian(double _radian)
{
  this->value = _radian;
}

//////////////////////////////////////////////////
void Angle::SetFromDegree(double _degree)
{
  this->value = _degree * M_PI / 180.0;
}

//////////////////////////////////////////////////
double Angle::GetAsRadian() const
{
  return this->value;
}

//////////////////////////////////////////////////
double Angle::Radian() const
{
  return this->value;
}

//////////////////////////////////////////////////
double Angle::GetAsDegree() const
{
  return this->value * 180.0 / M_PI;
}

//////////////////////////////////////////////////
double Angle::Degree() const
{
  return this->value * 180.0 / M_PI;
}

//////////////////////////////////////////////////
void Angle::Normalize()
{
  this->value = atan2(sin(this->value), cos(this->value));
}

//////////////////////////////////////////////////
Angle Angle::operator-(const Angle &angle) const
{
  return Angle(this->value - angle.value);
}

//////////////////////////////////////////////////
Angle Angle::operator+(const Angle &angle) const
{
  return Angle(this->value + angle.value);
}

//////////////////////////////////////////////////
Angle Angle::operator*(const Angle &angle) const
{
  return Angle(this->value * angle.value);
}

//////////////////////////////////////////////////
Angle Angle::operator/(const Angle &angle) const
{
  return Angle(this->value / angle.value);
}

//////////////////////////////////////////////////
Angle Angle::operator-=(const Angle &angle)
{
  this->value -= angle.value;
  return *this;
}

//////////////////////////////////////////////////
Angle Angle::operator+=(const Angle &angle)
{
  this->value += angle.value;
  return *this;
}

//////////////////////////////////////////////////
Angle Angle::operator*=(const Angle &angle)
{
  this->value *= angle.value;
  return *this;
}

//////////////////////////////////////////////////
Angle Angle::operator/=(const Angle &angle)
{
  this->value /= angle.value;
  return *this;
}

//////////////////////////////////////////////////
bool Angle::operator ==(const Angle &angle) const
{
  return equal(this->value, angle.value);
}

//////////////////////////////////////////////////
bool Angle::operator!=(const Angle &angle) const
{
  return !(*this == angle);
}

//////////////////////////////////////////////////
bool Angle::operator<(const Angle &angle) const
{
  return this->value < angle.value;
}

//////////////////////////////////////////////////
bool Angle::operator<=(const Angle &angle) const
{
  return this->value < angle.value || math::equal(this->value, angle.value);
}

//////////////////////////////////////////////////
bool Angle::operator>(const Angle &angle) const
{
  return this->value > angle.value;
}

//////////////////////////////////////////////////
bool Angle::operator>=(const Angle &angle) const
{
  return this->value > angle.value || math::equal(this->value, angle.value);
}


