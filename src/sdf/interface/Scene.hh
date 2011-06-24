/*********************************************************************
* Software License Agreement (BSD License)
* 
*  Copyright (c) 2008, Willow Garage, Inc.
*  All rights reserved.
* 
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
* 
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Willow Garage nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
* 
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

/* Author: Nate Koenig, John Hsu */

#ifndef SDF_SCENE_HH
#define SDF_SCENE_HH

#include <string>
#include <boost/shared_ptr.hpp>
#include <iostream>

#include "common/Color.hh"
#include "sdf/interface/Param.hh"

namespace sdf
{
  class Scene
  {
    public: Scene() : ambientColor("rgba", gazebo::common::Color()),
            backgroundColor("rgba", gazebo::common::Color()),
            skyMaterial("material",""),
            shadowEnabled("enabled",true),
            shadowColor("rgba", gazebo::common::Color()),
            shadowType("type",""),
            fogColor("rgba", gazebo::common::Color()),
            fogType("type","linear"),
            fogStart("start",1.0),
            fogEnd("end",100.0),
            fogDensity("density",1.0)
    { this->Clear(); }
  
    public: ParamT<gazebo::common::Color, true> ambientColor;
    public: ParamT<gazebo::common::Color, true> backgroundColor;
    public: ParamT<std::string, false> skyMaterial;

    public: ParamT<bool, false> shadowEnabled;
    public: ParamT<gazebo::common::Color, false> shadowColor;
    public: ParamT<std::string, false> shadowType;

    public: ParamT<gazebo::common::Color, false> fogColor;
    public: ParamT<std::string, false> fogType;
    public: ParamT<double, false> fogStart;
    public: ParamT<double, false> fogEnd;
    public: ParamT<double, false> fogDensity;
  
    public: void Clear()
    {
      this->ambientColor.Reset();
      this->backgroundColor.Reset();
      this->skyMaterial.Reset();
      this->shadowEnabled.Reset();
      this->shadowColor.Reset();
      this->shadowType.Reset();
      this->fogColor;
      this->fogType;
      this->fogStart;
      this->fogEnd;
      this->fogDensity;
    }

    public: void Print( const std::string _prefix)
    {
      std::cout << _prefix << "Scene:\n";
      std::cout << _prefix << "  SkyMaterial[" << this->skyMaterial << "]\n";
      std::cout << _prefix << "  ShadowType[" << this->shadowType << "]\n";

      std::cout << _prefix << "  Ambient Color[" << this->ambientColor << "]\n";

      std::cout << _prefix << "  Background Color[" << this->backgroundColor << "]\n";

      std::cout << _prefix << "  Shadow Color[" << this->shadowColor << "]\n";
      std::cout << _prefix << "  Shadow Enabled[" << this->shadowEnabled << "]\n";
    }
  };
}

#endif
