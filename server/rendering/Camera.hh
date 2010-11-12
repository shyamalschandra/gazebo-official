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
/* Desc: A persepective OGRE Camera
 * Author: Nate Koenig
 * Date: 15 July 2003
 * CVS: $Id: Camera.hh 8937 2010-10-11 16:59:55Z natepak $
 */

#ifndef CAMERASENSOR_HH
#define CAMERASENSOR_HH

#include <OgrePrerequisites.h>
#include <OgreTexture.h>

#include "Param.hh"
#include "Angle.hh"
#include "Pose3d.hh"
#include "Time.hh"

// Forward Declarations
namespace Ogre
{
  class TexturePtr;
  class RenderTarget;
  class Camera;
  class Viewport;
  class SceneNode;
}

namespace gazebo
{
  class XMLConfigNode;
  class Model;
  class Entity;
  class MouseEvent;
  class ViewController;
  class Scene;

  /// \addtogroup gazebo_rendering
  /// \brief Basic camera 
  /// \{
  /// \defgroup gazebo_camera Camera
  /// \brief Basic camera sensor
  // \{
  
  
  /// \brief Basic camera sensor
  ///
  /// This is the base class for all cameras.
  class Camera 
  {
    /// \brief Constructor
    public: Camera(const std::string &namePrefix, Scene *scene);
  
    /// \brief Destructor
    public: virtual ~Camera();
  
    /// \brief Load the camera using parameter from an XMLConfig node
    /// \param node The XMLConfig node
    public: void Load( XMLConfigNode *node );

    /// \brief Save camera info in xml format
    /// \param stream Output stream
    public: void Save(std::string &prefix, std::ostream &stream);
  
    /// \brief Initialize the camera
    public: void Init();

    /// \brief Render the camera
    public: virtual void Render();

    /// \brief Post render
    public: virtual void PostRender();

    /// \brief Update the sensor information
    public: void Update();
  
    /// Finalize the camera
    public: void Fini();

    /// \brief Set to true to enable rendering
    public: void SetRenderingEnabled(bool value);

    /// \brief Get whether the rendering is enabled
    public: bool GetRenderingEnabled() const;


    /// \brief Get the global pose of the camera
    public: Pose3d GetWorldPose() const;

    /// \brief Get the camera position in the world
    public: Vector3 GetWorldPosition() const;


    /// \brief Set the global pose of the camera
    public: void SetWorldPose(const Pose3d &pose);

    /// \brief Set the world position
    public: void SetWorldPosition(const Vector3 &pos);

    /// \brief Set the world orientation
    public: void SetWorldRotation(const Quatern &quant);

  
    /// \brief Translate the camera
    public: void Translate( const Vector3 &direction );
  
    /// \brief Rotate the camera around the yaw axis
    public: void RotateYaw( float angle );
  
    /// \brief Rotate the camera around the pitch axis
    public: void RotatePitch( float angle );



    /// \brief Set the clip distances
    public: void SetClipDist(float near, float far);

    /// \brief Set the camera FOV (horizontal)  
    public: void SetFOV( float radians );

    /// \brief Get the camera FOV (horizontal)  
    public: Angle GetHFOV() const;

    /// \brief Get the camera FOV (vertical)  
    public: Angle GetVFOV() const;
  
    /// \brief Get the width of the image
    public: unsigned int GetImageWidth() const;
  
    /// \brief Get the width of the texture 
    public: unsigned int GetTextureWidth() const;
  
    /// \brief Get the height of the image
    public: unsigned int GetImageHeight() const;
  
    /// \brief Get the height of the image
    public: int GetImageDepth() const;

    /// \brief Get the height of the image
    public: std::string GetImageFormat() const;

    /// \brief Get the height of the texture 
    public: unsigned int GetTextureHeight() const;
  
    /// \brief Get the image size in bytes
    public: size_t GetImageByteSize() const;
  
    /// \brief Get the Z-buffer value at the given image coordinate.
    ///
    /// \param x, y Image coordinate; (0, 0) specifies the top-left corner.
    /// \returns Image z value; note that this is abitrarily scaled and
    /// is @e not the same as the depth value.
    public: double GetZValue(int x, int y);
  
    /// \brief Get the near clip distance
    public: double GetNearClip();
  
    /// \brief Get the far clip distance
    public: double GetFarClip();
  
    /// \brief Enable or disable saving
    public: void EnableSaveFrame(bool enable);
 
    /// \brief Set the save frame pathname
    public: void SetSaveFramePathname(const std::string &pathname);

    /// \brief Toggle saving of frames
    public: void ToggleSaveFrame();
  
    /// \brief Get a pointer to the ogre camera
    public: Ogre::Camera *GetCamera() const;

    /// \brief Get the viewport width in pixels
    public: unsigned int GetViewportWidth() const;

    /// \brief Get the viewport height in pixels
    public: unsigned int GetViewportHeight() const;

    /// \brief Get the viewport up vector
    public: Vector3 GetUp();

    /// \brief Get the viewport right vector
    public: Vector3 GetRight();

    /// \brief Get the average FPS
    public: virtual float GetAvgFPS() { return 0;}

    /// \brief Get the triangle count
    public: virtual unsigned int GetTriangleCount() {return 0;}

    /// \brief Set the aspect ratio
    public: void SetAspectRatio( float ratio );

    /// \brief Set whether the user can move the camera via the GUI
    public: void SetUserMovable( bool movable );

    /// \brief Get whether the user can move the camera via the GUI
    public: bool GetUserMovable() const;

    /// \brief Set the camera's scene node
    public: void SetSceneNode( Ogre::SceneNode *node );

    /// \brief Get the camera's scene node
    public: Ogre::SceneNode *GetSceneNode() const;

    /// \brief Get a pointer to the image data
    public: virtual const unsigned char *GetImageData(unsigned int i=0);

    /// \brief Get the camera's name
    public: std::string GetName() const;

    /// \brief Set the camera's name
    public: void SetName( const std::string &name );
           
    /// \brief Toggle whether to view the world in wireframe
    public: void ToggleShowWireframe();

    /// \brief Set whether to view the world in wireframe
    public: void ShowWireframe(bool s);

    /// \brief Get a world space ray as cast from the camer through the viewport
    public: void GetCameraToViewportRay(int screenx, int screeny,
                                        Vector3 &origin, Vector3 &dir);

        /// \brief Set whether to capture data
    public: void SetCaptureData( bool value );

    /// \brief Set the render target
    public: void CreateRenderTexture( const std::string &textureName );

    /// \brief Get the scene this camera is in
    public: Scene *GetScene() const;

    /// \brief Get point on a plane
    public: Vector3 GetWorldPointOnPlane(int x, int y, Vector3 planeNorm, double d);

    /// \brief if user requests bayer image, post process rgb from ogre to generate bayer formats
    private: void ConvertRGBToBAYER(unsigned char* dst, unsigned char* src, std::string format,int width, int height);

    // Save the camera frame
    protected: virtual void SaveFrame();

    // Create the ogre camera
    private: void CreateCamera();

    private: std::string name;

    protected: ParamT<Angle> *hfovP;
    protected: ParamT<double> *nearClipP, *farClipP, *updateRateP;
    protected: ParamT< Vector2<int> > *imageSizeP;
    protected: unsigned int textureWidth, textureHeight;
  
    protected: Ogre::Camera *camera;
    protected: Ogre::Viewport *viewport;
    protected: Ogre::SceneNode *origParentNode;
    protected: Ogre::SceneNode *sceneNode;
    protected: Ogre::SceneNode *pitchNode;
  
    private: Pose3d pose;
  
    // Info for saving images
    protected: unsigned char *saveFrameBuffer;
    protected: unsigned char *bayerFrameBuffer;
    protected: unsigned int saveCount;
    protected: ParamT<bool> *saveFramesP;
    protected: ParamT<std::string> *savePathnameP;
    protected: ParamT<std::string> *imageFormatP;
 
    protected: ParamT<std::string> *visMaskP;
    protected: Ogre::PixelFormat imageFormat;
    protected: unsigned int visibilityMask;

    protected: Ogre::RenderTarget *renderTarget;

    protected: Ogre::TexturePtr renderTexture;

    private: static unsigned int cameraCounter;
    private: unsigned int myCount;

    protected: std::string uniqueName;

    protected: bool captureData;

    private: bool userMovable;
    protected: std::vector<Param*> camParameters;

    protected: bool renderingEnabled;

    private: bool newData;

    protected: Time renderPeriod;
    protected: Time lastUpdate;

    protected: Scene *scene;
  };
  
  /// \}
  /// \}
}
#endif

