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

#include "gazebo/common/Console.hh"

#include "gazebo/rendering/paged_geometry/PagedGeometry.h"
#include "gazebo/rendering/paged_geometry/GrassLoader.h"
#include "gazebo/rendering/paged_geometry/BatchPage.h"
#include "gazebo/rendering/paged_geometry/WindBatchPage.h"
#include "gazebo/rendering/paged_geometry/ImpostorPage.h"
#include "gazebo/rendering/paged_geometry/TreeLoader2D.h"

#include "gazebo/rendering/Camera.hh"
#include "gazebo/rendering/UserCamera.hh"
#include "gazebo/rendering/Scene.hh"
#include "gazebo/rendering/Forest.hh"

namespace HeightFunction
{
  inline float getTerrainHeight(const float _x, const float _z,
      void *_userData = NULL)
  {
    return 0;
  }
}

using namespace gazebo;
using namespace rendering;


//////////////////////////////////////////////////
Forest::Forest(ScenePtr _scene)
{
  this->scene = _scene;
  this->initialized = false;
}

//////////////////////////////////////////////////
Forest::~Forest()
{
  this->Clear();
}

//////////////////////////////////////////////////
void Forest::Clear()
{
  // [NOTE] Always remember to delete any PageLoader(s) and PagedGeometry
  // instances to avoid memory leaks.

  // Delete the PageLoader's
  // Delete the PagedGeometry instances
  if (this->grass)
  {
    delete this->grass->getPageLoader();
    delete this->grass;
    this->grass = NULL;
  }
  if (this->trees)
  {
    delete this->trees->getPageLoader();
    delete this->trees;
    this->trees = NULL;
  }
  if (this->bushes)
  {
    delete this->bushes->getPageLoader();
    delete this->bushes;
    this->bushes=0;
  }

  if (this->scene)
  {
    //Also delete the entities
    this->scene->GetManager()->destroyEntity("Tree1");
    this->scene->GetManager()->destroyEntity("Fern");
    this->scene->GetManager()->destroyEntity("Plant");
    this->scene->GetManager()->destroyEntity("Mushroom");

    this->scene.reset();
  }
  this->camera.reset();
}

//////////////////////////////////////////////////
void Forest::Load()
{
  if (!this->scene)
    return;

  // TODO: Extend to other types of cameras. How about multiple cameras sensors?
  if (this->scene->GetUserCameraCount() == 0u)
    return;

  this->camera = boost::dynamic_pointer_cast<Camera>(
      this->scene->GetUserCamera(0));

  this->connections.push_back(event::Events::ConnectRender(
        boost::bind(&Forest::Update, this, false)));
}

//////////////////////////////////////////////////
void Forest::LoadScene()
{
  if (!this->camera)
    return;

  // Setup the fog up to 1500 units away
  // this->scene->GetManager()->setFog(FOG_LINEAR, viewport->getBackgroundColour(), 0, 100, 900);


  Ogre::Vector3 upAxis = Ogre::Vector3(0, 0, 1);
  Ogre::Vector3 rightAxis = Ogre::Vector3(1, 0, 0);
  uint32_t visibilityFlags = GZ_VISIBILITY_GUI;

  // Setup a skybox
  // this->scene->GetManager()->setSkyBox(true, "3D-Diggers/SkyBox", 2000);

  // -------------------------------------- LOAD GRASS -----------------------
  // Create and configure a new PagedGeometry instance for this->grass
  this->grass = new Forests::PagedGeometry(this->camera->GetOgreCamera(), 30);
  this->grass->setCoordinateSystem(upAxis, rightAxis);
  this->grass->addDetailLevel<Forests::GrassPage>(60);

  // Create a GrassLoader object
  Forests::GrassLoader *grassLoader = new Forests::GrassLoader(this->grass);
  // Assign the "treeLoader" to be used to load geometry for the PagedGeometry
  // instance
  this->grass->setPageLoader(grassLoader);

  // Supply a height function to GrassLoader so it can calculate this->grass Y values
  // HeightFunction::initialize(this->scene->GetManager());
  grassLoader->setHeightFunction(&HeightFunction::getTerrainHeight);

  //Add some this->grass to the scene with GrassLoader::addLayer()
  Forests::GrassLayer *l = grassLoader->addLayer("3D-Diggers/plant1sprite");

  //Configure the this->grass layer properties (size, density, animation properties,
  // fade settings, etc.)
  l->setMinimumSize(0.7f, 0.7f);
  l->setMaximumSize(0.9f, 0.9f);
  //Enable animations
  l->setAnimationEnabled(true);
  //Sway fairly unsynchronized
  l->setSwayDistribution(7.0f);
  //Sway back and forth 0.5 units in length
  l->setSwayLength(0.1f);
  //Sway 1/2 a cycle every second
  l->setSwaySpeed(0.4f);
  //Relatively dense this->grass
  l->setDensity(3.0f);
  l->setRenderTechnique(Forests::GRASSTECH_SPRITE);
  //Distant this->grass should slowly raise out of the ground when coming in range
  l->setFadeTechnique(Forests::FADETECH_GROW);

  //[NOTE] This sets the color map, or lightmap to be used for this->grass. All this->grass will be colored according
  //to this texture. In this case, the colors of the terrain is used so this->grass will be shadowed/colored
  //just as the terrain is (this usually makes the this->grass fit in very well).
  l->setColorMap("terrain_texture2.jpg");

  //This sets the density map that will be used to determine the density levels of this->grass all over the
  //terrain. This can be used to make this->grass grow anywhere you want to; in this case it's used to make
  //this->grass grow only on fairly level ground (see densitymap.png to see how this works).
  l->setDensityMap("densitymap.png");

  //setMapBounds() must be called for the density and color maps to work (otherwise GrassLoader wouldn't
  //have any knowledge of where you want the maps to be applied). In this case, the maps are applied
  //to the same boundaries as the terrain.
  l->setMapBounds(Forests::TBounds(-1000, -1000, 1000, 1000));	//(0,0)-(1500,1500) is the full boundaries of the terrain

  //-------------------------------------- LOAD TREES --------------------------------------
  //Create and configure a new PagedGeometry instance
  this->trees = new Forests::PagedGeometry();
  this->trees->setCoordinateSystem(upAxis, rightAxis);

  //Set the camera so PagedGeometry knows how to calculate LODs
  this->trees->setCamera(this->camera->GetOgreCamera());
  //Set the size of each page of geometry
  this->trees->setPageSize(50);
  //Use infinite paging mode
  this->trees->setInfinite();

  #ifdef WIND
  //WindBatchPage is a variation of BatchPage which includes a wind animation shader
  this->trees->addDetailLevel<Forests::WindBatchPage>(90, 30);		//Use batches up to 150 units away, and fade for 30 more units
  #else
  this->trees->addDetailLevel<Forests::BatchPage>(90, 30);		//Use batches up to 150 units away, and fade for 30 more units
  #endif
  this->trees->addDetailLevel<Forests::ImpostorPage>(700, 50);	//Use impostors up to 400 units, and for for 50 more units

  // Create a new TreeLoader2D object
  Forests::TreeLoader2D *treeLoader = new Forests::TreeLoader2D(this->trees, Forests::TBounds(-1000, -1000, 1000, 1000));
  this->trees->setPageLoader(treeLoader);	//Assign the "treeLoader" to be used to load geometry for the PagedGeometry instance

  // Supply a height function to TreeLoader2D so it can calculate tree Y values
  // HeightFunction::initialize(this->scene->GetManager());
  treeLoader->setHeightFunction(&HeightFunction::getTerrainHeight);

  // [NOTE] This sets the color map, or lightmap to be used for this->trees. All this->trees will be colored according
  // to this texture. In this case, the shading of the terrain is used so this->trees will be shadowed
  // just as the terrain is (this should appear like the terrain is casting shadows on the this->trees).
  // You may notice that TreeLoader2D / TreeLoader3D doesn't have a setMapBounds() function as GrassLoader
  // does. This is because the bounds you specify in the TreeLoader2D constructor are used to apply
  // the color map.
  treeLoader->setColorMap("terrain_lightmap.jpg");

  //Load a tree entity
  Ogre::Entity *tree1 = this->scene->GetManager()->createEntity("Tree1", "fir05_30.mesh");
  tree1->setVisibilityFlags(visibilityFlags);

  Ogre::Entity *tree2 = this->scene->GetManager()->createEntity("Tree2", "fir14_25.mesh");
  tree2->setVisibilityFlags(visibilityFlags);

  #ifdef WIND
  this->trees->setCustomParam(tree1->getName(), "windFactorX", 15);
  this->trees->setCustomParam(tree1->getName(), "windFactorY", 0.01);
  this->trees->setCustomParam(tree2->getName(), "windFactorX", 22);
  this->trees->setCustomParam(tree2->getName(), "windFactorY", 0.013);
  #endif

  //Randomly place 10000 copies of the tree on the terrain
  Ogre::Vector3 position = Ogre::Vector3::ZERO;
  Ogre::Radian yaw;
  Ogre::Real scale;
  for (int i = 0; i < 10000; i++)
  {
    yaw = Ogre::Degree(Ogre::Math::RangeRandom(0, 360));

    position.x = Ogre::Math::RangeRandom(-1000, 1000);
    position.y = Ogre::Math::RangeRandom(-1000, 1000);

    scale = Ogre::Math::RangeRandom(0.07f, 0.12f);

    float rnd = Ogre::Math::UnitRandom();
    if (rnd < 0.5f)
    {
    //[NOTE] Unlike TreeLoader3D, TreeLoader2D's addTree() function accepts a Vector2D position (x/z)
    //The Y value is calculated during runtime (to save memory) from the height function supplied (above)
    if (Ogre::Math::UnitRandom() < 0.5f)
      treeLoader->addTree(tree1, position, yaw, scale);
    //else
    //	treeLoader->addTree(tree2, position, yaw, scale);
    }
    else
      treeLoader->addTree(tree2, position, yaw, scale);
  }

  // -------------------------------------- LOAD BUSHES ------------------------

  // Create and configure a new PagedGeometry instance for this->bushes
  this->bushes = new Forests::PagedGeometry(this->camera->GetOgreCamera(), 50);
  this->bushes->setCoordinateSystem(upAxis, rightAxis);

  #ifdef WIND
  this->bushes->addDetailLevel<Forests::WindBatchPage>(80, 50);
  #else
  this->bushes->addDetailLevel<Forests::BatchPage>(80, 50);
  #endif

  // Create a new TreeLoader2D object for the this->bushes
  Forests::TreeLoader2D *bushLoader =
      new Forests::TreeLoader2D(this->bushes, Forests::TBounds(-1000, -1000, 1000, 1000));
  this->bushes->setPageLoader(bushLoader);

  // Supply the height function to TreeLoader2D so it can calculate tree Y values
  //HeightFunction::initialize(this->scene->GetManager());
  bushLoader->setHeightFunction(&HeightFunction::getTerrainHeight);

  bushLoader->setColorMap("terrain_lightmap.jpg");

  // Load a bush entity
  Ogre::Entity *fern = this->scene->GetManager()->createEntity("Fern", "farn1.mesh");
  fern->setVisibilityFlags(visibilityFlags);

  Ogre::Entity *plant = this->scene->GetManager()->createEntity("Plant", "plant2.mesh");
  plant->setVisibilityFlags(visibilityFlags);

  Ogre::Entity *mushroom = this->scene->GetManager()->createEntity("Mushroom", "shroom1_1.mesh");
  mushroom->setVisibilityFlags(visibilityFlags);

  #ifdef WIND
  this->bushes->setCustomParam(fern->getName(), "factorX", 1);
  this->bushes->setCustomParam(fern->getName(), "factorY", 0.01);

  this->bushes->setCustomParam(plant->getName(), "factorX", 0.6);
  this->bushes->setCustomParam(plant->getName(), "factorY", 0.02);
  #endif

  // Randomly place 20,000 this->bushes on the terrain
  for (int i = 0; i < 20000; i++){
    yaw = Ogre::Degree(Ogre::Math::RangeRandom(0, 360));
    position.x = Ogre::Math::RangeRandom(-1000, 1000);
    position.y = Ogre::Math::RangeRandom(-1000, 1000);

    float rnd = Ogre::Math::UnitRandom();
    if (rnd < 0.8f) {
      scale = Ogre::Math::RangeRandom(0.3f, 0.4f);
      bushLoader->addTree(fern, position, yaw, scale);
    } else if (rnd < 0.9) {
      scale = Ogre::Math::RangeRandom(0.2f, 0.6f);
      bushLoader->addTree(mushroom, position, yaw, scale);
    } else {
      scale = Ogre::Math::RangeRandom(0.3f, 0.5f);
      bushLoader->addTree(plant, position, yaw, scale);
    }
  }
  this->initialized = true;
}

//////////////////////////////////////////////////
void Forest::Update(bool _force)
{
  if (!this->initialized)
    this->LoadScene();

  std::cerr << " update " << std::endl;

  // [NOTE] PagedGeometry::update() is called every frame to keep LODs, etc.
  // up-to-date
  if (this->grass)
    this->grass->update();
  if (this->trees)
    this->trees->update();
  if (this->bushes)
    this->bushes->update();
}
