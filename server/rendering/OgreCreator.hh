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
/* Desc: Some functions that creates Ogre objects together
 * Author: Jordi Polo	
 * Date: 27 Dec 2007
 */

#ifndef OGRECREATOR
#define OGRECREATOR


//#include <X11/Xlib.h>
//#include <X11/Xutil.h>

#include <string>
#include "Vector3.hh"
#include "Vector2.hh"

namespace Ogre
{
  class Camera;
  class RenderTarget;
  class RenderWindow;
}

class Fl_Window;

namespace gazebo
{
  class XMLConfigNode;
  class Entity;
  class OgreVisual;

/// \addtogroup gazebo_rendering
/// \{


   /// \brief Functions that creates Ogre3d objects
  class OgreCreator
  {

    /// \brief Constructor
    public: OgreCreator();

    /// \brief Destructor
    public: virtual ~OgreCreator();

    /// \brief Load some simple shapes on the render engine
      public: static void LoadBasicShapes();

    /// \brief Create a Plane
    /// It adds itself to the Visual node parent, it will those change parent
    /// properties if needed, to avoid this create a child visual node for the 
    /// plane
    public: static std::string CreatePlane(const Vector3 &normal, 
                const Vector2<double> &size, const Vector2<double> &segments, 
                const Vector2<double> &uvTile, const std::string &material, 
                bool castShadows, OgreVisual *parent, const std::string &name);
    
    /// \brief Create a light source 
    /// \return The name of the light source
    public: static std::string CreateLight(XMLConfigNode *node, OgreVisual *parent);

    /// \brief Save a light source info in xml format
    public: static void SaveLight(const std::string &prefix, 
                           const std::string lightName, std::ostream &stream);

    /// \brief Helper function to create a camera
    public: static Ogre::Camera *CreateCamera(const std::string &name, 
                double nearClip, double farClip, double hfov, 
                Ogre::RenderTarget *renderTarget);

    /// \brief Helper function to create fog
    public: static void CreateFog(XMLConfigNode *node);

    /// \brief Helper function to save the fog settings
    public: static void SaveFog(std::string &prefix, std::ostream &stream);

    /// \brief Helper function to create the sky 
    public: static void CreateSky(std::string material);

    /// \brief Create a new window
    public: static Ogre::RenderWindow *CreateWindow(Fl_Window *flWindow, 
                                                    unsigned int width, 
                                                    unsigned int height);


    /// \brief Create a window for Ogre
    public: static Ogre::RenderWindow *CreateWindow(long display, 
                                                    int screen, 
                                                    long winId, 
                                                    unsigned int width, 
                                                    unsigned int height);
   
    /// \brief Draw the uniform grid pattern
    public: static void DrawGrid();

    /// \brief Remove a mesh by name
    public: static void RemoveMesh(const std::string &name);

    private: static unsigned int lightCounter;
    private: static unsigned int windowCounter;

/// \}

  };
}
#endif
