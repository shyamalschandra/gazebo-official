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
/* Desc: Ogre Visual Class
 * Author: Nate Koenig
 * Date: 14 Dec 2007
 */

#ifndef OGREVISUAL_HH
#define OGREVISUAL_HH

#include <string>

#include "common/Event.hh"
#include "math/Box.hh"
#include "math/Pose.hh"
#include "math/Quaternion.hh"
#include "math/Vector3.hh"
#include "math/Vector2d.hh"
#include "common/Param.hh"
#include "common/Messages.hh"

#include "sdf/interface/sdf.h"
#include "rendering/RenderTypes.hh"

namespace Ogre
{
  class MovableObject;
  class SceneNode;
  class StaticGeometry;
  class RibbonTrail;
  class AxisAlignedBox;
}

namespace gazebo
{
  namespace common
  {
    class Mesh;
  }

	namespace rendering
  { 
    class SelectionObj;
    class Scene;
    class DynamicLines;
  
    /// \brief Ogre Visual Object
    class Visual 
    {
      /// \brief Constructor
      public: Visual (const std::string &name, Visual *parent);
  
      /// \brief Constructor
      public: Visual (const std::string &name, Ogre::SceneNode *parent);
  
      /// \brief Constructor
      public: Visual (const std::string &name, Scene *scene);
  
      /// \brief Destructor
      public: virtual ~Visual();
  
      /// \brief Helper for the contructor
      public: void Init();
  
      /// \brief Load from a message
      public: void LoadFromMsg(const boost::shared_ptr< msgs::Visual const> &msg);
  
      /// \brief Load the visual with a set of parameters
      public: void Load( sdf::ElementPtr &sdf );

      /// \brief Load the visual with default parameters
      public: void Load();
  
      /// \brief Update the visual.
      public: void Update();

      /// \brief Set the name of the visual
      public: void SetName( const std::string &name );

      /// \brief Get the name of the visual
      public: std::string GetName() const;
  
      /// \brief Attach a visual
      public: void AttachVisual(Visual *vis);
             
      /// \brief Detach a visual 
      public: void DetachVisual(Visual *vis);
  
      /// \brief Attach a renerable object to the visual
      public: void AttachObject( Ogre::MovableObject *obj);
  
      /// \brief Detach all objects
      public: void DetachObjects();
  
      /// \brief Get the number of attached objects
      public: unsigned short GetNumAttached();
  
      /// \brief Get an attached object
      public: Ogre::MovableObject *GetAttached(unsigned short num);
  
      /// \brief Attach a mesh to this visual by name
      public: void AttachMesh( const std::string &meshName );
  
      /// \brief Set the scale
      public: void SetScale( const math::Vector3 &scale );
  
      /// \brief Get the scale
      public: math::Vector3 GetScale();
  
      /// \brief Set the material
      public: void SetMaterial(const std::string &materialName);

      /// \brief Set the color of the visual
      public: void SetColor(const common::Color &_color);
  
      public: void AttachAxes();
  
      /// \brief Set the transparency
      public: void SetTransparency( float trans );
  
      /// \brief Get the transparency
      public: float GetTransparency();
  
      /// \brief Set highlighted or no
      public: void SetHighlight( bool highlight);
  
      /// \brief Set whether the visual should cast shadows
      public: void SetCastShadows(const bool &shadows);
  
      /// \brief Set whether the visual is visible
      /// \param visible set this node visible
      /// \param cascade setting this parameter in children too
      public: void SetVisible(bool visible, bool cascade=true);
  
      /// \brief Toggle whether this visual is visible
      public: void ToggleVisible();
  
      /// \brief Get whether the visual is visible
      public: bool GetVisible() const;
  
      /// \brief Set the position of the visual
      public: void SetPosition( const math::Vector3 &pos);
  
      /// \brief Set the rotation of the visual
      public: void SetRotation( const math::Quaternion &rot);
  
      /// \brief Set the pose of the visual
      public: void SetPose( const math::Pose &pose);
  
      /// \brief Get the position of the visual
      public: math::Vector3 GetPosition() const;
  
      /// \brief Get the rotation of the visual
      public: math::Quaternion GetRotation() const;
  
      /// \brief Get the pose of the visual
      public: math::Pose GetPose() const;
  
      /// \brief Get the global pose of the node
      public: math::Pose GetWorldPose() const;
  
      /// \brief Return the scene Node of this visual entity
      public: Ogre::SceneNode *GetSceneNode() const;
  
      /// \brief Make the visual objects static renderables
      public: void MakeStatic();
  
      /// \brief Return true if the  visual is a static geometry
      public: bool IsStatic() const;
  
      /// \brief Set one visual to track/follow another
      public: void EnableTrackVisual( Visual *vis );
  
      /// \brief Disable tracking of a visual
      public: void DisableTrackVisual();
  
      /// \brief Get the normal map
      public: std::string GetNormalMap() const;
  
      /// \brief Set the normal map
      public: void SetNormalMap(const std::string &nmap);
  
      /// \brief True on or off a ribbon trail
      public: void SetRibbonTrail(bool value);
  
      /// \brief Get the size of the bounding box
      public: math::Vector3 GetBoundingBoxSize() const;
  
      /// \brief Set whether to use the RT Shader system
      public: void SetUseRTShader(bool value);
  
      /// \brief Get whether to user the RT shader system
      public: bool GetUseRTShader() const;
  
      /// \brief Add a line to the visual
      public: DynamicLines *AddDynamicLine(RenderOpType type);
  
      /// \brief Delete a dynamic line
      public: void DeleteDynamicLine(DynamicLines *line);
  
      /// \brief Get the name of the material
      public: std::string GetMaterialName() const;
  
      /// \brief Get the bounding box for the visual
      public: math::Box GetBounds() const;
  
      /// \brief Insert a mesh into Ogre 
      public: static void InsertMesh( const common::Mesh *mesh);
  
      /// \brief Update a visual based on a message
      public: void UpdateFromMsg(const boost::shared_ptr< msgs::Visual const> &msg);
  
      private: void GetBoundsHelper(Ogre::SceneNode *node, math::Box &box) const;
      private: std::string GetMeshName() const;
  
      private: sdf::ElementPtr sdf;

      private: std::string myMaterialName;
      private: std::string origMaterialName;
  
      private: Ogre::SceneNode *sceneNode;
  
      private: float transparency;
  
      private: static unsigned int visualCounter;
  
      private: bool isStatic;
      private: Ogre::StaticGeometry *staticGeom;
      private: bool visible;
  
      private: static SelectionObj *selectionObj;
  
      private: Ogre::RibbonTrail *ribbonTrail;
  
      private: bool useRTShader;
      private: event::ConnectionPtr preRenderConnection;
  
      // List of all the lines created
      private: std::list<DynamicLines*> lines;

      private: std::string name;

      /// List of all the parameters
      protected: std::vector<common::Param*> parameters;
    };
  }

}
#endif
