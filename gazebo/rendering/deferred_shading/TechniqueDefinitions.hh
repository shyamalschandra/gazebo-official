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
#ifndef _TECHNIQUEDEFINITIONS_HH_
#define _TECHNIQUEDEFINITIONS_HH_

#include <string>

namespace gazebo
{
  namespace rendering
  {
    class NullTechnique
    {
      protected: std::string GetMaterialPrefix() const
                 {return "NullTechnique";}
      protected: int GetNumInputs() const
                 {return 0;}
      protected: bool UseMaterialProperties() const
                 {return true;}
    };
    class DeferredShading
    {
      protected: std::string GetMaterialPrefix() const
                 {return "DeferredShading";}
      protected: int GetGBufferSize() const
                 {return 2;}
      protected: bool UseMaterialProperties() const
                 {return true;}
    };
    class DeferredLighting
    {
      protected: std::string GetMaterialPrefix() const
                 {return "DeferredLighting";}
      protected: int GetGBufferSize() const
                 {return 1;}
      protected: bool UseMaterialProperties() const
                 {return false;}
    };
    class InferredLighting
    {
      protected: std::string GetMaterialPrefix() const
                 {return "InferredLighting";}
      protected: int GetGBufferSize() const
                 {return 1;}
      protected: bool UseMaterialProperties() const
                 {return false;}
    };
  }
}
#endif
