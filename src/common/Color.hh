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
/* Desc: Color class
 * Author: Nate Koenig
 * Date: 08 May 2009
 */

#ifndef COLOR_HH
#define COLOR_HH

#include <iostream>
#include "math/Vector3.hh"

namespace gazebo
{
  namespace common
  {
    /// \addtogroup gazebo_common Common 
    /// \{

    /// \brief Defines a color 
    class Color
    {
      /// \brief Constructor
      public: Color();

      /// \brief Constructor
      public: Color( float r, float g, float b, float a=1.0 );

      /// \brief Copy Constructor
      public: Color( const Color &clr );
    
      /// \brief Destructor
      public: virtual ~Color();

      /// \brief Reset the color to default values
      public: void Reset();
    
      /// \brief Set the contents of the vector
      public: void Set(float r = 1, float g =1 , float b = 1, float a = 1);

      /// \brief Get the color in HSV colorspace
      public: math::Vector3 GetAsHSV() const;
 
      /// \brief Set a color based on HSV values
      public: void SetFromHSV(float h, float s, float v);

      /// \brief Get the color in YUV colorspace
      public: math::Vector3 GetAsYUV() const;

      /// \brief Set from yuv
      public: void SetFromYUV(float y, float u, float v);

      /// \brief Equal operator
      public: const Color &operator=( const Color &pt );

      /// \brief Array index operator
      public: float operator[](unsigned int index);

      /// \brief Get the red color
      public: float R() const;

      /// \brief Get the green color
      public: float G() const;

      /// \brief Get the blue color
      public: float B() const;

      /// \brief Get the alpha color
      public: float A() const;

      /// \brief Set the red color
      public: void R(float r);

      /// \brief Set the green color
      public: void G(float g);

      /// \brief Set the blue color
      public: void B(float b);

      /// \brief Set the alpha color
      public: void A(float a);
   
      // Addition operators
      public: Color operator+( const Color &pt ) const;
      public: Color operator+( const float &v ) const;
      public: const Color &operator+=( const Color &pt );
    
      // Subtraction operators 
      public: Color operator-( const Color &pt ) const;
      public: Color operator-( const float &pt ) const;
      public: const Color &operator-=( const Color &pt );
    
      // Division operators
      public: const Color operator/( const float &i ) const;
      public: const Color operator/( const Color &pt ) const;
      public: const Color &operator/=( const Color &pt );
    
      // Multiplication operators
      public: const Color operator*(const float &i) const;
      public: const Color operator*( const Color &pt ) const;
      public: const Color &operator*=( const Color &pt );
    
      // Equality operators
      public: bool operator==( const Color &pt ) const;
      public: bool operator!=( const Color &pt ) const;

      /// Clamp the color values
      private: void Clamp();

      // Ostream operator
      public: friend std::ostream &operator<< (std::ostream &out, const Color &pt)    {
        out << pt.r << " " << pt.g << " " << pt.b << " " << pt.a;
        return out; 
      }

      // Istream operator
      public: friend std::istream &operator>> (std::istream &in, Color &pt)
      { 
        // Skip white spaces
        in.setf( std::ios_base::skipws );
        in >> pt.r >> pt.g >> pt.b >> pt.a;
        return in; 
      }

      // The values
      private: float r, g, b, a;
    };
    /// \}
  }
}
#endif
