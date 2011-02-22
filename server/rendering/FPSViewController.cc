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
#include "Simulator.hh"
#include "Global.hh"
#include "Vector2.hh"
#include "UserCamera.hh"
#include "MouseEvent.hh"
#include "FPSViewController.hh"

using namespace gazebo;

////////////////////////////////////////////////////////////////////////////////
/// Constructor
FPSViewController::FPSViewController(UserCamera *camera)
  : ViewController(camera)
{
}

////////////////////////////////////////////////////////////////////////////////
/// Destructor
FPSViewController::~FPSViewController()
{
}

////////////////////////////////////////////////////////////////////////////////
// Update
void FPSViewController::Update()
{
}

////////////////////////////////////////////////////////////////////////////////
/// Handle a mouse event
void FPSViewController::HandleMouseEvent(const MouseEvent &event)
{
  if (!this->camera->GetUserMovable())
    return;

  Vector2<int> drag = event.pos - event.prevPos;

  Vector3 directionVec(0,0,0);

  if (event.left == MouseEvent::DOWN)
  {
    this->camera->RotateYaw(DTOR(drag.x * 0.1));
    this->camera->RotatePitch(DTOR(-drag.y * 0.1));
  }
  else if (event.right == MouseEvent::DOWN)
  {
    // interactively pan view
    directionVec.x = 0;
    directionVec.y =  drag.x * event.moveScale;
    directionVec.z =  drag.y * event.moveScale;
  }
  else if (event.middle == MouseEvent::DOWN)
  {
    directionVec.x =  drag.y * event.moveScale;
    directionVec.y =  0;
    directionVec.z =  0;
  }
  else if (event.middle == MouseEvent::SCROLL)
  {
    directionVec.x -=  50.0 * event.scroll.y * event.moveScale;
    directionVec.y =  0;
    directionVec.z =  0;
  }

  this->camera->Translate(directionVec);
}
