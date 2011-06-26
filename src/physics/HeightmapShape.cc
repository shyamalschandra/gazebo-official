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
/* Desc: Heightmap shape
 * Author: Nate Keonig, Andrew Howard
 * Date: 8 May 2003
 */

#include <string.h>
#include <math.h>

#include "common/Image.hh"
#include "common/Global.hh"
#include "common/Exception.hh"

#include "physics/HeightmapShape.hh"

using namespace gazebo;
using namespace physics;


//////////////////////////////////////////////////////////////////////////////
// Constructor
HeightmapShape::HeightmapShape(GeomPtr parent)
    : Shape(parent)
{
  this->AddType(Base::HEIGHTMAP_SHAPE);

  common::Param::Begin(&this->parameters);
  this->imageFilenameP = new common::ParamT<std::string>("image","",1);
  this->worldTextureP = new common::ParamT<std::string>("world_texture","",0);
  this->detailTextureP = new common::ParamT<std::string>("detail_texture","",0);
  this->sizeP = new common::ParamT<math::Vector3>("size",math::Vector3(10,10,10), 0);
  this->offsetP = new common::ParamT<math::Vector3>("offset",math::Vector3(0,0,0), 0);
  common::Param::End();

  // NATY: this->ogreHeightmap = new OgreHeightmap(this->GetWorld()->GetScene());
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
HeightmapShape::~HeightmapShape()
{
  delete this->imageFilenameP;
  delete this->worldTextureP;
  delete this->detailTextureP;
  delete this->sizeP;
  delete this->offsetP;

  // NATY: delete this->ogreHeightmap;
}

//////////////////////////////////////////////////////////////////////////////
/// Update function.
void HeightmapShape::Update()
{
}

////////////////////////////////////////////////////////////////////////////////
/// Load the heightmap
void HeightmapShape::Load(common::XMLConfigNode *node)
{
  Shape::Load(node);
  this->imageFilenameP->Load(node);
  this->worldTextureP->Load(node);
  this->detailTextureP->Load(node);
  this->sizeP->Load(node);
  this->offsetP->Load(node);

  // Use the image to get the size of the heightmap
  this->img.Load( (**this->imageFilenameP) );

  // Width and height must be the same
  if (this->img.GetWidth() != this->img.GetHeight())
    gzthrow("Heightmap image must be square\n");

  this->terrainSize = (**this->sizeP);
}

////////////////////////////////////////////////////////////////////////////////
/// Initialize the heightmap
void HeightmapShape::Init()
{
}
 
////////////////////////////////////////////////////////////////////////////////
/// Save child parameters
void HeightmapShape::Save(std::string &prefix, std::ostream &stream)
{
  stream << prefix << *(this->imageFilenameP) << "\n";
  stream << prefix << *(this->worldTextureP) << "\n";
  stream << prefix << *(this->detailTextureP) << "\n";
  stream << prefix << *(this->sizeP) << "\n";
  stream << prefix << *(this->offsetP) << "\n";
}
