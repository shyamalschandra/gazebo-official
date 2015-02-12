#include "gazebo/gui/Example.hh"

using namespace gazebo;
using namespace gui;

MyExample::~MyExample()
{
}

void MyExample::Load()
{
  printf("Loaded\n");
}

Q_EXPORT_PLUGIN2(myexample, gazebo::gui::MyExample)
