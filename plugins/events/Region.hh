/*
 * Copyright (C) 2015 Open Source Robotics Foundation
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
#ifndef _GAZEBO_PLUGIN_EVENTS_REGION_HH_
#define _GAZEBO_PLUGIN_EVENTS_REGION_HH_

#include <sdf/sdf.hh>
#include "gazebo/math/Vector3.hh"
#include "gazebo/math/Box.hh"

namespace gazebo
{
  /// \brief A region, made of a list of volumes
  class Region
  {
    /// \brief Constructor
    public: Region() = default;

    /// \brief Destructor
    public: virtual ~Region() = default;

    /// \brief Load from a world file (inside a SimEvent plugin element)
    /// \param[in] _sdf the region element
    public: void Load(const sdf::ElementPtr &_sdf);

    /// \brief Checks if a point lies inside the region
    /// \param[in] _p point
    public: bool Contains(const math::Vector3 &_p) const;

    /// \brief Output operator to print a region to the console.
    /// \param[in] _out the output stream.
    /// \param[in] _region the instance to write out.
    public: friend std::ostream& operator<<(std::ostream &_out,
                                            const Region &_region);

    /// \brief name of the region (as defined in the world file)
    public: std::string name;

    /// \brief The list of volumes inside this region
    public: std::vector<math::Box> boxes;
  };

  /// \def RegionPtr
  /// \brief Shared pointer to a region
  typedef std::shared_ptr<Region> RegionPtr;
}
#endif
