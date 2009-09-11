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
/* Desc: Middleman between OGRE and Gazebo
 * Author: Nate Koenig
 * Date: 13 Feb 2006
 * CVS: $Id$
 */

#include <Ogre.h>
#include <OgreDataStream.h>
#include <OgreLogManager.h>
#include <OgreWindowEventUtilities.h>

#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <string.h>

#include "OgreVisual.hh"
#include "UserCamera.hh"
#include "OgreMovableText.hh"
#include "OgreHUD.hh"
#include "Entity.hh"
#include "GazeboError.hh"
#include "GazeboMessage.hh"
#include "GazeboConfig.hh"
#include "Global.hh"
#include "XMLConfig.hh"
#include "Simulator.hh"
#include "OgreFrameListener.hh"
#include "OgreCreator.hh"
#include "OgreAdaptor.hh"

using namespace gazebo;

enum SceneTypes{SCENE_BSP, SCENE_EXT};


////////////////////////////////////////////////////////////////////////////////
/// Constructor
OgreAdaptor::OgreAdaptor()
{
  // Create a new log manager and prevent output from going to stdout
  this->logManager = new Ogre::LogManager();
  this->logManager->createLog("Ogre.log", true, false, false);

  this->backgroundColor=NULL;
  this->logManager=NULL;
  this->sceneMgr=NULL;
  this->root=NULL;

  this->dummyDisplay = false;

  Param::Begin(&this->parameters);
  this->ambientP = new ParamT<Vector4>("ambient",Vector4(.1,.1,.1,.1),0);
  this->shadowTextureSizeP = new ParamT<int>("shadowTextureSize", 512,0);
  this->shadowTechniqueP = new ParamT<std::string>("shadowTechnique", "stencilModulative", 0);
  this->drawGridP = new ParamT<bool>("grid", true, 0);
  this->updateRateP = new ParamT<double>("maxUpdateRate",0,0);
  this->skyMaterialP = new ParamT<std::string>("material","",1);
  Param::End();
}

////////////////////////////////////////////////////////////////////////////////
/// Destructor
OgreAdaptor::~OgreAdaptor()
{
  if (this->dummyDisplay)
  {
    glXDestroyContext(this->dummyDisplay, this->dummyContext);
    XDestroyWindow(this->dummyDisplay, this->dummyWindowId);
    XCloseDisplay(this->dummyDisplay);
  }

  delete this->ambientP;
  delete this->shadowTextureSizeP;
  delete this->shadowIndexSizeP;
  delete this->shadowTechniqueP;
  delete this->drawGridP;
  delete this->updateRateP;
  delete this->skyMaterialP;

}

////////////////////////////////////////////////////////////////////////////////
// Closes and free
void OgreAdaptor::Close()
{
  if (this->frameListener)
    delete this->frameListener;
  this->frameListener = NULL;
}

////////////////////////////////////////////////////////////////////////////////
/// Load the parameters for Ogre
void OgreAdaptor::Load(XMLConfigNode *rootNode)
{
  XMLConfigNode *node;

  node = rootNode->GetChild("ogre", "rendering");

  // Make the root
  try
  {
    this->root = new Ogre::Root();
  }
  catch (Ogre::Exception e)
  {
    gzthrow("Unable to create an Ogre rendering environment, no Root ");
  }

  // Default background color
  this->backgroundColor = new Ogre::ColourValue(Ogre::ColourValue::Black);

  // Load all the plugins
  this->LoadPlugins();

  // Setup the available resources
  this->SetupResources();

  // Setup the rendering system, and create the context
  this->SetupRenderSystem();

  // Initialize the root node, and don't create a window
  this->root->initialise(false);
}

////////////////////////////////////////////////////////////////////////////////
// Initialize ogre
void OgreAdaptor::Init(XMLConfigNode *rootNode)
{
  XMLConfigNode *node;
  Ogre::ColourValue ambient;

  node = rootNode->GetChild("ogre", "rendering");

  /// Create a dummy rendering context.
  /// This will allow gazebo to run headless. And it also allows OGRE to 
  /// initialize properly
  if (!Simulator::Instance()->GetGuiEnabled())
  {
    this->dummyDisplay = XOpenDisplay(0);
    if (!this->dummyDisplay) 
      gzthrow(std::string("Can't open display: ") + XDisplayName(0) + "\n");

    int screen = DefaultScreen(this->dummyDisplay);

    int attribList[] = {GLX_RGBA, GLX_DOUBLEBUFFER, GLX_DEPTH_SIZE, 16, 
                        GLX_STENCIL_SIZE, 8, None };

    this->dummyVisual = glXChooseVisual(this->dummyDisplay, screen, 
                                        (int *)attribList);

    this->dummyWindowId = XCreateSimpleWindow(this->dummyDisplay, 
        RootWindow(this->dummyDisplay, screen), 0, 0, 1, 1, 0, 0, 0);

    this->dummyContext = glXCreateContext(this->dummyDisplay, 
                                          this->dummyVisual, NULL, 1);

    glXMakeCurrent(this->dummyDisplay, this->dummyWindowId, this->dummyContext);

    OgreCreator::Instance()->CreateWindow(this->dummyDisplay, screen, 
                                          (long)this->dummyWindowId,1,1);
  }

  // Set default mipmap level (NB some APIs ignore this)
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps( 5 );

  // Get the SceneManager, in this case a generic one
  if (node->GetChild("bsp"))
  {
    this->sceneType= SCENE_BSP;
    this->sceneMgr = this->root->createSceneManager("BspSceneManager");
  }
  else
  {
    this->sceneType= SCENE_EXT;
    //this->sceneMgr = this->root->createSceneManager(Ogre::ST_EXTERIOR_FAR);
    //this->sceneMgr = this->root->createSceneManager(Ogre::ST_EXTERIOR_CLOSE);
    this->sceneMgr = this->root->createSceneManager(Ogre::ST_GENERIC);
  }

  // Load Resources
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

  Param::Begin(&this->parameters);
  this->shadowIndexSizeP = new ParamT<int>("shadowIndexSize",this->sceneMgr->getShadowIndexBufferSize(), 0);
  Param::End();

  this->ambientP->Load(node);
  this->shadowTextureSizeP->Load(node);
  this->shadowIndexSizeP->Load(node);
  this->shadowTechniqueP->Load(node);
  this->drawGridP->Load(node);
  this->updateRateP->Load(node);

  //Preload basic shapes that can be used anywhere
  OgreCreator::LoadBasicShapes();

  ambient.r = (**(this->ambientP)).x;
  ambient.g = (**(this->ambientP)).y;
  ambient.b = (**(this->ambientP)).z;
  ambient.a = (**(this->ambientP)).w;

  // Settings for shadow mapping
  if (**(this->shadowTechniqueP) == std::string("stencilAdditive"))
    this->sceneMgr->setShadowTechnique( Ogre::SHADOWTYPE_STENCIL_ADDITIVE );
  else if (**(this->shadowTechniqueP) == std::string("stencilModulative"))
    this->sceneMgr->setShadowTechnique( Ogre::SHADOWTYPE_STENCIL_MODULATIVE );
  else if (**(this->shadowTechniqueP) == std::string("textureAdditive"))
    this->sceneMgr->setShadowTechnique( Ogre::SHADOWTYPE_TEXTURE_ADDITIVE );
  else if (**(this->shadowTechniqueP) == std::string("textureModulative"))
    this->sceneMgr->setShadowTechnique( Ogre::SHADOWTYPE_TEXTURE_MODULATIVE );
  else if (**(this->shadowTechniqueP) == std::string("none"))
    this->sceneMgr->setShadowTechnique( Ogre::SHADOWTYPE_NONE );
  else 
    gzthrow(std::string("Unsupported shadow technique: ") + **(this->shadowTechniqueP) + "\n");

  this->sceneMgr->setShadowTextureSelfShadow(true);
  this->sceneMgr->setShadowTexturePixelFormat(Ogre::PF_FLOAT16_R);
  this->sceneMgr->setShadowTextureSize(**(this->shadowTextureSizeP));
  this->sceneMgr->setShadowIndexBufferSize(**(this->shadowIndexSizeP) );
  

  // Ambient lighting
  this->sceneMgr->setAmbientLight(ambient);

  this->sceneMgr->setShadowTextureSettings(512,2);
  this->sceneMgr->setShadowColour(Ogre::ColourValue(0.2, 0.2, 0.2));
  this->sceneMgr->setShadowFarDistance(30);

  // Add a sky dome to our scene
  if (node->GetChild("sky"))
  {
    this->skyMaterialP->Load(node->GetChild("sky"));
    OgreCreator::CreateSky(**(this->skyMaterialP));
  }

  // Add fog. This changes the background color
  OgreCreator::CreateFog(node->GetChild("fog"));

  if (**(this->drawGridP))
    OgreCreator::DrawGrid();

  // Set up the world geometry link
  if (this->sceneType==SCENE_BSP)
  {
    this->worldGeometry = node->GetString("bsp","",1);

    try
    {
      this->sceneMgr->setWorldGeometry(this->worldGeometry);
    }
    catch (Ogre::Exception e)
    {
      gzmsg(-1) << "Unable to load BSP geometry." << e.getDescription() << "\n";
      exit(-1);
    }
  }

  // Create our frame listener and register it
  this->frameListener = new OgreFrameListener();
  this->root->addFrameListener(this->frameListener);

  this->raySceneQuery = this->sceneMgr->createRayQuery( Ogre::Ray() );
  this->raySceneQuery->setSortByDistance(true);
  this->raySceneQuery->setQueryMask(Ogre::SceneManager::ENTITY_TYPE_MASK);
}

////////////////////////////////////////////////////////////////////////////////
// Save
void OgreAdaptor::Save(std::string &prefix, std::ostream &stream)
{
  stream << prefix << "<rendering:ogre>\n";
  stream << prefix << "  " << *(this->ambientP) << "\n";
  stream << prefix << "  " << *(this->drawGridP) << "\n";
  stream << prefix << "  " << *(this->updateRateP) << "\n";
  stream << prefix << "  <sky>\n";
  stream << prefix << "    " << *(this->skyMaterialP) << "\n";
  stream << prefix << "  </sky>\n";
  OgreCreator::SaveFog(prefix, stream);
  stream << prefix << "</rendering:ogre>\n";
}


////////////////////////////////////////////////////////////////////////////////
// Load plugins
void OgreAdaptor::LoadPlugins()
{
  std::list<std::string>::iterator iter;
  std::list<std::string> ogrePaths=Simulator::Instance()->GetGazeboConfig()->GetOgrePaths();
 
  for (iter=ogrePaths.begin(); 
       iter!=ogrePaths.end(); ++iter)
  {
    std::string path(*iter);
    DIR *dir=opendir(path.c_str()); 

    if (dir == NULL)
    {
      continue;
    }
    closedir(dir);

    std::vector<std::string> plugins;
    std::vector<std::string>::iterator piter;

    plugins.push_back(path+"/RenderSystem_GL.so");
    plugins.push_back(path+"/Plugin_ParticleFX.so");
    //plugins.push_back(path+"/Plugin_BSPSceneManager.so");
    //plugins.push_back(path+"/Plugin_OctreeSceneManager.so");

    for (piter=plugins.begin(); piter!=plugins.end(); piter++)
    {
      try
      {
        // Load the plugin into OGRE
        this->root->loadPlugin(*piter);
      }
      catch (Ogre::Exception e)
      {
        std::string description("Unable to load Ogre Plugins on directory ");
        description.append(path);
        description.append("\n Make sure the plugins path in the gazebo configuration file are set correctly.\n");
        gzthrow( description + e.getDescription() );
      }
    }
  }

}

////////////////////////////////////////////////////////////////////////////////
// Setup resources
void OgreAdaptor::SetupResources()
{
  std::vector<std::string> archNames;
  std::vector<std::string>::iterator aiter;
  std::list<std::string>::iterator iter;

  for (iter=Simulator::Instance()->GetGazeboConfig()->GetGazeboPaths().begin();
       iter!=Simulator::Instance()->GetGazeboConfig()->GetGazeboPaths().end(); iter++)
  {
    DIR *dir;
    if ((dir=opendir((*iter).c_str())) == NULL)
    {
      continue;
    }
    closedir(dir);

    archNames.push_back((*iter)+"/Media");
    archNames.push_back((*iter)+"/Media/fonts");
    archNames.push_back((*iter)+"/Media/materials/programs");
    archNames.push_back((*iter)+"/Media/materials/scripts");
    archNames.push_back((*iter)+"/Media/materials/textures");
    archNames.push_back((*iter)+"/Media/models");
    archNames.push_back((*iter)+"/Media/sets");
    archNames.push_back((*iter)+"/Media/maps");


    //we want to add all the material files of the sets
    if ((dir=opendir(((*iter)+"/Media/sets").c_str()))!= NULL)
    {
      std::string filename;
      struct dirent *dir_entry_p;
      while ( (dir_entry_p = readdir(dir))!=NULL )
      {
        filename =(*iter)+"/Media/sets/"+ dir_entry_p->d_name;
        archNames.push_back(filename);
      }
      closedir(dir);
    }

    for (aiter=archNames.begin(); aiter!=archNames.end(); aiter++)
    {
      try
      {
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation( *aiter, "FileSystem", "General");
      }
      catch (Ogre::Exception)
      {
        gzthrow("Unable to load Ogre Resources.\nMake sure the resources path in the world file is set correctly.");
      }
    }
  }
}


////////////////////////////////////////////////////////////////////////////////
// Setup render system
void OgreAdaptor::SetupRenderSystem()
{
  Ogre::RenderSystem *renderSys;

  // Set parameters of render system (window size, etc.)
  Ogre::RenderSystemList *rsList = this->root->getAvailableRenderers();
  int c = 0;

  renderSys = NULL;

  do
  {
    if (c == (int)rsList->size())
      break;

    renderSys = rsList->at(c);
    c++;
  }
  while (renderSys->getName().compare("OpenGL Rendering Subsystem")!= 0);

  if (renderSys == NULL)
  {
    gzthrow( "unable to find rendering system" );
  }

  // We operate in windowed mode
  renderSys->setConfigOption("Full Screen","No");

  /// We used to allow the user to set the RTT mode to PBuffer, FBO, or Copy. 
  ///   Copy is slow, and there doesn't seem to be a good reason to use it
  ///   PBuffer limits the size of the renderable area of the RTT to the
  ///           size of the first window created.
  ///   FBO seem to be the only good option
  renderSys->setConfigOption("RTT Preferred Mode", "FBO");

  this->root->setRenderSystem(renderSys);
}


////////////////////////////////////////////////////////////////////////////////
// Update the user cameras
void OgreAdaptor::UpdateCameras()
{
  UserCamera *userCam;

  std::vector<OgreCamera*>::iterator iter;

  OgreCreator::Instance()->Update();

  this->root->_fireFrameStarted();

  // Draw all the non-user cameras
  for (iter = this->cameras.begin(); iter != this->cameras.end(); iter++)
  {
    if (dynamic_cast<UserCamera*>((*iter)) == NULL)
      (*iter)->Render();
  }
  

  // Must update the user camera's last.
  for (iter = this->cameras.begin(); iter != this->cameras.end(); iter++)
  {
    userCam = dynamic_cast<UserCamera*>((*iter));
    if (userCam)
      userCam->Update();
  }

  this->root->_fireFrameEnded();
}

////////////////////////////////////////////////////////////////////////////////
/// Get an entity at a pixel location using a camera. Used for mouse picking. 
Entity *OgreAdaptor::GetEntityAt(OgreCamera *camera, Vector2<int> mousePos) 
{
  Entity *entity = NULL;
  Ogre::Camera *ogreCam = camera->GetOgreCamera();
  Ogre::Vector3 camPos = ogreCam->getPosition();

  Ogre::Ray mouseRay = ogreCam->getCameraToViewportRay(
      (float)mousePos.x / ogreCam->getViewport()->getActualWidth(), 
      (float)mousePos.y / ogreCam->getViewport()->getActualHeight() );

  this->raySceneQuery->setRay( mouseRay );

  // Perform the scene query
  Ogre::RaySceneQueryResult &result = this->raySceneQuery->execute();
  Ogre::RaySceneQueryResult::iterator iter = result.begin();

  for (iter = result.begin(); iter != result.end(); iter++)
  {
    if (iter->movable)
    {
      OgreVisual *vis = dynamic_cast<OgreVisual*>(iter->movable->getUserObject());

      if (vis && vis->GetOwner())
      {
        entity = vis->GetOwner();
        if (!entity)
          continue;

        return entity;

        /*entity->GetVisualNode()->ShowSelectionBox(true);

        if (entityType == "model")
          return (Entity*)(Simulator::Instance()->GetParentModel(entity));
        else if (entityType == "body")
          return (Entity*)(Simulator::Instance()->GetParentBody(entity));
        else
          return entity;
          */
      }
    }
  }

  return NULL;
}


////////////////////////////////////////////////////////////////////////////////
/// Get the desired update rate
double OgreAdaptor::GetUpdateRate()
{
  return **(this->updateRateP);
}

////////////////////////////////////////////////////////////////////////////////
/// Register a user camera
void OgreAdaptor::RegisterCamera( OgreCamera *cam )
{
  this->cameras.push_back( cam );
}
