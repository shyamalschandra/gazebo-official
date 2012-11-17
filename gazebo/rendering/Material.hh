/*
 * Copyright 2012 Nate Koenig
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
#ifndef _RENDERING_MATERIAL_HH_
#define _RENDERING_MATERIAL_HH_

#include "common/Material.hh"

/// \cond
namespace gazebo
{
  namespace rendering
  {

    /// \class Material Material.hh rendering/rendering.hh
    /// \TODO Nate fill in
    class Material
    {
      /// \brief 
      /// \param[in] _mat
      /// \TODO Nate, fill in
      public: static void CreateMaterials();
      public: static void Update(const gazebo::common::Material *_mat);
    };
  }
}
/// \endcond
#endif
