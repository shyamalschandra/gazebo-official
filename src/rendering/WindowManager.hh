/*
 * Copyright 2011 Nate Koenig & Andrew Howard
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/
#ifndef WINDOWMANAGER_HH
#define WINDOWMANAGER_HH

#include "SingletonT.hh"
#include "Event.hh"
#include <string>
#include <vector>

namespace Ogre
{
  class RenderWindow;
}

namespace gazebo
{
	namespace rendering
{
  class RenderControl;
  class Camera;

  class WindowManager : public SingletonT<WindowManager>
  {
    public: WindowManager();
    public: virtual ~WindowManager();

    public: int CreateWindow( RenderControl *control );

    public: int CreateWindow( std::string ogreHandle, 
                              unsigned int width, 
                              unsigned int height );


    /// \brief Attach a camera to a window
    public: void SetCamera( int windowId, Camera *camera);

    /// \brief Resize a window
    public: void Resize(unsigned int id, int width, int height);

    public: void Render();

    private: std::vector<Ogre::RenderWindow *> windows;

    private: static unsigned int windowCounter;

    private: event::ConnectionPtr renderConnection;

    private: friend class DestroyerT<WindowManager>;
    private: friend class SingletonT<WindowManager>;
  };
}
}
#endif
