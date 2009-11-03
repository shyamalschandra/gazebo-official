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
/* Desc: Geom class
 * Author: Nate Koenig
 * Date: 13 Feb 2006
 * SVN: $Id$
 */

#ifndef GEOM_HH
#define GEOM_HH

#include "Shape.hh"
#include "Param.hh"
#include "Entity.hh"
#include "Pose3d.hh"
#include "Vector3.hh"
#include "Mass.hh"

namespace gazebo
{
  class Model;
  class Body;
  class ContactParams;
  class XMLConfigNode;
  class OgreVisual;
  class PhysicsEngine;

  /// \addtogroup gazebo_physics
  /// \brief Base class for all geoms
  /// \{

  /// \brief Base class for all geoms
  class Geom : public Entity
  {
    /// \brief Constructor
    //public: Geom(Body *body, const std::string &name);
    public: Geom(Body *body);
  
    /// \brief Destructor
    public: virtual ~Geom();

    /// \brief Load the geom
    public: void Load(XMLConfigNode *node);

    /// \brief Load the geom
    public: void Save(std::string &prefix, std::ostream &stream);
 
    /// \brief Set the encapsulated geometry object
    public: void SetGeom(bool placeable);
  
    /// \brief Update function for geoms
    public: void Update();
 
    /// \brief Return whether this geom is placeable
    public: bool IsPlaceable() const;
    
    /// \brief Set the category bits, used during collision detection
    /// \param bits The bits
    public: virtual void SetCategoryBits(unsigned int bits) = 0;
  
    /// \brief Set the collide bits, used during collision detection
    /// \param bits The bits
    public: virtual void SetCollideBits(unsigned int bits) = 0;
  
    /// \brief Get the mass of the geom
    public: virtual Mass GetBodyMassMatrix() = 0;
  
    /// \brief Set the laser fiducial integer id
    public: void SetLaserFiducialId(int id);
  
    /// \brief Get the laser fiducial integer id
    public: int GetLaserFiducialId() const;
  
    /// \brief Set the laser retro reflectiveness 
    public: void SetLaserRetro(float retro);
  
    /// \brief Get the laser retro reflectiveness 
    public: float GetLaserRetro() const;
 
    /// \brief Set the visibility of the bounding box
    public: void ShowBoundingBox(bool show);

    /// \brief Set the visibility of the joints
    public: void ShowJoints(bool show);

    /// \brief Set the visibility of the physical entity of this geom
    public: void ShowPhysics(bool);

    /// \brief Set the mass
    public: void SetMass(const double &mass);

    /// \brief Set the mass
    public: void SetMass(const Mass &mass);

    /// \brief Get the number of visuals
    public: unsigned int GetVisualCount() const;

    /// \brief Get a visual
    public: OgreVisual *GetVisual(unsigned int index) const; 

    /// \brief Get a visual by id
    public: OgreVisual *GetVisualById( int id ) const;

    /// \brief Get the body this geom belongs to
    public: Body *GetBody() const;

    /// \brief Get the model this geom belongs to
    public: Model *GetModel() const;

    /// \brief Set the friction mode of the geom
    public: void SetFrictionMode( const bool &v );

    /// \brief Get the bounding box for this geom
    public: virtual void GetBoundingBox(Vector3 &min, Vector3 &max) const = 0;

    /// \brief Get a pointer to the mass
    public: const Mass &GetMass() const;

    /// \brief Get the shape type
    public: Shape::Type GetType();

    /// \brief Set the shape for this geom
    public: void SetShape(Shape *shape);
            
    /// \brief Get the attached shape
    public: Shape *GetShape() const;

    /// \brief Create the bounding box for the geom
    private: void CreateBoundingBox();

    ///  Contact parameters
    public: ContactParams *contact; 
 
    /// The body this geom belongs to
    protected: Body *body;
  
    protected: bool placeable;

    protected: Mass mass;

    private: ParamT<int> *laserFiducialIdP;
    private: ParamT<float> *laserRetroP;

    ///  Mass as a double
    private: ParamT<double> *massP;

    protected: ParamT<Vector3> *xyzP;
    protected: ParamT<Quatern> *rpyP;

    /// Special bounding box visual
    private: OgreVisual *bbVisual;

    private: float transparency;

    /// All the visual apparence 
    private: std::vector<OgreVisual*> visuals;

    ///our XML DATA
    private: XMLConfigNode *xmlNode;

    private: std::string typeName;

    protected: PhysicsEngine *physicsEngine;

    protected: Shape *shape;
  };

  /// \}

}
#endif
