/*
 *  Gazebo - Outdoor Multi-Robot Simulator
 *  Copyright (C) 2003
 *     Nate Koenig & Andrew Howard
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/* Desc: Heightmap geometry
 * Author: Nate Keonig, Andrew Howard
 * Date: 8 May 2003
 * CVS: $Id$
 */

#include <ode/ode.h>
#include <Ogre.h>
#include <iostream>
#include <string.h>
#include <math.h>

#include "GazeboError.hh"
#include "OgreAdaptor.hh"
#include "Simulator.hh"
#include "OgreAdaptor.hh"
#include "Global.hh"
#include "Body.hh"
#include "HeightmapGeom.hh"

using namespace gazebo;

//////////////////////////////////////////////////////////////////////////////
// Constructor
HeightmapGeom::HeightmapGeom(Body *body)
    : Geom(body)
{
  Param::Begin(&this->parameters);
  this->imageFilenameP = new ParamT<std::string>("image","",1);
  this->worldTextureP = new ParamT<std::string>("worldTexture","",0);
  this->detailTextureP = new ParamT<std::string>("detailTexture","",0);
  this->sizeP = new ParamT<Vector3>("size",Vector3(10,10,10), 0);
  this->offsetP = new ParamT<Vector3>("offset",Vector3(0,0,0), 0);
  Param::End();
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
HeightmapGeom::~HeightmapGeom()
{
  OgreAdaptor *ogreAdaptor = Simulator::Instance()->GetRenderEngine();
  ogreAdaptor->sceneMgr->destroyQuery(this->rayQuery);

  delete this->imageFilenameP;
  delete this->worldTextureP;
  delete this->detailTextureP;
  delete this->sizeP;
  delete this->offsetP;
}

//////////////////////////////////////////////////////////////////////////////
/// Update function.
void HeightmapGeom::UpdateChild()
{
}

//////////////////////////////////////////////////////////////////////////////
/// get height at a point
float HeightmapGeom::GetHeightAt(const Vector2<float> &pos)
{
  Ogre::Vector3 pos3(pos.x,this->terrainSize.z,pos.y);

  this->ray.setOrigin(pos3);
  this->rayQuery->setRay(this->ray);
  this->distToTerrain = 0;
  this->rayQuery->execute(this);

  return this->terrainSize.z - this->distToTerrain;
}

////////////////////////////////////////////////////////////////////////////////
// Create a lookup table of the terrain's height
void HeightmapGeom::FillHeightMap()
{
  unsigned int x,y;
  float h;

  // Resize the vector to match the size of the vertices
  this->heights.resize(this->odeVertSize*this->odeVertSize);

  // Iterate over all the verices
  for (y=0; y<this->odeVertSize; y++)
  {
    for (x=0; x<this->odeVertSize; x++)
    {
      // Find the height at a vertex
      h = this->GetHeightAt(Vector2<float>(x*this->odeScale.x, y*this->odeScale.y));

      // Store the height for future use
      this->heights[y*this->odeVertSize + x] = h;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Called by ODE to get the height at a vertex
dReal HeightmapGeom::GetHeightCallback(void *data, int x, int y)
{
  HeightmapGeom *geom = (HeightmapGeom*)(data);

  // Return the height at a specific vertex
  return geom->heights[y * geom->odeVertSize + x];
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Overloaded Ogre function for Ray Scene Queries
bool HeightmapGeom::queryResult(Ogre::MovableObject *obj, Ogre::Real dist)
{
  return false;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Overloaded Ogre function for Ray Scene Queries
bool HeightmapGeom::queryResult(Ogre::SceneQuery::WorldFragment *frag, Ogre::Real dist)
{
  this->distToTerrain = dist;
  return false;
}

////////////////////////////////////////////////////////////////////////////////
/// Load the heightmap
void HeightmapGeom::LoadChild(XMLConfigNode *node)
{
  Ogre::Image tmpImage;
  int tileSize;
  OgreAdaptor *ogreAdaptor;

  ogreAdaptor = Simulator::Instance()->GetRenderEngine();

  this->imageFilenameP->Load(node);
  this->worldTextureP->Load(node);
  this->detailTextureP->Load(node);
  this->sizeP->Load(node);
  this->offsetP->Load(node);


  // Use the image to get the size of the heightmap
  tmpImage.load(this->imageFilenameP->GetValue(),Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

  // Width and height must be the same
  if (tmpImage.getWidth() != tmpImage.getHeight())
  {
    gzthrow("Heightmap image must be square\n");
  }

  this->terrainVertSize = tmpImage.getWidth();

  float nf = (float)(log(this->terrainVertSize-1)/log(2));
  int ni = (int)(log(this->terrainVertSize-1)/log(2));

  // Make sure the heightmap image size is (2^n)+1 in size
  if ( nf - ni != 0)
  {
    gzthrow("Heightmap image size must be (2^n)+1\n");
  }

  // Calculate a good tile size
  tileSize = (int)(pow( 2,  ni/2 ));


  if (tileSize <= 2)
  {
    tileSize = 4;
  }

  tileSize++;

  this->terrainSize = this->sizeP->GetValue();

  this->terrainScale = this->terrainSize / this->terrainVertSize;

  this->odeVertSize = this->terrainVertSize * 4;
  this->odeScale = this->terrainSize / this->odeVertSize;


  std::ostringstream stream;
 
  /*std::cout << "Terrain Scale[" << this->terrainScale << "]\n";
  std::cout << "ODE Scale[" << this->odeScale << "]\n";
  std::cout << "Terrain Image[" << this->imageFilenameP->GetValue() << "] Size[" << this->terrainSize << "]\n";
  printf("Terrain Size[%f %f %f]\n", this->terrainSize.x, this->terrainSize.y, this->terrainSize.z);
  printf("VertSize[%d] Tile Size[%d]\n", this->terrainVertSize, tileSize);
  */

  stream << "WorldTexture=" << worldTextureP->GetValue() << "\n";
  //The detail texture
  stream << "DetailTexture=" << detailTextureP->GetValue() << "\n";
  // number of times the detail texture will tile in a terrain tile
  stream << "DetailTile=3\n";
  // Heightmap source
  stream << "PageSource=Heightmap\n";
  // Heightmap-source specific settings
  stream << "Heightmap.image=" << this->imageFilenameP->GetValue() << "\n";
  // How large is a page of tiles (in vertices)? Must be (2^n)+1
  stream << "PageSize=" << this->terrainVertSize << "\n";
  // How large is each tile? Must be (2^n)+1 and be smaller than PageSize
  stream << "TileSize=" << tileSize << "\n";
  // The maximum error allowed when determining which LOD to use
  stream << "MaxPixelError=4\n";
  // The size of a terrain page, in world units
  stream << "PageWorldX=" << this->terrainSize.x << "\n";
  stream << "PageWorldZ=" << this->terrainSize.y << "\n";
  // Maximum height of the terrain
  stream << "MaxHeight="<< this->terrainSize.z << "\n";
  // Upper LOD limit
  stream << "MaxMipMapLevel=2\n";

  // Create a data stream for loading the terrain into Ogre
  char *mstr = new char[1024];//stream.str().size()];
  bzero (mstr, 1024);
  sprintf(mstr, stream.str().c_str());
  Ogre::DataStreamPtr dataStream(
    new Ogre::MemoryDataStream(mstr,strlen(mstr)) );

  // Set the static terrain in Ogre
  ogreAdaptor->sceneMgr->setWorldGeometry(dataStream);

  // HACK to make the terrain oriented properly
  Ogre::SceneNode *tnode = ogreAdaptor->sceneMgr->getSceneNode("Terrain");
  tnode->pitch(Ogre::Degree(90));
  tnode->translate(Ogre::Vector3(-this->terrainSize.x*0.5, this->terrainSize.y*0.5, 0));

  // Setup the ray scene query, which is used to determine the heights of
  // the vertices for ODE
  this->ray = Ogre::Ray(Ogre::Vector3::ZERO, Ogre::Vector3::NEGATIVE_UNIT_Y);
  this->rayQuery = ogreAdaptor->sceneMgr->createRayQuery(this->ray);
  this->rayQuery->setQueryTypeMask(Ogre::SceneManager::WORLD_GEOMETRY_TYPE_MASK);
  this->rayQuery->setWorldFragmentType(Ogre::SceneQuery::WFT_SINGLE_INTERSECTION);

  // Construct the heightmap lookup table
  this->FillHeightMap();

  // Create the ODE heightfield
  this->odeData = dGeomHeightfieldDataCreate();

  // Setup a callback method for ODE
  dGeomHeightfieldDataBuildCallback(
    this->odeData,
    this,
    HeightmapGeom::GetHeightCallback,
    this->terrainSize.x,
    this->terrainSize.y,
    this->odeVertSize,
    this->odeVertSize,
    1.0, // scale
    0.0, // vertical offset
    0.0, // vertical thickness
    0 // wrap mode
  );

  // Restrict the bounds of the AABB to improve efficiency
  dGeomHeightfieldDataSetBounds( this->odeData, 0, this->terrainSize.z);

  this->geomId = dCreateHeightfield( this->spaceId, this->odeData, 1);

  this->SetGeom(this->geomId, false);

  this->SetStatic(true);

  //Rotate so Z is up, not Y (which is the default orientation)
  Quatern quat;
  Pose3d pose = this->GetPose();

  quat.SetFromEuler(Vector3(DTOR(90),0,0));

  pose.rot = pose.rot * quat;
  this->body->SetPose(pose);

  delete [] mstr;
}

////////////////////////////////////////////////////////////////////////////////
/// Save child parameters
void HeightmapGeom::SaveChild(std::string &prefix, std::ostream &stream)
{
  stream << prefix << *(this->imageFilenameP) << "\n";
  stream << prefix << *(this->worldTextureP) << "\n";
  stream << prefix << *(this->detailTextureP) << "\n";
  stream << prefix << *(this->sizeP) << "\n";
  stream << prefix << *(this->offsetP) << "\n";
}


