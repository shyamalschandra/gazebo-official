/*
 * Copyright (C) 2015 Open Source Robotics Foundation
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

#ifndef _GAZEBO_JOYSTICK_PRIVATE_HH_
#define _GAZEBO_JOYSTICK_PRIVATE_HH_

#include <SDL2/SDL.h>

namespace gazebo
{
  namespace util
  {
    /// \internal
    /// \brief Private data for Joystick class.
    class JoystickPrivate
    {
      public: JoystickPrivate()
              : joy(NULL), axesCount(0), buttonCount(0),
                hatCount(0), ballCount(0)
      {
      }

      /// \brief Pointer to the SDL joystick interface.
      public: SDL_Joystick *joy;

      /// \brief True if the joystick has been initialized.
      public: bool initialized;

      public: int axesCount;
      public: int buttonCount;
      public: int hatCount;
      public: int ballCount;
    };
  }
}
#endif
