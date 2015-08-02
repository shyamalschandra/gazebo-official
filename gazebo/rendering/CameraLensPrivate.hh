/*
 * Copyright (C) 2012-2015 Open Source Robotics Foundation
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

#ifndef _GAZEBO_RENDERING_CAMERALENSPRIVATE_HH_
#define _GAZEBO_RENDERING_CAMERALENSPRIVATE_HH_

namespace gazebo
{
  namespace rendering
  {
    /// \brief Private fields of camera lens
    class CameraLensPrivate
    {
      public: float c1;
      public: float c2;
      public: float c3;
      public: float f;
      public: float cutOffAngle;

      public: enum MapFunction {SIN=0,TAN,ID};
      public: MapFunction fun;
    };
  }
}

#endif
