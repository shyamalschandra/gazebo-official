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
/* Desc: A ray
 * Author: Nate Keonig, Andrew Howard
 * Date: 14 Oct 2009
 * SVN: $Id:$
 */

#ifndef ODERAYSHAPE_HH
#define ODERAYSHAPE_HH

#include "RayShape.hh"
#include "Shape.hh"

namespace gazebo
{
  /// \addtogroup gazebo_physics_geom
  /// \{
  /** \defgroup gazebo_ray_geom Ray geom
      \brief Ray geom

      This geom is used soley by ray sensors. It should not be directly included in a world file.
  */
  /// \}
  /// \addtogroup gazebo_ray_geom 
  /// \{

  /// \brief Ray geom 
  class ODERayShape : public RayShape
  {
    /// \brief Constructor
    /// \param body Body the ray is attached to
    /// \param displayRays Indicates if the rays should be displayed when rendering images
    public: ODERayShape( Geom *geom, bool displayRays );
  
    /// \brief Destructor
    public: virtual ~ODERayShape();

    /// \brief Update the tay geom
    public: virtual void Update();
 
    /// \brief Set the ray based on starting and ending points relative to the 
    ///        body
    /// \param posStart Start position, relative the body
    /// \param posEnd End position, relative to the body
    public: virtual void SetPoints(const Vector3 &posStart, 
                                   const Vector3 &posEnd);
  };
  
  /// \}
}

#endif
