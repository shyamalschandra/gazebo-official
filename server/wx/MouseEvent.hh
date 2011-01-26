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
#ifndef MOUSEEVENT_HH
#define MOUSEEVENT_HH

#include "Vector2.hh"

namespace gazebo
{
  class Camera;

  class MouseEvent
  {
    public: enum ButtonState {DOWN, UP, SCROLL};

    public: MouseEvent()
            : pos(0,0), prevPos(0,0), pressPos(0,0), scroll(0,0),
              moveScale(0.01),dragging(false), left(UP), right(UP), middle(UP),
              camera(NULL)
            {}

    public: Vector2<int> pos; 
    public: Vector2<int> prevPos;
    public: Vector2<int> pressPos; 
    public: Vector2<int> scroll; 

    public: float moveScale;

    public: bool dragging;

    public: ButtonState left;
    public: ButtonState right;
    public: ButtonState middle;

    public: Camera *camera;
  };
};
#endif
