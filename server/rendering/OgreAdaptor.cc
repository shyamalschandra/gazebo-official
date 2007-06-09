#include <Ogre.h>
#include <CEGUISystem.h>
#include <CEGUISchemeManager.h>
#include <OgreCEGUIRenderer.h>
#include <OgreLogManager.h>
#include <OgreWindowEventUtilities.h>

#include "GazeboMessage.hh"
#include "Global.hh"
#include "GazeboError.hh"
#include "XMLConfig.hh"
#include "OgreFrameListener.hh"
#include "OgreAdaptor.hh"

using namespace gazebo;

OgreAdaptor *OgreAdaptor::myself = NULL;

/// Constructor
OgreAdaptor::OgreAdaptor()
{
  // Create a new log manager and prevent output from going to stdout
  this->logManager = new Ogre::LogManager();
  this->logManager->createLog("Ogre.log", true, false, false);

  // Make the root 
  this->root = new Ogre::Root(); 
}

/// Destructor
OgreAdaptor::~OgreAdaptor()
{
}

OgreAdaptor *OgreAdaptor::Instance()
{
  if (!myself)
    myself = new OgreAdaptor;

  return myself;
}


void OgreAdaptor::Init(XMLConfigNode *node)
{
  if (!node)
  {
    gzthrow( "missing OGRE Rendernig information" );
  }

  // Load all the plugins
  this->LoadPlugins(node->GetChild("plugins"));

  // Setup the available resources 
  this->SetupResources(node->GetChild("resources"));

  // Setup the rendering system, and create the context
  this->SetupRenderSystem(true);

  // Initialize the root node, and create a window
  this->window = this->root->initialise(true); 

  // Get the SceneManager, in this case a generic one
  this->sceneMgr = this->root->createSceneManager(Ogre::ST_GENERIC);

  // Set default mipmap level (NB some APIs ignore this)
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps( 5 );

  // Load Resources
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

  // Get the SceneManager, in this case a generic one
  // Default lighting
  this->sceneMgr->setAmbientLight(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 1.0f)); 
  this->sceneMgr->setShadowTechnique( Ogre::SHADOWTYPE_STENCIL_ADDITIVE );

  // Add a sky dome to our scene
  this->sceneMgr->setSkyDome(true,"Gazebo/CloudySky",5,8);

  // Create our frame listener and register it
  this->frameListener = new OgreFrameListener();

  this->root->addFrameListener(this->frameListener);

  // CEGUI setup
  this->guiRenderer = new CEGUI::OgreCEGUIRenderer(this->window, 
      Ogre::RENDER_QUEUE_OVERLAY, false, 0, this->sceneMgr);

  this->guiSystem = new CEGUI::System(this->guiRenderer);
}

void OgreAdaptor::Init(Display *display, 
                       XVisualInfo *visual, 
                       Window windowId, int width, int height)
{
  Ogre::NameValuePairList params;
  Ogre::StringVector paramsVector;

  this->display = display;
  this->visual = visual;
  this->windowId = windowId;

  // Setup the available resources 
  this->SetupResources(NULL);

  // Setup the rendering system, and don't create the context
  this->SetupRenderSystem(false);

  // Initialize the root node, and don't create a window
  this->window = this->root->initialise(false); 

  // Create the window
  this->CreateWindow(width,height);

  // Get the SceneManager, in this case a generic one
  this->sceneMgr = this->root->createSceneManager(Ogre::ST_GENERIC);

  // Set default mipmap level (NB some APIs ignore this)
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps( 2 );

  // Load Resources
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

  // Default lighting
  this->sceneMgr->setAmbientLight(Ogre::ColourValue(0.8f, 0.8f, 0.8f, 1.0f)); 
  this->sceneMgr->setShadowTechnique( Ogre::SHADOWTYPE_STENCIL_ADDITIVE );

  OgreGLXWindowInterface* pWindowInterface = NULL;

  this->window->getCustomAttribute("GLXWINDOWINTERFACE", &pWindowInterface);

  //pWindowInterface->exposed(true);


  // Create our frame listener and register it
  this->frameListener = new OgreFrameListener();
  this->root->addFrameListener(this->frameListener);
}

void OgreAdaptor::SetupResources(XMLConfigNode *node)
{
  XMLConfigNode *sectionNode;
  XMLConfigNode *childNode;
  std::string path, sectionName, typeName, archName;

  if (!node)
  {
    gzthrow( "ogre resources xml nodemissing" );
  }

  path = node->GetString("path","",1);

  if (path.empty())
  {
    gzthrow( "empty resource path" );
  }

  sectionNode = node->GetChild();

  while(sectionNode)
  {
    sectionName = sectionNode->GetName();
    sectionName[0] = std::toupper(sectionName[0]);

    childNode = sectionNode->GetChild();

    while (childNode)
    {
      typeName = childNode->GetName();
      archName = path+"/"+childNode->GetValue();

      typeName[0] = std::toupper(typeName[0]);

      // Hack to make OGRE happy
      if (typeName == "Filesystem")
        typeName = "FileSystem";

      Ogre::ResourceGroupManager::getSingleton().addResourceLocation( archName, typeName, sectionName); 

      childNode = childNode->GetNext();
    }

    sectionNode = sectionNode->GetNext();
  }
}

void OgreAdaptor::SetupRenderSystem(bool create)
{
  // Set parameters of render system (window size, etc.)
  //if (!this->root->restoreConfig())
  {
    Ogre::RenderSystemList *rsList = this->root->getAvailableRenderers();
    int c = 0;
    Ogre::RenderSystem *selectedRenderSystem = NULL;

    do 
    {
      if (c == (int)rsList->size())
        break;
      selectedRenderSystem = rsList->at(c);
      c++;
    } while (selectedRenderSystem->getName().compare("OpenGL Rendering Subsystem")!= 0);

    if (selectedRenderSystem == NULL)
    {
      gzthrow( "unable to find rendering system" );
    }

    selectedRenderSystem->setConfigOption("Full Screen","No");
    selectedRenderSystem->setConfigOption("FSAA","2");

    if (create)
    {
      selectedRenderSystem->setConfigOption("Video Mode","800 x 600 @ 16-bit colour");
    }

    this->root->setRenderSystem(selectedRenderSystem);
  }
}


void OgreAdaptor::CreateWindow(int width, int height)
{
  Ogre::StringVector paramsVector;
  Ogre::NameValuePairList params;

  paramsVector.push_back( Ogre::StringConverter::toString( (int)(this->display) ) );
  paramsVector.push_back( Ogre::StringConverter::toString((int)this->visual->screen));

  paramsVector.push_back( Ogre::StringConverter::toString((int)this->windowId));
  paramsVector.push_back( Ogre::StringConverter::toString((int)(this->visual)));

  params["parentWindowHandle"] = Ogre::StringConverter::toString(paramsVector);

  this->window = this->root->createRenderWindow( "WindowName", width, height, false, &params);
}

int OgreAdaptor::Render()
{
  Ogre::WindowEventUtilities::messagePump();

  root->renderOneFrame();

  return 0;
}

// Load plugins
void OgreAdaptor::LoadPlugins(XMLConfigNode *node)
{
  std::string pathStr;
  std::string pluginStr;
  XMLConfigNode *pluginNode;

  if (!node)
  {
    gzthrow( "missing plugins xml node" );
  }

  // Get the path prefix
  pathStr = node->GetString("path","/usr/local/lib/OGRE",1);

  // Make sure a path has been specified
  if (pathStr.empty())
  {
    gzthrow( "no Plugin Path Set" );
  }

  // The first plugin
  pluginNode = node->GetChild();

  // Read all the plugins
  while (pluginNode)
  {
    pluginStr = pathStr + "/" + pluginNode->GetValue();
    gzmsg(5) << "OGRE: Load Plugin[" << pluginStr << "]\n";

    // Load the plugin into OGRE
    this->root->loadPlugin(pluginStr);
    pluginNode = pluginNode->GetNext();
  }
}
