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
#include "sensors/CameraSensor.hh"
#include "rendering/Camera.hh"
#include "gazebo.h"

namespace gazebo
{
  class CameraPlugin : public SensorPlugin
  {
    public: CameraPlugin();

    public: void Load( sensors::SensorPtr &_sensor, sdf::ElementPtr &_sdf );

    public: virtual void OnNewFrame(const unsigned char *_image,
                              unsigned int _width, unsigned int _height, 
                              unsigned int _depth, const std::string &_format);

    protected: unsigned int width, height, depth;
    protected: std::string format;

    protected: sensors::CameraSensorPtr parentSensor;
    protected: rendering::CameraPtr camera;

    private: event::ConnectionPtr newFrameConnection;
  };
}
