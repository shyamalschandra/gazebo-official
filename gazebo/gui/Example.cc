#include "gazebo/gui/Example.hh"
#include "gazebo/gui/GuiIface.hh"

using namespace gazebo;
using namespace gui;

MyExample::~MyExample()
{
}

void MyExample::Load(sdf::ElementPtr /*_elem*/)
{
  printf("Loaded\n");
  gui::get_active_camera();
}

Q_EXPORT_PLUGIN2(myexample, gazebo::gui::MyExample)
