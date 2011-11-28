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

#include "math/Vector3.hh"
#include "BoxShape.hh"

using namespace gazebo;
using namespace physics;


////////////////////////////////////////////////////////////////////////////////
/// Constructor
BoxShape::BoxShape(CollisionPtr parent) : Shape(parent)
{
  this->AddType(Base::BOX_SHAPE);
}

////////////////////////////////////////////////////////////////////////////////
/// Destructor
BoxShape::~BoxShape()
{
} 

////////////////////////////////////////////////////////////////////////////////
/// Load the box
void BoxShape::Load( sdf::ElementPtr &_sdf )
{
  Shape::Load(_sdf);
}

////////////////////////////////////////////////////////////////////////////////
// Initialize the box
void BoxShape::Init()
{
  this->SetSize( this->sdf->GetValueVector3("size") );
}

////////////////////////////////////////////////////////////////////////////////
/// Set the size of the box
void BoxShape::SetSize( const math::Vector3 &size )
{
  this->sdf->GetAttribute("size")->Set( size );
}

math::Vector3 BoxShape::GetSize() const
{
  return this->sdf->GetValueVector3("size");
}

void BoxShape::FillShapeMsg(msgs::Geometry &_msg)
{
  _msg.set_type(msgs::Geometry::BOX);
  msgs::Set(_msg.mutable_box()->mutable_size(),this->GetSize());
}
