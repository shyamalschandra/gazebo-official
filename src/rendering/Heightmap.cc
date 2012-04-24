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
/* Desc: Heightmap geometry
 * Author: Nate Keonig
 * Date: 12 May 2009
 */

#include <string.h>
#include <math.h>

#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>

#include "rendering/ogre.h"
#include "common/Image.hh"
#include "common/Exception.hh"

#include "rendering/Scene.hh"
#include "rendering/Heightmap.hh"

using namespace gazebo;
using namespace rendering;

//////////////////////////////////////////////////
Heightmap::Heightmap(ScenePtr _scene)
{
  this->scene = _scene;
}

//////////////////////////////////////////////////
Heightmap::~Heightmap()
{
  this->scene.reset();
}

//////////////////////////////////////////////////
void Heightmap::Load()
{
  this->terrainGlobals = new Ogre::TerrainGlobalOptions();
  this->terrainGroup = new Ogre::TerrainGroup(this->scene->GetManager(),
      Ogre::Terrain::ALIGN_X_Y, 513, 513.0f);
  this->terrainGroup->setFilenameConvention(
      Ogre::String("BasicTutorial3Terrain"), Ogre::String("dat"));
  this->terrainGroup->setOrigin(Ogre::Vector3::ZERO);

  this->ConfigureTerrainDefaults();

  for (int x = 0; x <= 0; ++x)
    for (int y = 0; y <= 0; ++y)
      this->DefineTerrain(x, y);

  // sync load since we want everything in place when we start
  this->terrainGroup->loadAllTerrains(true);

  // Calculate blend maps
  if (this->terrainsImported)
  {
    Ogre::TerrainGroup::TerrainIterator ti =
      this->terrainGroup->getTerrainIterator();
    while (ti.hasMoreElements())
    {
      Ogre::Terrain *t = ti.getNext()->instance;
      this->InitBlendMaps(t);
    }
  }

  this->terrainGroup->freeTemporaryResources();
}

///////////////////////////////////////////////////
void Heightmap::ConfigureTerrainDefaults()
{
  // Configure global

  // MaxPixelError: Decides how precise our terrain is going to be.
  // A lower number will mean a more accurate terrain, at the cost of
  // performance (because of more vertices)
  this->terrainGlobals->setMaxPixelError(8);

  // CompositeMapDistance: decides how far the Ogre terrain will render
  // the lightmapped terrain.
  this->terrainGlobals->setCompositeMapDistance(3000);

  // Important to set these so that the terrain knows what to use for
  // derived (non-realtime) data
  this->terrainGlobals->setLightMapDirection(Ogre::Vector3(0, 0, -1));
  this->terrainGlobals->setCompositeMapAmbient(
      this->scene->GetManager()->getAmbientLight());
  this->terrainGlobals->setCompositeMapDiffuse(Ogre::ColourValue(1, 1, 1, 1));

  // Configure default import settings for if we use imported image
  Ogre::Terrain::ImportData &defaultimp =
    this->terrainGroup->getDefaultImportSettings();

  defaultimp.terrainSize = 513;
  defaultimp.worldSize = 513.0f;
  defaultimp.inputScale = 1;
  defaultimp.minBatchSize = 33;
  defaultimp.maxBatchSize = 65;

  // textures. The default material generator takes two materials per layer.
  //    1. diffuse_specular - diffuse texture with a specular map in the
  //    alpha channel
  //    2. normal_height - normal map with a height map in the alpha channel
  {
    // number of texture layers
    defaultimp.layerList.resize(3);

    // The worldSize decides how big each splat of textures will be.
    // A smaller value will increase the resolution
    defaultimp.layerList[0].worldSize = 10;
    defaultimp.layerList[0].textureNames.push_back(
        "dirt_grayrocky_diffusespecular.dds");
    defaultimp.layerList[0].textureNames.push_back(
        "dirt_grayrocky_normalheight.dds");

    defaultimp.layerList[1].worldSize = 3;
    defaultimp.layerList[1].textureNames.push_back(
        "grass_green-01_diffusespecular.dds");
    defaultimp.layerList[1].textureNames.push_back(
        "grass_green-01_normalheight.dds");

    defaultimp.layerList[2].worldSize = 20;
    defaultimp.layerList[2].textureNames.push_back(
        "growth_weirdfungus-03_diffusespecular.dds");
    defaultimp.layerList[2].textureNames.push_back(
        "growth_weirdfungus-03_normalheight.dds");
  }
}

/////////////////////////////////////////////////
void Heightmap::DefineTerrain(int x, int y)
{
  Ogre::String filename = this->terrainGroup->generateFilename(x, y);
  if (Ogre::ResourceGroupManager::getSingleton().resourceExists(
        this->terrainGroup->getResourceGroup(), filename))
  {
    this->terrainGroup->defineTerrain(x, y);
  }
  else
  {
    Ogre::Image img;
    bool flipX = x % 2 != 0;
    bool flipY = y % 2 != 0;

    img.load("canyon.png",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    if (flipX)
      img.flipAroundY();
    if (flipY)
      img.flipAroundX();

    // this->GetTerrainImage(x % 2 != 0, y % 2 != 0, img);
    this->terrainGroup->defineTerrain(x, y, &img);
    this->terrainsImported = true;
  }
}

/////////////////////////////////////////////////
void Heightmap::InitBlendMaps(Ogre::Terrain *_terrain)
{
  Ogre::TerrainLayerBlendMap* blendMap0 = _terrain->getLayerBlendMap(1);
  Ogre::TerrainLayerBlendMap* blendMap1 = _terrain->getLayerBlendMap(2);
  Ogre::Real minHeight0 = 30;
  Ogre::Real fadeDist0 = 20;
  Ogre::Real minHeight1 = 31;
  Ogre::Real fadeDist1 = 10;
  float* pBlend0 = blendMap0->getBlendPointer();
  float* pBlend1 = blendMap1->getBlendPointer();
  for (Ogre::uint16 y = 0; y < _terrain->getLayerBlendMapSize(); ++y)
  {
    for (Ogre::uint16 x = 0; x < _terrain->getLayerBlendMapSize(); ++x)
    {
      Ogre::Real tx, ty;

      blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
      Ogre::Real height = _terrain->getHeightAtTerrainPosition(tx, ty);
      Ogre::Real val = (height - minHeight0) / fadeDist0;
      val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
      *pBlend0++ = val;

      val = (height - minHeight1) / fadeDist1;
      val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
      *pBlend1++ = val;
    }
  }
  blendMap0->dirty();
  blendMap1->dirty();
  blendMap0->update();
  blendMap1->update();
}

/////////////////////////////////////////////////
double Heightmap::GetHeight(double x, double y)
{
  return this->terrainGroup->getHeightAtWorldPosition(x, y, 600);
}
