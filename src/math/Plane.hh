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
#ifndef PLANE_HH
#define PLANE_HH

#include "math/Vector3.hh"
#include "math/Vector2d.hh"

namespace gazebo
{
	namespace math
  {
    class Plane
    {
      public: Plane();
      public: Plane(Vector3 normal, Vector2d size, double offset);
      public: virtual ~Plane();
  
      public: void Set(Vector3 normal, Vector2d size, double offset);
  
      /// \brief Equal operator
      public: const Plane &operator=(const Plane & p);
  
      public: Vector3 normal;
      public: Vector2d size;
      public: double d;
    };
  }
}
#endif
