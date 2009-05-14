#include "Gui.hh"
#include "XMLConfig.hh"
#include "GuiAPI.hh"

using namespace gazebo;

////////////////////////////////////////////////////////////////////////////////
// Constructor
GuiAPI::GuiAPI(int x, int y, int w, int h, const std::string &t)
{
  this->gui = new Gui(x,y,w,h,t);
}

////////////////////////////////////////////////////////////////////////////////
// Destructor
GuiAPI::~GuiAPI()
{
  delete this->gui;
}

////////////////////////////////////////////////////////////////////////////////
/// Load the gui
void GuiAPI::Load(XMLConfigNode *node)
{
  this->gui->Load(node);
}

////////////////////////////////////////////////////////////////////////////////
/// Save the gui params in xml format
void GuiAPI::Save(std::string &prefix, std::ostream &stream)
{
  this->gui->Save(prefix, stream);
}

////////////////////////////////////////////////////////////////////////////////
/// Initalize the gui
void GuiAPI::Init()
{
  this->gui->Init();
}

////////////////////////////////////////////////////////////////////////////////
// Update 
void GuiAPI::Update()
{
  this->gui->Update();
}
