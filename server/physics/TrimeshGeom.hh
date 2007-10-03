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
/* Desc: Trimesh geometry
 * Author: Nate Keonig, Andrew Howard
 * Date: 8 May 2003
 * CVS: $Id$
 */

#ifndef TRIMESHGEOM_HH
#define TRIMESHGEOM_HH

#include "Geom.hh"

namespace gazebo
{
  /// \addtogroup gazebo_physics_geom
  /// \brief Trimesh geom
  /// \{
  /// \defgroup gazebo_trimesh_geom Triangle Mesh geom
  /// \brief Trimesh geom
  /// \{


  /// \brief Triangle mesh geom
  class TrimeshGeom : public Geom
  {
    /// \brief Constructor
//    public: TrimeshGeom(Body *body, const std::string &name, double mass, const std::string &meshName, const Vector3 &scale);
    public: TrimeshGeom(Body *body);

    /// \brief Destructor
    public: virtual ~TrimeshGeom();

    /// \brief Update function 
    public: void Update();

    /// \brief Load the trimesh
    protected: virtual void LoadChild(XMLConfigNode *node);

    private: dTriMeshDataID odeData;

    private: dReal matrix_dblbuff[16*2];
    private: int last_matrix_index;
  };

  /// \}
  /// \}
}

#endif
