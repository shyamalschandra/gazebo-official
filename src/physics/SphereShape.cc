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
/* Desc: Sphere shape
 * Author: Nate Keonig
 * Date: 14 Oct 2009
 */
#include "physics/SphereShape.hh"

using namespace gazebo;
using namespace physics;

////////////////////////////////////////////////////////////////////////////////
/// Constructor
SphereShape::SphereShape(CollisionPtr parent) 
  : Shape(parent)
{
  this->AddType(Base::SPHERE_SHAPE);
}

////////////////////////////////////////////////////////////////////////////////
/// Destructor
SphereShape::~SphereShape()
{
}

////////////////////////////////////////////////////////////////////////////////
/// Load the sphere
void SphereShape::Load( sdf::ElementPtr &_sdf )
{
  Shape::Load(_sdf);
}

////////////////////////////////////////////////////////////////////////////////
/// Initialize the sphere
void SphereShape::Init()
{
  this->SetRadius(this->sdf->GetValueDouble("radius"));
}

////////////////////////////////////////////////////////////////////////////////
/// Set the size
void SphereShape::SetRadius(const double &_radius)
{
  this->sdf->GetAttribute("radius")->Set(_radius);
}

double SphereShape::GetRadius() const
{
  return this->sdf->GetValueDouble("radius");
}

void SphereShape::FillShapeMsg(msgs::Geometry &_msg)
{
  _msg.set_type(msgs::Geometry::SPHERE);
  _msg.mutable_sphere()->set_radius(this->GetRadius());
}
