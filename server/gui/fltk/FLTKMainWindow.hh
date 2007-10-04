#ifndef FLTKMAINWINDOW_HH
#define FLTKMAINWINDOW_HH

#include <FL/Fl.H>
#include <FL/Fl_Window.H>

#include "Gui.hh"

namespace gazebo
{

  class FLTKGui;

  /// \brief FLTK Main Window
  class FLTKMainWindow : public Gui, public Fl_Window
  {
    /// \brief Constructor
    public: FLTKMainWindow (int x, int y, int w, int h, const std::string &t);
 
    /// \brief Destructor
    public: virtual ~FLTKMainWindow();

    /// \brief Initalize the gui
    public: virtual void Init();

    /// \brief Update the gui
    public: virtual void Update();

    /// \brief Get the width of the gui's rendering window
    public: virtual unsigned int GetWidth() const;

    /// \brief Get the height of the gui's rendering window
    public: virtual unsigned int GetHeight() const;

    //public: virtual void draw();
    //public: virtual void flush();

    public: void resize(int x, int y, int w, int h);
    public: int handle(int event);

    private: FLTKGui *glWindow;
  };

}

#endif
