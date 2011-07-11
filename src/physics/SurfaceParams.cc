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
/* Desc: common::Parameters for contact joints
 * Author: Nate Koenig
 * Date: 30 July 2003
 */

#include <float.h>
#include "physics/SurfaceParams.hh"

using namespace gazebo;
using namespace physics;

//////////////////////////////////////////////////////////////////////////////
// Default constructor
SurfaceParams::SurfaceParams()
{
  // Bounce param
  this->bounce = 0.0;

  // Minumum velocity before bounce is applied
  this->bounceThreshold = 10.0;
}

//////////////////////////////////////////////////////////////////////////////
/// Load the contact params
void SurfaceParams::Load( sdf::ElementPtr &_sdf )
{
  sdf::ElementPtr bounceElem = _sdf->GetElement("bounce");
  if (bounceElem)
  {
    this->bounce = bounceElem->GetValueDouble("restitution_coefficient");
    this->bounceThreshold = bounceElem->GetValueDouble("threshold");
  }
}
