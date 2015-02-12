#ifndef _GUI_EXAMPLE_
#define _GUI_EXAMPLE_

#include "qt.h"
#include "gazebo/gui/GUIPlugin.hh"
namespace gazebo
{
  namespace gui
  {
    class MyExample : public QWidget, GUIPlugin
    {
      Q_OBJECT
      Q_INTERFACES(GUIPlugin)

      public: virtual ~MyExample();
      public: virtual void Load();
    };
  }
}

#endif
