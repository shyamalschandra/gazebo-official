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
/* Desc: Toolbar
 * Author: Nate Koenig
 * Date: 13 Feb 2006
 * SVN: $Id$
 */

#ifndef TOOLBAR_HH
#define TOOLBAR_HH

#include <FL/Fl_Group.H>
#include <FL/Fl_Hold_Browser.H>

class Fl_Value_Output;
class Fl_Output;
class Fl_Input;
class Fl_Button;

namespace gazebo
{
  class Common;

  /// \brief Toolbar
  class Toolbar : public Fl_Group
  {
    /// \brief Constructor
    public: Toolbar (int x, int y, int w, int h, const char *l=0);
  
    /// \brief Destructor
    public: virtual ~Toolbar();

    /// \brief Update the toolbar data
    public: void Update();

    /// \brief Callback for the parameter browser
    public: static void ParamBrowserCB( Fl_Widget * w, void *data);

    /// \brief Callback for parameter modification
    public: static void ParamInputCB( Fl_Widget * w, void *data);

    /// \brief Callback for entity browser
    public: static void EntityBrowserCB( Fl_Widget *w, void *data );

    /// \brief Add an entity ot the param browser
    private: void AddEntityToParamBrowser(Common *ent, std::string prefix);

    /// \brief Add a line to the param browser
    private: void AddToParamBrowser(const std::string &line);


    /// \brief Update entity browser
    private: void UpdateEntityBrowser();

    private: Fl_Hold_Browser *entityBrowser;

    private: Fl_Hold_Browser *paramBrowser;
    private: Fl_Input *paramInput;
    private: std::string paramInputLbl;

    private: int paramColumnWidths[3];
    private: int paramCount;
  };
  
}


#endif

