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
/* Desc: Vector 3
 * Author: Andrew Howard and Nate Koenig
 * Date: 4 Apr 2007
 * SVN: $Id$
 */

#include "math/Helpers.hh"
#include "math/Vector3.hh"

using namespace gazebo;
using namespace math;


////////////////////////////////////////////////////////////////////////////////
// Constructor
Vector3::Vector3()
    : x(0.0), y(0.0), z(0.0)
{
}

////////////////////////////////////////////////////////////////////////////////
// Constructor
Vector3::Vector3( const double &_x, const double &_y, const double &_z )
    : x(_x), y(_y), z(_z)
{
}

////////////////////////////////////////////////////////////////////////////////
// Copy Constructor
Vector3::Vector3( const Vector3 &_pt )
    : x(_pt.x), y(_pt.y), z(_pt.z)
{
}

////////////////////////////////////////////////////////////////////////////////
// Destructor
Vector3::~Vector3()
{
}

////////////////////////////////////////////////////////////////////////////////
// Calc distance to the given point
double Vector3::Distance(const Vector3 &pt ) const
{
  return sqrt((this->x-pt.x)*(this->x-pt.x) + (this->y-pt.y)*(this->y-pt.y) + (this->z-pt.z)*(this->z-pt.z));
}

////////////////////////////////////////////////////////////////////////////////
// Returns the length (magnitude) of the vector
double Vector3::GetLength() const
{
  return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}

////////////////////////////////////////////////////////////////////////////////
// Return the square of the length (magnitude) of the vector
double Vector3::GetSquaredLength() const
{
  return this->x * this->x + this->y * this->y + this->z * this->z;
}

////////////////////////////////////////////////////////////////////////////////
// Normalize the vector length
void Vector3::Normalize()
{
  double d = sqrt(this->x * this->x + this->y * this->y + this->z * this->z);

  if (d != 0.0)
  {
    this->x /= d;
    this->y /= d;
    this->z /= d;
  }
}

////////////////////////////////////////////////////////////////////////////////
/// Round to near whole number 
Vector3 Vector3::Round()
{
  this->x = nearbyint(this->x);
  this->y = nearbyint(this->y);
  this->z = nearbyint(this->z);
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
/// Get a rounded version of this vector
Vector3 Vector3::GetRounded() const
{
  Vector3 result = *this;
  result.Round();
  return result;
}

////////////////////////////////////////////////////////////////////////////////
// Set the contents of the vector
void Vector3::Set(double x, double y, double z)
{
  this->x = x;
  this->y = y;
  this->z = z;
}

////////////////////////////////////////////////////////////////////////////////
Vector3 Vector3::GetCrossProd(const Vector3 &pt) const
{
  Vector3 c(0,0,0);

  c.x = this->y * pt.z - this->z * pt.y;
  c.y = this->z * pt.x - this->x * pt.z;
  c.z = this->x * pt.y - this->y * pt.x;

  return c;
}

////////////////////////////////////////////////////////////////////////////////
/// Return the dot product of this vector and pt
double Vector3::GetDotProd(const Vector3 &pt) const
{
  return this->x * pt.x + this->y * pt.y + this->z * pt.z;
}

////////////////////////////////////////////////////////////////////////////////
/// Get the absolute value of the vector
Vector3 Vector3::GetAbs() const
{
  return Vector3(fabs(this->x), fabs(this->y), fabs(this->z));
}

////////////////////////////////////////////////////////////////////////////////
/// Return a vector that is perpendicular to this one.
Vector3 Vector3::GetPerpendicular() const
{
  static const double sqrZero = 1e-06 * 1e-06;

  Vector3 perp = this->GetCrossProd( Vector3(1,0,0) );

  // Check the length of the vector
  if (perp.GetSquaredLength() < sqrZero)
  {
    perp = this->GetCrossProd( Vector3(0,1,0) );
  }

  return perp;
}

////////////////////////////////////////////////////////////////////////////////
/// Get a normal vector to a triangle
Vector3 Vector3::GetNormal(const Vector3 &v1, const Vector3 &v2, 
                           const Vector3 &v3)
{
  Vector3 a = v2 - v1;
  Vector3 b = v3 - v1;
  Vector3 n = a.GetCrossProd(b);
  //n.Normalize();
  return n;
}

////////////////////////////////////////////////////////////////////////////////
// Get a distance to a plane
double Vector3::GetDistToPlane(const Vector3 &_dir, 
                               const Vector3 &_planeNormal, double _d) const
{
  double denom = _planeNormal.GetDotProd(_dir);

  if (fabs(denom) < 1e-3)
  {
    // parallel
    return -1;
  }
  else
  {
    double nom = this->GetDotProd(_planeNormal) + _d;
    double t = -(nom/denom);
    return t;
  }
}

////////////////////////////////////////////////////////////////////////////////
/// Set this vector's components to the maximum of itself and the passed in 
/// vector
void Vector3::SetToMax(const Vector3 & v)
{
  if (v.x > this->x) this->x = v.x;
  if (v.y > this->y) this->y = v.y;
  if (v.z > this->z) this->z = v.z;
}

////////////////////////////////////////////////////////////////////////////////
/// Set this vector's components to the minimum of itself and the passed in 
/// vector
void Vector3::SetToMin(const Vector3 & v)
{
  if (v.x < this->x) this->x = v.x;
  if (v.y < this->y) this->y = v.y;
  if (v.z < this->z) this->z = v.z;
}


////////////////////////////////////////////////////////////////////////////////
// Equals operator
const Vector3 &Vector3::operator=( const Vector3 &pt )
{
  this->x = pt.x;
  this->y = pt.y;
  this->z = pt.z;

  return *this;
}

////////////////////////////////////////////////////////////////////////////////
/// Equal operator
const Vector3 &Vector3::operator=( double value )
{
  this->x = value;
  this->y = value;
  this->z = value;

  return *this;
}



////////////////////////////////////////////////////////////////////////////////
// Addition operator
Vector3 Vector3::operator+( const Vector3 &pt ) const
{
  return Vector3(this->x + pt.x, this->y + pt.y, this->z + pt.z);
}

const Vector3 &Vector3::operator+=( const Vector3 &pt )
{
  this->x += pt.x;
  this->y += pt.y;
  this->z += pt.z;

  return *this;
}

const Vector3 &Vector3::operator-=( const Vector3 &pt )
{
  this->x -= pt.x;
  this->y -= pt.y;
  this->z -= pt.z;

  return *this;
}


////////////////////////////////////////////////////////////////////////////////
// Division operators

const Vector3 Vector3::operator/( const Vector3 &pt ) const
{
  return Vector3(this->x / pt.x, this->y / pt.y, this->z / pt.z);
}

const Vector3 &Vector3::operator/=( const Vector3 &pt )
{
  this->x /= pt.x;
  this->y /= pt.y;
  this->z /= pt.z;

  return *this;
}

const Vector3 Vector3::operator/( double v ) const
{
  return Vector3(this->x / v, this->y / v, this->z / v);
}

const Vector3 &Vector3::operator/=( double v )
{
  this->x /= v;
  this->y /= v;
  this->z /= v;

  return *this;
}



////////////////////////////////////////////////////////////////////////////////
// Mulitplication operators
const Vector3 Vector3::operator*( const Vector3 &pt ) const
{
  return Vector3(this->x * pt.x, this->y * pt.y, this->z * pt.z);
}

const Vector3 &Vector3::operator*=( const Vector3 &pt )
{
  this->x *= pt.x;
  this->y *= pt.y;
  this->z *= pt.z;

  return *this;
}

const Vector3 Vector3::operator*( double v ) const
{
  return Vector3(this->x * v, this->y * v, this->z * v);
}

const Vector3 &Vector3::operator*=( double v)
{
  this->x *= v;
  this->y *= v;
  this->z *= v;

  return *this;
}

////////////////////////////////////////////////////////////////////////////////
// Equality operator
bool Vector3::operator==( const Vector3 &_pt ) const
{
  return equal(this->x, _pt.x,0.001) && 
         equal(this->y, _pt.y,0.001) && 
         equal(this->z, _pt.z,0.001);
}

////////////////////////////////////////////////////////////////////////////////
// Inequality operator
bool Vector3::operator!=( const Vector3 &pt ) const
{
  return !(*this == pt);
}

////////////////////////////////////////////////////////////////////////////////
// See if a point is finite (e.g., not nan)
bool Vector3::IsFinite() const
{
  return finite(this->x) && finite(this->y) && finite(this->z);
}

////////////////////////////////////////////////////////////////////////////////
/// [] operator
double Vector3::operator[](unsigned int index) const
{
  switch (index)
  {
    case 0:
      return this->x;
    case 1:
      return this->y;
    case 2:
      return this->z;
    default:
      return 0;
  }
}

/// Round all values to _decimalPlaces
void Vector3::Round(int _precision)
{
  this->x = precision(this->x, _precision);
  this->y = precision(this->y, _precision);
  this->z = precision(this->z, _precision);
}

/// Returns true if the two vectors are exacatly equal 
bool Vector3::Equal( const Vector3 &_v ) const
{
  return this->x == _v.x && this->y == _v.y && this->z == _v.z;
}


