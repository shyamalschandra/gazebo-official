#include "OgreCamera.hh"
#include "ViewController.hh"

using namespace gazebo;

////////////////////////////////////////////////////////////////////////////////
// Constructor
ViewController::ViewController(OgreCamera *cam)
  : camera(cam)
{
}

////////////////////////////////////////////////////////////////////////////////
/// Destructor
ViewController::~ViewController()
{
  this->camera = NULL;
}
