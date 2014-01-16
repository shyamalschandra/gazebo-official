/*
 * Copyright (C) 2012-2013 Open Source Robotics Foundation
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
#ifndef MESSAGES_UTILITY_H
#define MESSAGES_UTILITY_H

#include <string>

#include <sdf/sdf.hh>
#include <rml/rml.hh>

#include <robot_msgs/MessageTypes.hh>
#include "gazebo/msgs/MessageTypes.hh"

#include "gazebo/math/MathTypes.hh"
#include "gazebo/math/Vector3.hh"
#include "gazebo/math/Pose.hh"
#include "gazebo/math/Plane.hh"
#include "gazebo/math/Box.hh"

#include "gazebo/common/Image.hh"
#include "gazebo/common/Color.hh"
#include "gazebo/common/Time.hh"

namespace gazebo
{
  /// \ingroup gazebo_msgs Messages
  /// \brief Messages namespace
  namespace msgs
  {
    /// \addtogroup gazebo_msgs Messages
    /// \brief All messages and helper functions
    /// \{

    /// \brief Create a request message
    /// \param[in] _request Request string
    /// \param[in] _data Optional data string
    /// \return A Request message
    msgs::Request *CreateRequest(const std::string &_request,
                                 const std::string &_data = "");

    /// \brief Initialize a message
    /// \param[in] _message Message to initialize
    /// \param[in] _id Optional string id
    void Init(google::protobuf::Message &_message, const std::string &_id ="");

    /// \brief Time stamp a header
    /// \param[in] _header Header to stamp
    void Stamp(msgs::Header *_header);

    /// \brief Set the time in a time message
    /// \param[in] _time A Time message
    void Stamp(robot_msgs::Time *_time);

    /// \cond
    std::string Package(const std::string &type,
        const google::protobuf::Message &message);
    /// \endcond

    /// \brief Convert a math::Vector3 to a robot_msgs::Vector3d
    /// \param[in] _v The vector to convert
    /// \return A robot_msgs::Vector3d object
    robot_msgs::Vector3d      Convert(const math::Vector3 &_v);

    /// \brief Convert a sdf::Vector3 to a robot_msgs::Vector3d
    /// \param[in] _v The vector to convert
    /// \return A robot_msgs::Vector3d object
    robot_msgs::Vector3d      Convert(const sdf::Vector3 &_v);

    /// \brief Convert a math::Quaternion to a robot_msgs::Quaternion
    /// \param[in] _q The quaternion to convert
    /// \return A robot_msgs::Quaternion object
    robot_msgs::Quaternion Convert(const math::Quaternion &_q);

    /// \brief Convert a sdf::Quaternion to a robot_msgs::Quaternion
    /// \param[in] _q The quaternion to convert
    /// \return A robot_msgs::Quaternion object
    robot_msgs::Quaternion Convert(const sdf::Quaternion &_q);

    /// \brief Convert a math::Pose to a robot_msgs::Pose
    /// \param[in] _p The pose to convert
    /// \return A robot_msgs::Pose object
    robot_msgs::Pose       Convert(const math::Pose &_p);

    /// \brief Convert a sdf::Pose to a robot_msgs::Pose
    /// \param[in] _p The pose to convert
    /// \return A robot_msgs::Pose object
    robot_msgs::Pose       Convert(const sdf::Pose &_p);

    /// \brief Convert a common::Color to a robot_msgs::Color
    /// \param[in] _c The color to convert
    /// \return A robot_msgs::Color object
    robot_msgs::Color      Convert(const common::Color &_c);

    /// \brief Convert a sdf::Color to a robot_msgs::Color
    /// \param[in] _c The color to convert
    /// \return A robot_msgs::Color object
    robot_msgs::Color      Convert(const sdf::Color &_c);

    /// \brief Convert a common::Time to a robot_msgs::Time
    /// \param[in] _t The time to convert
    /// \return A robot_msgs::Time object
    robot_msgs::Time       Convert(const common::Time &_t);

    /// \brief Convert a math::Plane to a msgs::PlaneGeom
    /// \param[in] _p The plane to convert
    /// \return A msgs::PlaneGeom object
    msgs::PlaneGeom Convert(const math::Plane &_p);

    /// \brief Convert a robot_msgs::Vector3d to a math::Vector
    /// \param[in] _v The plane to convert
    /// \return A math::Vector3 object
    math::Vector3    Convert(const robot_msgs::Vector3d &_v);

    /// \brief Convert a robot_msgs::Quaternion to a math::Quaternion
    /// \param[in] _q The quaternion to convert
    /// \return A math::Quaternion object
    math::Quaternion Convert(const robot_msgs::Quaternion &_q);

    /// \brief Convert a robot_msgs::Pose to a math::Pose
    /// \param[in] _q The pose to convert
    /// \return A math::Pose object
    math::Pose       Convert(const robot_msgs::Pose &_p);

    /// \brief Convert a robot_msgs::Image to a common::Image
    /// \param[out] _img The common::Image container
    /// \param[in] _msg The Image message to convert
    void Set(common::Image &_img, const robot_msgs::Image &_msg);

    /// \brief Convert common::Image::PixelFormat to
    /// robot_msgs::Image::Format
    /// \param[in] _fmt common::Image::PixelFormat value
    /// \return Equivalent robot_msgs::Image::Format.
    robot_msgs::Image::Format Convert(common::Image::PixelFormat _fmt);

    /// \brief Convert robot_msgs::Image::Format to 
    /// common::Image::PixelFormat
    /// \param[in] _fmt robot_msgs::Image::Format value
    /// \return Equivalent common::Image::PixelFormat.
    common::Image::PixelFormat Convert(robot_msgs::Image::Format _fmt);

    /// \brief Convert a robot_msgs::Color to a common::Color
    /// \param[in] _c The color to convert
    /// \return A common::Color object
    common::Color    Convert(const robot_msgs::Color &_c);

    /// \brief Convert a robot_msgs::Time to a common::Time
    /// \param[in] _t The time to convert
    /// \return A common::Time object
    common::Time     Convert(const robot_msgs::Time &_t);

    /// \brief Convert a msgs::PlaneGeom to a common::Plane
    /// \param[in] _p The plane to convert
    /// \return A common::Plane object
    math::Plane      Convert(const msgs::PlaneGeom &_p);

    /// \brief Set a robot_msgs::Image from a common::Image
    /// \param[out] _msg A robot_msgs::Image pointer
    /// \param[in] _i A common::Image reference
    void Set(robot_msgs::Image *_msg, const common::Image &_i);

    /// \brief Set a robot_msgs::Vector3d from a math::Vector3
    /// \param[out] _pt A robot_msgs::Vector3d pointer
    /// \param[in] _v A math::Vector3 reference
    void Set(robot_msgs::Vector3d *_pt, const math::Vector3 &_v);

    /// \brief Set a robot_msgs::Vector3d from a sdf::Vector3
    /// \param[out] _pt A robot_msgs::Vector3d pointer
    /// \param[in] _v A sdf::Vector3 reference
    void Set(robot_msgs::Vector3d *_pt, const sdf::Vector3 &_v);

    /// \brief Set a robot_msgs::Vector2d from a math::Vector2d
    /// \param[out] _pt A robot_msgs::Vector2d pointer
    /// \param[in] _v A math::Vector2d reference
    void Set(robot_msgs::Vector2d *_pt, const math::Vector2d &_v);

    /// \brief Set a robot_msgs::Vector2d from a sdf::Vector2d
    /// \param[out] _pt A robot_msgs::Vector2d pointer
    /// \param[in] _v A sdf::Vector2d reference
    void Set(robot_msgs::Vector2d *_pt, const sdf::Vector2d &_v);

    /// \brief Set a robot_msgs::Quaternion from a math::Quaternion
    /// \param[out] _q A robot_msgs::Quaternion pointer
    /// \param[in] _v A math::Quaternion reference
    void Set(robot_msgs::Quaternion *_q, const math::Quaternion &_v);

    /// \brief Set a robot_msgs::Quaternion from a sdf::Quaternion
    /// \param[out] _q A robot_msgs::Quaternion pointer
    /// \param[in] _v A sdf::Quaternion reference
    void Set(robot_msgs::Quaternion *_q, const sdf::Quaternion &_v);

    /// \brief Set a robot_msgs::Pose from a math::Pose
    /// \param[out] _p A robot_msgs::Pose pointer
    /// \param[in] _v A math::Pose reference
    void Set(robot_msgs::Pose *_p, const math::Pose &_v);

    /// \brief Set a robot_msgs::Pose from a sdf::Pose
    /// \param[out] _p A robot_msgs::Pose pointer
    /// \param[in] _v A sdf::Pose reference
    void Set(robot_msgs::Pose *_p, const sdf::Pose &_v);

    /// \brief Set a robot_msgs::Color from a common::Color
    /// \param[out] _p A robot_msgs::Color pointer
    /// \param[in] _v A common::Color reference
    void Set(robot_msgs::Color *_c, const common::Color &_v);

    /// \brief Set a robot_msgs::Color from a sdf::Color
    /// \param[out] _p A robot_msgs::Color pointer
    /// \param[in] _v A sdf::Color reference
    void Set(robot_msgs::Color *_c, const sdf::Color &_v);

    /// \brief Set a robot_msgs::Time from a common::Time
    /// \param[out] _p A robot_msgs::Time pointer
    /// \param[in] _v A common::Time reference
    void Set(robot_msgs::Time *_t, const common::Time &_v);

    /// \brief Set a msgs::Plane from a math::Plane
    /// \param[out] _p A msgs::Plane pointer
    /// \param[in] _v A math::Plane reference
    void Set(msgs::PlaneGeom *_p, const math::Plane &_v);

    /// \brief Create a msgs::TrackVisual from a track visual SDF element
    /// \param[in] _sdf The sdf element
    /// \return The new msgs::TrackVisual object
    msgs::TrackVisual TrackVisualFromSDF(sdf::ElementPtr _sdf);

    /// \brief Create a msgs::TrackVisual from a track visual RML element
    /// \param[in] _rml The RML element
    /// \return The new msgs::TrackVisual object
    msgs::TrackVisual TrackVisualFromRML(
        const rml::Gui::Camera::Track_Visual &_rml);

    /// \brief Create a msgs::GUI from a GUI SDF element
    /// \param[in] _sdf The sdf element
    /// \return The new msgs::GUI object
    msgs::GUI GUIFromSDF(sdf::ElementPtr _sdf);

    /// \brief Create a msgs::GUI from a GUI RML element
    /// \param[in] _sdf The RML element
    /// \return The new msgs::GUI object
    msgs::GUI GUIFromRML(const rml::Gui &_rml);

    /// \brief Create a msgs::Light from a light SDF element
    /// \param[in] _sdf The sdf element
    /// \return The new msgs::Light object
    msgs::Light LightFromSDF(sdf::ElementPtr _sdf);

    /// \brief Create a msgs::Light from a light RML element
    /// \param[in] _rml The RML element
    /// \return The new msgs::Light object
    msgs::Light LightFromRML(const rml::Light &_rml);

    /// \brief Create a msgs::MeshGeom from a mesh SDF element
    /// \param[in] _sdf The sdf element
    /// \return The new msgs::MeshGeom object
    msgs::MeshGeom MeshFromSDF(sdf::ElementPtr _sdf) GAZEBO_DEPRECATED(2.0);

    /// \brief Create a msgs::MeshGeom from a mesh RML element
    /// \param[in] _sdf The rml element
    /// \return The new msgs::MeshGeom object
    msgs::MeshGeom MeshFromRML(const rml::Mesh &_rml);

    /// \brief Create a msgs::Geometry from a geometry SDF element
    /// \param[in] _sdf The sdf element
    /// \return The new msgs::Geometry object
    msgs::Geometry GeometryFromSDF(sdf::ElementPtr _sdf);

    /// \brief Create a msgs::Geometry from a geometry RML element
    /// \param[in] _sdf The sdf element
    /// \return The new msgs::Geometry object
    msgs::Geometry GeometryFromRML(const rml::Geometry &_rml);

    /// \brief Create a msgs::Visual from a visual SDF element
    /// \param[in] _sdf The sdf element
    /// \return The new msgs::Visual object
    msgs::Visual VisualFromSDF(sdf::ElementPtr _sdf) GAZEBO_DEPRECATED(2.0);

    /// \brief Create a msgs::Visual from a visual RML element
    /// \param[in] _rml The RML element
    /// \return The new msgs::Visual object
    msgs::Visual VisualFromRML(const rml::Visual &_rml);

    /// \brief Create a msgs::Fog from a fog SDF element
    /// \param[in] _sdf The sdf element
    /// \return The new msgs::Fog object
    msgs::Fog FogFromSDF(sdf::ElementPtr _sdf);

    /// \brief Create a msgs::Fog from a fog RML element
    /// \param[in] _sdf The RML element
    /// \return The new msgs::Fog object
    msgs::Fog FogFromSDF(const rml::Scene::Fog &_rml);

    /// \brief Create a msgs::Scene from a scene SDF element
    /// \param[in] _sdf The sdf element
    /// \return The new msgs::Scene object
    msgs::Scene SceneFromSDF(sdf::ElementPtr _sdf) GAZEBO_DEPRECATED(2.0);

    /// \brief Create a msgs::Scene from a scene RML element
    /// \param[in] _rml The rml element
    /// \return The new msgs::Scene object
    msgs::Scene SceneFromRML(const rml::Scene &_rml);

    /// \cond
    const google::protobuf::FieldDescriptor *GetFD(
        google::protobuf::Message &message, const std::string &name);
    /// \endcond

    /// \brief Get the header from a protobuf message
    /// \param[in] _message A google protobuf message
    /// \return A pointer to the message's header
    msgs::Header *GetHeader(google::protobuf::Message &_message);

    /// \}
  }
}

#endif
