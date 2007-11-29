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
/* Desc: FLTK Mainwindow
 * Author: Nate Koenig
 * Date: 13 Feb 2006
 * SVN: $Id:$
 */

#include <string>

#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Choice.H>

#include "FLTKGui.hh"
#include "Global.hh"
#include "GuiFactory.hh"
#include "InputHandler.hh"
#include "MainMenu.hh"
#include "Toolbar.hh"
#include "FLTKMainWindow.hh"

using namespace gazebo;

GZ_REGISTER_STATIC_GUI("fltk", FLTKMainWindow);


////////////////////////////////////////////////////////////////////////////////
/// Constructor
FLTKMainWindow::FLTKMainWindow (int x, int y, int width, int height, const std::string &t)
  : Gui(), Fl_Window(x, y, width+200, height+30, t.c_str())
{

  Fl::scheme("plastic");

  // Create a main menu
  MainMenu *mainMenu = new MainMenu(0,0,w(),30,"MainMenu");

  // Create the toolbar
  this->toolbar = new Toolbar(w()-200, 30, 200, h()-30);

  // Create the Rendering window
  this->glWindow = new FLTKGui(0, 30, w()-200, h()-30,"GL Window");

  this->end();
  this->show();

  this->glWindow->Init();

  this->display = this->glWindow->display;
  this->visual = this->glWindow->visual;
  this->colormap = this->glWindow->colormap;
  this->windowId = this->glWindow->windowId;
  
  this->resizable(this->glWindow);
  
}

////////////////////////////////////////////////////////////////////////////////
/// Destructor
FLTKMainWindow::~FLTKMainWindow()
{
  delete this->glWindow;
}

////////////////////////////////////////////////////////////////////////////////
/// Initalize the gui
void FLTKMainWindow::Init()
{
}

////////////////////////////////////////////////////////////////////////////////
void FLTKMainWindow::Update()
{
  this->toolbar->Update();
}

////////////////////////////////////////////////////////////////////////////////
/// Get the width of the gui's rendering window
unsigned int FLTKMainWindow::GetWidth() const
{
  return this->glWindow->w();
}

////////////////////////////////////////////////////////////////////////////////
/// Get the height of the gui's rendering window
unsigned int FLTKMainWindow::GetHeight() const
{
  return this->glWindow->h();
}

