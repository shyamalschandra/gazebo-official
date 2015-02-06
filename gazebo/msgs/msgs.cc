/*
 * Copyright (C) 2012-2015 Open Source Robotics Foundation
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

#include <google/protobuf/descriptor.h>
#include <algorithm>

#include "gazebo/math/Vector3.hh"
#include "gazebo/math/Pose.hh"
#include "gazebo/math/Quaternion.hh"
#include "gazebo/math/Plane.hh"
#include "gazebo/math/Rand.hh"

#include "gazebo/common/CommonIface.hh"
#include "gazebo/common/Image.hh"
#include "gazebo/common/Exception.hh"
#include "gazebo/common/Console.hh"
#include "gazebo/msgs/msgs.hh"

namespace gazebo
{
  namespace msgs
  {
    /// Create a request message
    msgs::Request *CreateRequest(const std::string &_request,
        const std::string &_data)
    {
      msgs::Request *request = new msgs::Request;

      request->set_request(_request);
      request->set_data(_data);
      request->set_id(math::Rand::GetIntUniform(1, 10000));

      return request;
    }

    const google::protobuf::FieldDescriptor *GetFD(
        google::protobuf::Message &message, const std::string &name)
    {
      return message.GetDescriptor()->FindFieldByName(name);
    }

    msgs::Header *GetHeader(google::protobuf::Message &message)
    {
      google::protobuf::Message *msg = NULL;

      if (GetFD(message, "str_id"))
        msg = &message;
      else
      {
        const google::protobuf::FieldDescriptor *fd;
        fd = GetFD(message, "header");

        if (fd)
          msg = message.GetReflection()->MutableMessage(&message, fd);
      }

      return (msgs::Header*)msg;
    }

    void Init(google::protobuf::Message &_message, const std::string &_id)
    {
      msgs::Header *header = GetHeader(_message);

      if (header)
      {
        if (!_id.empty())
          header->set_str_id(_id);
        Stamp(header->mutable_stamp());
      }
    }

    void Stamp(msgs::Header *_hdr)
    {
      Stamp(_hdr->mutable_stamp());
    }

    void Stamp(msgs::Time *_time)
    {
      common::Time tm = common::Time::GetWallTime();

      _time->set_sec(tm.sec);
      _time->set_nsec(tm.nsec);
    }

    std::string Package(const std::string &type,
        const google::protobuf::Message &message)
    {
      std::string data;
      msgs::Packet pkg;

      Stamp(pkg.mutable_stamp());
      pkg.set_type(type);

      std::string *serialized_data = pkg.mutable_serialized_data();
      if (!message.IsInitialized())
        gzthrow("Can't serialize message of type[" + message.GetTypeName() +
            "] because it is missing required fields");

      if (!message.SerializeToString(serialized_data))
        gzthrow("Failed to serialized message");

      if (!pkg.SerializeToString(&data))
        gzthrow("Failed to serialized message");

      return data;
    }

    void Set(msgs::Vector3d *_pt, const math::Vector3 &_v)
    {
      _pt->set_x(_v.x);
      _pt->set_y(_v.y);
      _pt->set_z(_v.z);
    }

    void Set(msgs::Vector2d *_pt, const math::Vector2d &_v)
    {
      _pt->set_x(_v.x);
      _pt->set_y(_v.y);
    }

    void Set(msgs::Quaternion *_q, const math::Quaternion &_v)
    {
      _q->set_x(_v.x);
      _q->set_y(_v.y);
      _q->set_z(_v.z);
      _q->set_w(_v.w);
    }

    void Set(msgs::Pose *_p, const math::Pose &_v)
    {
      Set(_p->mutable_position(), _v.pos);
      Set(_p->mutable_orientation(), _v.rot);
    }

    void Set(msgs::Color *_c, const common::Color &_v)
    {
      _c->set_r(_v.r);
      _c->set_g(_v.g);
      _c->set_b(_v.b);
      _c->set_a(_v.a);
    }

    void Set(msgs::Time *_t, const common::Time &_v)
    {
      _t->set_sec(_v.sec);
      _t->set_nsec(_v.nsec);
    }

    /////////////////////////////////////////////////
    void Set(msgs::SphericalCoordinates *_s,
             const common::SphericalCoordinates &_v)
    {
      switch (_v.GetSurfaceType())
      {
        case common::SphericalCoordinates::EARTH_WGS84:
          _s->set_surface_model(msgs::SphericalCoordinates::EARTH_WGS84);
          break;
        default:
          gzerr << "Unable to map surface type[" <<  _v.GetSurfaceType()
            << "] to a SphericalCoordinates message.\n";
          _s->set_surface_model(msgs::SphericalCoordinates::EARTH_WGS84);
          break;
      };

      _s->set_latitude_deg(_v.GetLatitudeReference().Degree());
      _s->set_longitude_deg(_v.GetLongitudeReference().Degree());
      _s->set_heading_deg(_v.GetHeadingOffset().Degree());
      _s->set_elevation(_v.GetElevationReference());
    }

    /////////////////////////////////////////////////
    void Set(msgs::PlaneGeom *_p, const math::Plane &_v)
    {
      Set(_p->mutable_normal(), _v.normal);
      _p->mutable_size()->set_x(_v.size.x);
      _p->mutable_size()->set_y(_v.size.y);
      _p->set_d(_v.d);
    }

    /////////////////////////////////////////////////
    void Set(common::Image &_img, const msgs::Image &_msg)
    {
      _img.SetFromData(
          (const unsigned char*)_msg.data().data(),
          _msg.width(),
          _msg.height(),
          (common::Image::PixelFormat)(_msg.pixel_format()));
    }

    /////////////////////////////////////////////////
    void Set(msgs::Image *_msg, const common::Image &_i)
    {
      _msg->set_width(_i.GetWidth());
      _msg->set_height(_i.GetHeight());
      _msg->set_pixel_format(_i.GetPixelFormat());
      _msg->set_step(_i.GetPitch());

      unsigned char *data = NULL;
      unsigned int size;
      _i.GetData(&data, size);
      _msg->set_data(data, size);
      if (data)
      {
        delete[] data;
      }
    }

    /////////////////////////////////////////////////
    msgs::Vector2d Convert(const math::Vector2d &_v)
    {
      msgs::Vector2d result;
      result.set_x(_v.x);
      result.set_y(_v.y);
      return result;
    }

    /////////////////////////////////////////////////
    msgs::Vector3d Convert(const math::Vector3 &_v)
    {
      msgs::Vector3d result;
      result.set_x(_v.x);
      result.set_y(_v.y);
      result.set_z(_v.z);
      return result;
    }

    msgs::Quaternion Convert(const math::Quaternion &_q)
    {
      msgs::Quaternion result;
      result.set_x(_q.x);
      result.set_y(_q.y);
      result.set_z(_q.z);
      result.set_w(_q.w);
      return result;
    }

    msgs::Pose Convert(const math::Pose &_p)
    {
      msgs::Pose result;
      result.mutable_position()->CopyFrom(Convert(_p.pos));
      result.mutable_orientation()->CopyFrom(Convert(_p.rot));
      return result;
    }

    msgs::Color Convert(const common::Color &_c)
    {
      msgs::Color result;
      result.set_r(_c.r);
      result.set_g(_c.g);
      result.set_b(_c.b);
      result.set_a(_c.a);
      return result;
    }

    msgs::Time Convert(const common::Time &_t)
    {
      msgs::Time result;
      result.set_sec(_t.sec);
      result.set_nsec(_t.nsec);
      return result;
    }

    msgs::PlaneGeom Convert(const math::Plane &_p)
    {
      msgs::PlaneGeom result;
      result.mutable_normal()->CopyFrom(Convert(_p.normal));
      result.mutable_size()->set_x(_p.size.x);
      result.mutable_size()->set_y(_p.size.y);
      result.set_d(_p.d);
      return result;
    }

    msgs::Joint::Type ConvertJointType(const std::string &_str)
    {
      msgs::Joint::Type result = msgs::Joint::REVOLUTE;
      if (_str == "revolute")
      {
        result = msgs::Joint::REVOLUTE;
      }
      else if (_str == "revolute2")
      {
        result = msgs::Joint::REVOLUTE2;
      }
      else if (_str == "prismatic")
      {
        result = msgs::Joint::PRISMATIC;
      }
      else if (_str == "universal")
      {
        result = msgs::Joint::UNIVERSAL;
      }
      else if (_str == "ball")
      {
        result = msgs::Joint::BALL;
      }
      else if (_str == "screw")
      {
        result = msgs::Joint::SCREW;
      }
      else if (_str == "gearbox")
      {
        result = msgs::Joint::GEARBOX;
      }
      return result;
    }

    std::string ConvertJointType(const msgs::Joint::Type _type)
    {
      std::string result;
      switch (_type)
      {
        case msgs::Joint::REVOLUTE:
        {
          result = "revolute";
          break;
        }
        case msgs::Joint::REVOLUTE2:
        {
          result = "revolute2";
          break;
        }
        case msgs::Joint::PRISMATIC:
        {
          result = "prismatic";
          break;
        }
        case msgs::Joint::UNIVERSAL:
        {
          result = "universal";
          break;
        }
        case msgs::Joint::BALL:
        {
          result = "ball";
          break;
        }
        case msgs::Joint::SCREW:
        {
          result = "screw";
          break;
        }
        case msgs::Joint::GEARBOX:
        {
          result = "gearbox";
          break;
        }
        default:
        {
          result = "unknown";
          break;
        }
      }
      return result;
    }

    /////////////////////////////////////////////////
    msgs::Geometry::Type ConvertGeometryType(const std::string &_str)
    {
      msgs::Geometry::Type result = msgs::Geometry::BOX;
      if (_str == "box")
      {
        result = msgs::Geometry::BOX;
      }
      else if (_str == "cylinder")
      {
        result = msgs::Geometry::CYLINDER;
      }
      else if (_str == "sphere")
      {
        result = msgs::Geometry::SPHERE;
      }
      else if (_str == "plane")
      {
        result = msgs::Geometry::PLANE;
      }
      else if (_str == "image")
      {
        result = msgs::Geometry::IMAGE;
      }
      else if (_str == "heightmap")
      {
        result = msgs::Geometry::HEIGHTMAP;
      }
      else if (_str == "mesh")
      {
        result = msgs::Geometry::MESH;
      }
      else if (_str == "polyline")
      {
        result = msgs::Geometry::POLYLINE;
      }
      else
      {
        gzwarn << "Geometry: '" << _str << "' is not recognized, "
            << " returning type as msgs::Geometry::BOX." << std::endl;
      }

      return result;
    }

    /////////////////////////////////////////////////
    std::string ConvertGeometryType(const msgs::Geometry::Type _type)
    {
      std::string result;
      switch (_type)
      {
        case msgs::Geometry::BOX:
        {
          result = "box";
          break;
        }
        case msgs::Geometry::CYLINDER:
        {
          result = "cylinder";
          break;
        }
        case msgs::Geometry::SPHERE:
        {
          result = "sphere";
          break;
        }
        case msgs::Geometry::PLANE:
        {
          result = "plane";
          break;
        }
        case msgs::Geometry::IMAGE:
        {
          result = "image";
          break;
        }
        case msgs::Geometry::HEIGHTMAP:
        {
          result = "heightmap";
          break;
        }
        case msgs::Geometry::MESH:
        {
          result = "mesh";
          break;
        }
        case msgs::Geometry::POLYLINE:
        {
          result = "polyline";
          break;
        }
        default:
        {
          result = "unknown";
          break;
        }
      }
      return result;
    }

    math::Vector3 Convert(const msgs::Vector3d &_v)
    {
      return math::Vector3(_v.x(), _v.y(), _v.z());
    }

    math::Vector2d Convert(const msgs::Vector2d &_v)
    {
      return math::Vector2d(_v.x(), _v.y());
    }

    math::Quaternion Convert(const msgs::Quaternion &_q)
    {
      return math::Quaternion(_q.w(), _q.x(), _q.y(), _q.z());
    }

    math::Pose Convert(const msgs::Pose &_p)
    {
      return math::Pose(Convert(_p.position()),
          Convert(_p.orientation()));
    }

    common::Color Convert(const msgs::Color &_c)
    {
      return common::Color(_c.r(), _c.g(), _c.b(), _c.a());
    }

    common::Time Convert(const msgs::Time &_t)
    {
      return common::Time(_t.sec(), _t.nsec());
    }

    math::Plane Convert(const msgs::PlaneGeom &_p)
    {
      return math::Plane(Convert(_p.normal()),
          math::Vector2d(_p.size().x(), _p.size().y()),
          _p.d());
    }

    msgs::GUI GUIFromSDF(sdf::ElementPtr _sdf)
    {
      msgs::GUI result;

      result.set_fullscreen(_sdf->Get<bool>("fullscreen"));


      if (_sdf->HasElement("camera"))
      {
        sdf::ElementPtr camSDF = _sdf->GetElement("camera");
        msgs::GUICamera *guiCam = result.mutable_camera();

        guiCam->set_name(camSDF->Get<std::string>("name"));

        if (camSDF->HasElement("pose"))
        {
          msgs::Set(guiCam->mutable_pose(), camSDF->Get<math::Pose>("pose"));
        }

        if (camSDF->HasElement("view_controller"))
        {
          guiCam->set_view_controller(
              camSDF->Get<std::string>("view_controller"));
        }

        if (camSDF->HasElement("track_visual"))
        {
          guiCam->mutable_track()->CopyFrom(
              TrackVisualFromSDF(camSDF->GetElement("track_visual")));
        }
      }

      return result;
    }

    /////////////////////////////////////////////////
    msgs::TrackVisual TrackVisualFromSDF(sdf::ElementPtr _sdf)
    {
      msgs::TrackVisual result;

      result.set_name(_sdf->Get<std::string>("name"));

      if (_sdf->HasElement("min_dist"))
        result.set_min_dist(_sdf->GetElement("min_dist")->Get<double>());

      if (_sdf->HasElement("max_dist"))
        result.set_max_dist(_sdf->GetElement("max_dist")->Get<double>());

      return result;
    }


    /////////////////////////////////////////////////
    msgs::Light LightFromSDF(sdf::ElementPtr _sdf)
    {
      msgs::Light result;

      std::string type = _sdf->Get<std::string>("type");
      std::transform(type.begin(), type.end(), type.begin(), ::tolower);

      result.set_name(_sdf->Get<std::string>("name"));

      result.set_cast_shadows(_sdf->Get<bool>("cast_shadows"));

      if (type == "point")
        result.set_type(msgs::Light::POINT);
      else if (type == "spot")
        result.set_type(msgs::Light::SPOT);
      else if (type == "directional")
        result.set_type(msgs::Light::DIRECTIONAL);

      if (_sdf->HasElement("pose"))
      {
        result.mutable_pose()->CopyFrom(Convert(_sdf->Get<math::Pose>("pose")));
      }

      if (_sdf->HasElement("diffuse"))
      {
        result.mutable_diffuse()->CopyFrom(
            Convert(_sdf->Get<common::Color>("diffuse")));
      }

      if (_sdf->HasElement("specular"))
      {
        result.mutable_specular()->CopyFrom(
            Convert(_sdf->Get<common::Color>("specular")));
      }

      if (_sdf->HasElement("attenuation"))
      {
        sdf::ElementPtr elem = _sdf->GetElement("attenuation");
        result.set_attenuation_constant(elem->Get<double>("constant"));
        result.set_attenuation_linear(elem->Get<double>("linear"));
        result.set_attenuation_quadratic(elem->Get<double>("quadratic"));
        result.set_range(elem->Get<double>("range"));
      }

      if (_sdf->HasElement("direction"))
      {
        result.mutable_direction()->CopyFrom(
            Convert(_sdf->Get<math::Vector3>("direction")));
      }

      if (_sdf->HasElement("spot"))
      {
        sdf::ElementPtr elem = _sdf->GetElement("spot");
        result.set_spot_inner_angle(elem->Get<double>("inner_angle"));
        result.set_spot_outer_angle(elem->Get<double>("outer_angle"));
        result.set_spot_falloff(elem->Get<double>("falloff"));
      }

      return result;
    }

    /////////////////////////////////////////////////
    msgs::MeshGeom MeshFromSDF(sdf::ElementPtr _sdf)
    {
      msgs::MeshGeom result;

      if (_sdf->GetName() != "mesh")
      {
        gzerr << "Cannot create a mesh message from an "
          << _sdf->GetName() << " SDF element.\n";
        return result;
      }

        msgs::Set(result.mutable_scale(), _sdf->Get<math::Vector3>("scale"));

        result.set_filename(_sdf->Get<std::string>("uri"));

        if (_sdf->HasElement("submesh"))
        {
          sdf::ElementPtr submeshElem = _sdf->GetElement("submesh");
          if (submeshElem->HasElement("name") &&
              submeshElem->Get<std::string>("name") != "__default__")
          {
            result.set_submesh(submeshElem->Get<std::string>("name"));

            if (submeshElem->HasElement("center"))
              result.set_center_submesh(submeshElem->Get<bool>("center"));
          }
        }

      return result;
    }

    /////////////////////////////////////////////////
    msgs::Geometry GeometryFromSDF(sdf::ElementPtr _sdf)
    {
      msgs::Geometry result;

      if (_sdf->GetName() != "geometry")
      {
        gzerr << "Cannot create a geometry message from an "
          << _sdf->GetName() << " SDF element.\n";
        return result;
      }

      // Load the geometry
      sdf::ElementPtr geomElem = _sdf->GetFirstElement();
      if (!geomElem)
        gzthrow("Invalid geometry element");

      if (geomElem->GetName() == "box")
      {
        result.set_type(msgs::Geometry::BOX);
        msgs::Set(result.mutable_box()->mutable_size(),
            geomElem->Get<math::Vector3>("size"));
      }
      else if (geomElem->GetName() == "cylinder")
      {
        result.set_type(msgs::Geometry::CYLINDER);
        result.mutable_cylinder()->set_radius(
            geomElem->Get<double>("radius"));
        result.mutable_cylinder()->set_length(
            geomElem->Get<double>("length"));
      }
      else if (geomElem->GetName() == "sphere")
      {
        result.set_type(msgs::Geometry::SPHERE);
        result.mutable_sphere()->set_radius(
            geomElem->Get<double>("radius"));
      }
      else if (geomElem->GetName() == "plane")
      {
        result.set_type(msgs::Geometry::PLANE);
        msgs::Set(result.mutable_plane()->mutable_normal(),
            geomElem->Get<math::Vector3>("normal"));
        msgs::Set(result.mutable_plane()->mutable_size(),
            geomElem->Get<math::Vector2d>("size"));
      }
      else if (geomElem->GetName() == "polyline")
      {
        result.set_type(msgs::Geometry::POLYLINE);
        result.mutable_polyline()->set_height(geomElem->Get<double>("height"));
        sdf::ElementPtr pointElem = geomElem->GetElement("point");
        while (pointElem)
        {
           math::Vector2d point = pointElem->Get<math::Vector2d>();
           pointElem = pointElem->GetNextElement("point");
           msgs::Vector2d *ptMsg = result.mutable_polyline()->add_point();
           msgs::Set(ptMsg, point);
        }
      }
      else if (geomElem->GetName() == "image")
      {
        result.set_type(msgs::Geometry::IMAGE);
        result.mutable_image()->set_scale(
            geomElem->Get<double>("scale"));
        result.mutable_image()->set_height(
            geomElem->Get<double>("height"));
        result.mutable_image()->set_uri(
            geomElem->Get<std::string>("uri"));
      }
      else if (geomElem->GetName() == "heightmap")
      {
        result.set_type(msgs::Geometry::HEIGHTMAP);
        msgs::Set(result.mutable_heightmap()->mutable_size(),
            geomElem->Get<math::Vector3>("size"));
        msgs::Set(result.mutable_heightmap()->mutable_origin(),
            geomElem->Get<math::Vector3>("pos"));

        sdf::ElementPtr textureElem = geomElem->GetElement("texture");
        while (textureElem)
        {
          msgs::HeightmapGeom::Texture *tex =
            result.mutable_heightmap()->add_texture();
          tex->set_diffuse(textureElem->Get<std::string>("diffuse"));
          tex->set_normal(textureElem->Get<std::string>("normal"));
          tex->set_size(textureElem->Get<double>("size"));
          textureElem = textureElem->GetNextElement("texture");
        }

        sdf::ElementPtr blendElem = geomElem->GetElement("blend");
        while (blendElem)
        {
          msgs::HeightmapGeom::Blend *blend =
            result.mutable_heightmap()->add_blend();

          blend->set_min_height(blendElem->Get<double>("min_height"));
          blend->set_fade_dist(blendElem->Get<double>("fade_dist"));
          blendElem = blendElem->GetNextElement("blend");
        }

        // Set if the rendering engine uses terrain paging
        bool useTerrainPaging =
            geomElem->Get<bool>("use_terrain_paging");
        result.mutable_heightmap()->set_use_terrain_paging(useTerrainPaging);
      }
      else if (geomElem->GetName() == "mesh")
      {
        result.set_type(msgs::Geometry::MESH);
        result.mutable_mesh()->CopyFrom(MeshFromSDF(geomElem));
      }
      else if (geomElem->GetName() == "empty")
      {
        result.set_type(msgs::Geometry::EMPTY);
      }
      else
        gzthrow("Unknown geometry type\n");

      return result;
    }

    /////////////////////////////////////////////////
    msgs::Visual VisualFromSDF(sdf::ElementPtr _sdf)
    {
      msgs::Visual result;

      result.set_name(_sdf->Get<std::string>("name"));

      if (_sdf->HasElement("cast_shadows"))
        result.set_cast_shadows(_sdf->Get<bool>("cast_shadows"));

      if (_sdf->HasElement("transparency"))
        result.set_transparency(_sdf->Get<double>("transparency"));

      if (_sdf->HasElement("laser_retro"))
        result.set_laser_retro(_sdf->Get<double>("laser_retro"));

      // Load the geometry
      if (_sdf->HasElement("geometry"))
      {
        msgs::Geometry *geomMsg = result.mutable_geometry();
        geomMsg->CopyFrom(GeometryFromSDF(_sdf->GetElement("geometry")));
      }

      /// Load the material
      if (_sdf->HasElement("material"))
      {
        sdf::ElementPtr elem = _sdf->GetElement("material");
        msgs::Material *matMsg = result.mutable_material();

        if (elem->HasElement("script"))
        {
          sdf::ElementPtr scriptElem = elem->GetElement("script");
          matMsg->mutable_script()->set_name(
              scriptElem->Get<std::string>("name"));

          sdf::ElementPtr uriElem = scriptElem->GetElement("uri");
          while (uriElem)
          {
            matMsg->mutable_script()->add_uri(uriElem->Get<std::string>());
            uriElem = uriElem->GetNextElement("uri");
          }
        }

        if (elem->HasElement("lighting"))
        {
          matMsg->set_lighting(elem->Get<bool>("lighting"));
        }

        if (elem->HasElement("shader"))
        {
          sdf::ElementPtr shaderElem = elem->GetElement("shader");

          if (shaderElem->Get<std::string>("type") == "pixel")
            matMsg->set_shader_type(msgs::Material::PIXEL);
          else if (shaderElem->Get<std::string>("type") == "vertex")
            matMsg->set_shader_type(msgs::Material::VERTEX);
          else if (shaderElem->Get<std::string>("type") ==
              "normal_map_object_space")
            matMsg->set_shader_type(msgs::Material::NORMAL_MAP_OBJECT_SPACE);
          else if (shaderElem->Get<std::string>("type") ==
              "normal_map_tangent_space")
            matMsg->set_shader_type(msgs::Material::NORMAL_MAP_TANGENT_SPACE);
          else
            gzthrow(std::string("Unknown shader type[") +
                shaderElem->Get<std::string>("type") + "]");

          if (shaderElem->HasElement("normal_map"))
            matMsg->set_normal_map(
                shaderElem->GetElement("normal_map")->Get<std::string>());
        }

        if (elem->HasElement("ambient"))
          msgs::Set(matMsg->mutable_ambient(),
              elem->Get<common::Color>("ambient"));
        if (elem->HasElement("diffuse"))
          msgs::Set(matMsg->mutable_diffuse(),
              elem->Get<common::Color>("diffuse"));
        if (elem->HasElement("specular"))
          msgs::Set(matMsg->mutable_specular(),
              elem->Get<common::Color>("specular"));
        if (elem->HasElement("emissive"))
          msgs::Set(matMsg->mutable_emissive(),
              elem->Get<common::Color>("emissive"));
      }

      // Set the origin of the visual
      if (_sdf->HasElement("pose"))
      {
        msgs::Set(result.mutable_pose(), _sdf->Get<math::Pose>("pose"));
      }

      // Set plugins of the visual
      if (_sdf->HasElement("plugin"))
      {
        sdf::ElementPtr elem = _sdf->GetElement("plugin");
        msgs::Plugin *plgnMsg = result.mutable_plugin();
        // if (elem->HasElement("name"))
          plgnMsg->set_name(elem->Get<std::string>("name"));
        // if (elem->HasElement("filename"))
          plgnMsg->set_filename(elem->Get<std::string>("filename"));

        std::stringstream ss;
        for (sdf::ElementPtr innerElem = elem->GetFirstElement();
            innerElem;
            innerElem = innerElem->GetNextElement(""))
        {
          ss << innerElem->ToString("");
        }
        plgnMsg->set_innerxml("<sdf>" + ss.str() + "</sdf>");
      }

      return result;
    }

    /////////////////////////////////////////////////
    msgs::Fog FogFromSDF(sdf::ElementPtr _sdf)
    {
      msgs::Fog result;

      std::string type = _sdf->Get<std::string>("type");
      if (type == "linear")
        result.set_type(msgs::Fog::LINEAR);
      else if (type == "exp")
        result.set_type(msgs::Fog::EXPONENTIAL);
      else if (type == "exp2")
        result.set_type(msgs::Fog::EXPONENTIAL2);
      else if (type == "none")
        result.set_type(msgs::Fog::NONE);
      else
        gzthrow(std::string("Unknown fog type[") + type + "]");

      result.mutable_color()->CopyFrom(
          Convert(_sdf->Get<common::Color>("color")));

      result.set_density(_sdf->Get<double>("density"));
      result.set_start(_sdf->Get<double>("start"));
      result.set_end(_sdf->Get<double>("end"));
      return result;
    }

    msgs::Scene SceneFromSDF(sdf::ElementPtr _sdf)
    {
      msgs::Scene result;

      Init(result, "scene");

      if (_sdf->HasElement("grid"))
        result.set_grid(_sdf->Get<bool>("grid"));
      else
        result.set_grid(true);

      if (_sdf->HasElement("ambient"))
        result.mutable_ambient()->CopyFrom(
            Convert(_sdf->Get<common::Color>("ambient")));

      if (_sdf->HasElement("background"))
      {
        result.mutable_background()->CopyFrom(
            Convert(_sdf->Get<common::Color>("background")));
      }

      if (_sdf->HasElement("sky"))
      {
        msgs::Sky *skyMsg = result.mutable_sky();
        skyMsg->set_time(_sdf->GetElement("sky")->Get<double>("time"));
        skyMsg->set_sunrise(_sdf->GetElement("sky")->Get<double>("sunrise"));
        skyMsg->set_sunset(_sdf->GetElement("sky")->Get<double>("sunset"));

        if (_sdf->GetElement("sky")->HasElement("clouds"))
        {
          sdf::ElementPtr cloudsElem =
            _sdf->GetElement("sky")->GetElement("clouds");
          skyMsg->set_wind_speed(cloudsElem->Get<double>("speed"));
          skyMsg->set_wind_direction(cloudsElem->Get<double>("direction"));
          skyMsg->set_humidity(cloudsElem->Get<double>("humidity"));
          skyMsg->set_mean_cloud_size(cloudsElem->Get<double>("mean_size"));
          msgs::Set(skyMsg->mutable_cloud_ambient(),
                    cloudsElem->Get<common::Color>("ambient"));
        }
      }

      if (_sdf->HasElement("fog"))
        result.mutable_fog()->CopyFrom(FogFromSDF(_sdf->GetElement("fog")));

      if (_sdf->HasElement("shadows"))
        result.set_shadows(_sdf->Get<bool>("shadows"));

      return result;
    }

    /////////////////////////////////////////////////
    sdf::ElementPtr LightToSDF(const msgs::Light &_msg, sdf::ElementPtr _sdf)
    {
      sdf::ElementPtr lightSDF;

      if (_sdf)
      {
        lightSDF = _sdf;
      }
      else
      {
        lightSDF.reset(new sdf::Element);
        sdf::initFile("light.sdf", lightSDF);
      }

      lightSDF->GetAttribute("name")->Set(_msg.name());

      if (_msg.has_type() && _msg.type() == msgs::Light::POINT)
        lightSDF->GetAttribute("type")->Set("point");
      else if (_msg.has_type() && _msg.type() == msgs::Light::SPOT)
        lightSDF->GetAttribute("type")->Set("spot");
      else if (_msg.has_type() && _msg.type() == msgs::Light::DIRECTIONAL)
        lightSDF->GetAttribute("type")->Set("directional");

      if (_msg.has_pose())
      {
        lightSDF->GetElement("pose")->Set(msgs::Convert(_msg.pose()));
      }

      if (_msg.has_diffuse())
      {
        lightSDF->GetElement("diffuse")->Set(msgs::Convert(_msg.diffuse()));
      }

      if (_msg.has_specular())
      {
        lightSDF->GetElement("specular")->Set(msgs::Convert(_msg.specular()));
      }

      if (_msg.has_direction())
      {
        lightSDF->GetElement("direction")->Set(msgs::Convert(_msg.direction()));
      }

      if (_msg.has_attenuation_constant())
      {
        sdf::ElementPtr elem = lightSDF->GetElement("attenuation");
        elem->GetElement("constant")->Set(_msg.attenuation_constant());
      }

      if (_msg.has_attenuation_linear())
      {
        sdf::ElementPtr elem = lightSDF->GetElement("attenuation");
        elem->GetElement("linear")->Set(_msg.attenuation_linear());
      }

      if (_msg.has_attenuation_quadratic())
      {
        sdf::ElementPtr elem = lightSDF->GetElement("attenuation");
        elem->GetElement("quadratic")->Set(_msg.attenuation_quadratic());
      }

      if (_msg.has_range())
      {
        sdf::ElementPtr elem = lightSDF->GetElement("attenuation");
        elem->GetElement("range")->Set(_msg.range());
      }

      if (_msg.has_cast_shadows())
        lightSDF->GetElement("cast_shadows")->Set(_msg.cast_shadows());

      if (_msg.has_spot_inner_angle())
      {
        sdf::ElementPtr elem = lightSDF->GetElement("spot");
        elem->GetElement("inner_angle")->Set(_msg.spot_inner_angle());
      }

      if (_msg.has_spot_outer_angle())
      {
        sdf::ElementPtr elem = lightSDF->GetElement("spot");
        elem->GetElement("outer_angle")->Set(_msg.spot_outer_angle());
      }

      if (_msg.has_spot_falloff())
      {
        sdf::ElementPtr elem = lightSDF->GetElement("spot");
        elem->GetElement("falloff")->Set(_msg.spot_falloff());
      }
      return lightSDF;
    }

    ////////////////////////////////////////////////////////
    void AddBoxLink(msgs::Model &_msg, double _mass,
                    const math::Vector3 &_size)
    {
      _msg.add_link();
      int linkCount = _msg.link_size();
      msgs::Link *link = _msg.mutable_link(linkCount-1);
      {
        std::ostringstream linkName;
        linkName << "link" << linkCount;
        link->set_name(linkName.str());
      }

      msgs::Inertial *inertial = link->mutable_inertial();
      inertial->set_mass(_mass);
      {
        double dx = _size.x;
        double dy = _size.y;
        double dz = _size.z;
        double ixx = _mass/12.0 * (dy*dy + dz*dz);
        double iyy = _mass/12.0 * (dz*dz + dx*dx);
        double izz = _mass/12.0 * (dx*dx + dy*dy);
        inertial->set_ixx(ixx);
        inertial->set_iyy(iyy);
        inertial->set_izz(izz);
        inertial->set_ixy(0.0);
        inertial->set_ixz(0.0);
        inertial->set_iyz(0.0);
      }

      link->add_collision();
      msgs::Collision *collision = link->mutable_collision(0);
      collision->set_name("collision");

      msgs::Geometry *geometry = collision->mutable_geometry();
      geometry->set_type(Geometry_Type_BOX);
      msgs::Set(geometry->mutable_box()->mutable_size(), _size);
    }

    ////////////////////////////////////////////////////////
    std::string ToSDF(const msgs::Model &_msg)
    {
      std::ostringstream stream;
      stream << "<model name='"
             << _msg.name()
             << "'>";

      // ignore the id field, since it's not used in sdformat
      if (_msg.has_is_static())
      {
        stream << "<static>" << _msg.is_static() << "</static>";
      }
      if (_msg.has_pose())
      {
        stream << "<pose>"
               << msgs::Convert(_msg.pose())
               << "</pose>";
      }
      for (int i = 0; i < _msg.joint_size(); ++i)
      {
        stream << msgs::ToSDF(_msg.joint(i));
      }
      for (int i = 0; i < _msg.link_size(); ++i)
      {
        stream << msgs::ToSDF(_msg.link(i));
      }
      // ignore the deleted field, since it's not used in sdformat
      if (_msg.visual_size() > 0)
      {
        gzerr << "Model visuals not yet parsed" << std::endl;
      }
      // ignore the scale field, since it's not used in sdformat

      stream << "</model>";
      return stream.str();
    }

    ////////////////////////////////////////////////////////
    std::string ToSDF(const msgs::Link &_msg)
    {
      std::ostringstream stream;
      stream << "<link name='"
             << _msg.name()
             << "'>";

      // ignore the id field, since it's not used in sdformat
      if (_msg.has_self_collide())
      {
        stream << "<self_collide>" << _msg.self_collide() << "</self_collide>";
      }
      if (_msg.has_gravity())
      {
        stream << "<gravity>" << _msg.gravity() << "</gravity>";
      }
      if (_msg.has_kinematic())
      {
        stream << "<kinematic>" << _msg.kinematic() << "</kinematic>";
      }
      // ignore the enabled field, since it's not used in sdformat
      if (_msg.has_inertial())
      {
        stream << msgs::ToSDF(_msg.inertial());
      }
      if (_msg.has_pose())
      {
        stream << "<pose>"
               << msgs::Convert(_msg.pose())
               << "</pose>";
      }
      if (_msg.visual_size() > 0)
      {
        gzerr << "Link visuals not yet parsed" << std::endl;
      }
      for (int i = 0; i < _msg.collision_size(); ++i)
      {
        stream << msgs::ToSDF(_msg.collision(i));
      }
      if (_msg.sensor_size() > 0)
      {
        gzerr << "Link sensors not yet parsed" << std::endl;
      }
      if (_msg.projector_size() > 0)
      {
        gzerr << "Link projectors not yet parsed" << std::endl;
      }
      // ignore the canonical field, since it's not used in sdformat

      stream << "</link>";
      return stream.str();
    }

    ////////////////////////////////////////////////////////
    std::string ToSDF(const msgs::Collision &_msg)
    {
      std::ostringstream stream;
      stream << "<collision name='"
             << _msg.name()
             << "'>";

      // ignore the id field, since it's not used in sdformat
      if (_msg.has_laser_retro())
      {
        stream << "<laser_retro>" << _msg.laser_retro() << "</laser_retro>";
      }
      if (_msg.has_max_contacts())
      {
        stream << "<max_contacts>" << _msg.max_contacts() << "</max_contacts>";
      }
      if (_msg.has_pose())
      {
        stream << "<pose>"
               << msgs::Convert(_msg.pose())
               << "</pose>";
      }
      if (_msg.has_geometry())
      {
        stream << msgs::ToSDF(_msg.geometry());
      }
      if (_msg.has_surface())
      {
        stream << msgs::ToSDF(_msg.surface());
      }
      // also ignore the visual field, since it's not used in sdformat

      stream << "</collision>";
      return stream.str();
    }

    ////////////////////////////////////////////////////////
    std::string ToSDF(const msgs::Geometry &_msg)
    {
      std::ostringstream stream;
      stream << "<geometry>";

      if (!_msg.has_type())
      {
        gzerr << "msgs::Geometry missing type" << std::endl;
        stream << "</geometry>";
        return stream.str();
      }

      if (_msg.type() == Geometry_Type_BOX &&
          _msg.has_box())
      {
        stream << msgs::ToSDF(_msg.box());
      }
      else if (_msg.type() == Geometry_Type_CYLINDER &&
          _msg.has_cylinder())
      {
        stream << msgs::ToSDF(_msg.cylinder());
      }
      else if (_msg.type() == Geometry_Type_HEIGHTMAP &&
          _msg.has_heightmap())
      {
        gzerr << "ToSDF(msgs::HeightmapGeom not implemented" << std::endl;
        // stream << msgs::ToSDF(_msg.heightmap());
      }
      else if (_msg.type() == Geometry_Type_IMAGE &&
          _msg.has_image())
      {
        stream << msgs::ToSDF(_msg.image());
      }
      else if (_msg.type() == Geometry_Type_MESH &&
          _msg.has_mesh())
      {
        gzerr << "ToSDF(msgs::MeshGeom not implemented" << std::endl;
        // stream << msgs::ToSDF(_msg.mesh());
      }
      else if (_msg.type() == Geometry_Type_PLANE &&
          _msg.has_plane())
      {
        stream << msgs::ToSDF(_msg.plane());
      }
      else if (_msg.type() == Geometry_Type_SPHERE &&
          _msg.has_sphere())
      {
        stream << msgs::ToSDF(_msg.sphere());
      }
      else
      {
        gzerr << "Unrecognized geometry type" << std::endl;
      }

      stream << "</geometry>";
      return stream.str();
    }

    ////////////////////////////////////////////////////////
    std::string ToSDF(const msgs::BoxGeom &_msg)
    {
      std::ostringstream stream;
      stream << "<box>"
             << "<size>"
             << msgs::Convert(_msg.size())
             << "</size>"
             << "</box>";
      return stream.str();
    }

    ////////////////////////////////////////////////////////
    std::string ToSDF(const msgs::CylinderGeom &_msg)
    {
      std::ostringstream stream;
      stream << "<cylinder>"
             << "<radius>" << _msg.radius() << "</radius>"
             << "<length>" << _msg.length() << "</length>"
             << "</cylinder>";
      return stream.str();
    }

    ////////////////////////////////////////////////////////
    std::string ToSDF(const msgs::ImageGeom &_msg)
    {
      std::ostringstream stream;
      stream << "<image>"
             << "<uri>" << _msg.uri() << "</uri>";
      if (_msg.has_scale())
      {
        stream << "<scale>" << _msg.scale() << "</scale>";
      }
      if (_msg.has_threshold())
      {
        stream << "<threshold>" << _msg.threshold() << "</threshold>";
      }
      if (_msg.has_height())
      {
        stream << "<height>" << _msg.height() << "</height>";
      }
      if (_msg.has_granularity())
      {
        stream << "<granularity>" << _msg.granularity() << "</granularity>";
      }
      stream << "</image>";
      return stream.str();
    }

    ////////////////////////////////////////////////////////
    std::string ToSDF(const msgs::PlaneGeom &_msg)
    {
      std::ostringstream stream;
      stream << "<plane>"
             << "<normal>"
             << msgs::Convert(_msg.normal())
             << "</normal>"
             << "<size>"
             << msgs::Convert(_msg.size())
             << "</size>";
      if (_msg.has_d())
      {
        gzerr << "sdformat doesn't have Plane.d variable" << std::endl;
      }
      stream << "</plane>";
      return stream.str();
    }

    ////////////////////////////////////////////////////////
    std::string ToSDF(const msgs::SphereGeom &_msg)
    {
      std::ostringstream stream;
      stream << "<sphere>"
             << "<radius>" << _msg.radius() << "</radius>"
             << "</sphere>";
      return stream.str();
    }

    ////////////////////////////////////////////////////////
    std::string ToSDF(const msgs::Surface &_msg)
    {
      std::ostringstream stream;
      stream << "<surface>";

      // bounce element block
      stream << "<bounce>";
      if (_msg.has_restitution_coefficient())
      {
        stream << "<restitution_coefficient>"
               << _msg.restitution_coefficient()
               << "</restitution_coefficient>";
      }
      if (_msg.has_bounce_threshold())
      {
        stream << "<threshold>"
               << _msg.bounce_threshold()
               << "</threshold>";
      }
      stream << "</bounce>";

      // friction element block
      if (_msg.has_friction())
      {
        stream << msgs::ToSDF(_msg.friction());
      }

      // contact element block
      stream << "<contact>";
      if (_msg.has_collide_without_contact())
      {
        stream << "<collide_without_contact>"
               << _msg.collide_without_contact()
               << "</collide_without_contact>";
      }
      if (_msg.has_collide_without_contact_bitmask())
      {
        stream << "<collide_without_contact_bitmask>"
               << _msg.collide_without_contact_bitmask()
               << "</collide_without_contact_bitmask>";
      }
      {
        std::ostringstream odeStream, bulletStream;
        odeStream    << "<ode>";
        bulletStream << "<bullet>";
        if (_msg.has_soft_cfm())
        {
          odeStream    << "<soft_cfm>" << _msg.soft_cfm() << "</soft_cfm>";
          bulletStream << "<soft_cfm>" << _msg.soft_cfm() << "</soft_cfm>";
        }
        if (_msg.has_soft_erp())
        {
          odeStream    << "<soft_erp>" << _msg.soft_erp() << "</soft_erp>";
          bulletStream << "<soft_erp>" << _msg.soft_erp() << "</soft_erp>";
        }
        if (_msg.has_kp())
        {
          odeStream    << "<kp>" << _msg.kp() << "</kp>";
          bulletStream << "<kp>" << _msg.kp() << "</kp>";
        }
        if (_msg.has_kd())
        {
          odeStream    << "<kd>" << _msg.kd() << "</kd>";
          bulletStream << "<kd>" << _msg.kd() << "</kd>";
        }
        if (_msg.has_max_vel())
        {
          odeStream << "<max_vel>" << _msg.max_vel() << "</max_vel>";
        }
        if (_msg.has_min_depth())
        {
          odeStream << "<min_depth>" << _msg.min_depth() << "</min_depth>";
        }
        odeStream    << "</ode>";
        bulletStream << "</bullet>";
        stream << odeStream.str() << bulletStream.str();
      }
      stream << "</contact>";

      stream << "</surface>";
      return stream.str();
    }

    ////////////////////////////////////////////////////////
    std::string ToSDF(const msgs::Inertial &_msg)
    {
      std::ostringstream stream;
      stream << "<inertial>";

      if (_msg.has_mass())
      {
        stream << "<mass>" << _msg.mass() << "</mass>";
      }
      if (_msg.has_pose())
      {
        stream << "<pose>" << msgs::Convert(_msg.pose()) << "</pose>";
      }

      stream << "<inertia>";
      if (_msg.has_ixx())
      {
        stream << "<ixx>" << _msg.ixx() << "</ixx>";
      }
      if (_msg.has_ixy())
      {
        stream << "<ixy>" << _msg.ixy() << "</ixy>";
      }
      if (_msg.has_ixz())
      {
        stream << "<ixz>" << _msg.ixz() << "</ixz>";
      }
      if (_msg.has_iyy())
      {
        stream << "<iyy>" << _msg.iyy() << "</iyy>";
      }
      if (_msg.has_iyz())
      {
        stream << "<iyz>" << _msg.iyz() << "</iyz>";
      }
      if (_msg.has_izz())
      {
        stream << "<izz>" << _msg.izz() << "</izz>";
      }
      stream << "</inertia>";

      stream << "</inertial>";
      return stream.str();
    }

    ////////////////////////////////////////////////////////
    std::string ToSDF(const msgs::Joint &_msg,
                      int _useParentModelFrame1,
                      int _useParentModelFrame2)
    {
      std::ostringstream stream;
      stream << "<joint name='"
             << _msg.name();
      if (_msg.has_type())
      {
        if (_msg.type() == Joint::REVOLUTE)
        {
          stream << "' type='revolute";
        }
        else if (_msg.type() == Joint::REVOLUTE2)
        {
          stream << "' type='revolute2";
        }
        else if (_msg.type() == Joint::PRISMATIC)
        {
          stream << "' type='prismatic";
        }
        else if (_msg.type() == Joint::UNIVERSAL)
        {
          stream << "' type='universal";
        }
        else if (_msg.type() == Joint::BALL)
        {
          stream << "' type='ball";
        }
        else if (_msg.type() == Joint::SCREW)
        {
          stream << "' type='screw";
        }
        else if (_msg.type() == Joint::GEARBOX)
        {
          stream << "' type='gearbox";
        }
      }
      stream << "'>";

      // ignore the id field, since it's not used in sdformat
      // ignore the parent_id field, since it's not used in sdformat
      // ignore the child_id field, since it's not used in sdformat
      // ignore the angle field, since it's not used in sdformat
      if (_msg.has_parent())
      {
        stream << "<parent>" << _msg.parent() << "</parent>";
      }
      if (_msg.has_child())
      {
        stream << "<child>" << _msg.child() << "</child>";
      }
      if (_msg.has_pose())
      {
        stream << "<pose>"
               << msgs::Convert(_msg.pose())
               << "</pose>";
      }
      if (_msg.has_axis1())
      {
        stream << ToSDF(_msg.axis1(), "axis", _useParentModelFrame1);
      }
      if (_msg.has_axis2())
      {
        stream << ToSDF(_msg.axis2(), "axis2", _useParentModelFrame2);
      }

      stream << "<physics>"
             << "<ode>";
      if (_msg.has_cfm())
      {
        stream << "<cfm>" << _msg.cfm() << "</cfm>";
      }
      if (_msg.has_bounce())
      {
        stream << "<bounce>" << _msg.bounce() << "</bounce>";
      }
      if (_msg.has_velocity())
      {
        stream << "<velocity>" << _msg.velocity() << "</velocity>";
      }
      if (_msg.has_fudge_factor())
      {
        stream << "<fudge_factor>" << _msg.fudge_factor() << "</fudge_factor>";
      }

      stream << "<limit>";
      if (_msg.has_limit_cfm())
      {
        stream << "<cfm>" << _msg.limit_cfm() << "</cfm>";
      }
      if (_msg.has_limit_erp())
      {
        stream << "<erp>" << _msg.limit_erp() << "</erp>";
      }
      stream << "</limit>"
             << "<suspension>";
      if (_msg.has_suspension_cfm())
      {
        stream << "<cfm>" << _msg.suspension_cfm() << "</cfm>";
      }
      if (_msg.has_suspension_erp())
      {
        stream << "<erp>" << _msg.suspension_erp() << "</erp>";
      }
      stream << "</suspension>"
             << "</ode>"
             << "</physics>";
      // also ignore the sensor field for now

      stream << "</joint>";
      return stream.str();
    }

    ////////////////////////////////////////////////////////
    std::string ToSDF(const msgs::Axis &_msg,
                      const std::string &_name,
                      int _useParentModelFrame)
    {
      std::ostringstream stream;
      stream << "<" << _name << ">";

      if (_msg.has_xyz())
      {
        stream << "<xyz>"
               << msgs::Convert(_msg.xyz())
               << "</xyz>";
      }
      if (_useParentModelFrame >= 0)
      {
        stream << "<use_parent_model_frame>"
               << _useParentModelFrame
               << "</use_parent_model_frame>";
      }

      stream << "<dynamics>";
      if (_msg.has_damping())
      {
        stream << "<damping>"
               << _msg.damping()
               << "</damping>";
      }
      if (_msg.has_friction())
      {
        stream << "<friction>"
               << _msg.friction()
               << "</friction>";
      }
      stream << "</dynamics>";

      stream << "<limit>";
      if (_msg.has_limit_lower())
      {
        stream << "<lower>"
               << _msg.limit_lower()
               << "</lower>";
      }
      if (_msg.has_limit_upper())
      {
        stream << "<upper>"
               << _msg.limit_upper()
               << "</upper>";
      }
      if (_msg.has_limit_effort())
      {
        stream << "<effort>"
               << _msg.limit_effort()
               << "</effort>";
      }
      if (_msg.has_limit_velocity())
      {
        stream << "<velocity>"
               << _msg.limit_velocity()
               << "</velocity>";
      }
      stream << "</limit>";

      stream << "</" << _name << ">";
      return stream.str();
    }

    ////////////////////////////////////////////////////////
    std::string ToSDF(const msgs::Friction &_msg)
    {
      std::ostringstream stream;
      stream
        << "<friction>"
        << "<ode>";
      if (_msg.has_mu())
      {
        stream << "<mu>" << _msg.mu() << "</mu>";
      }
      if (_msg.has_mu2())
      {
        stream << "<mu2>" << _msg.mu2() << "</mu2>";
      }
      if (_msg.has_slip1())
      {
        stream << "<slip1>" << _msg.slip1() << "</slip1>";
      }
      if (_msg.has_slip2())
      {
        stream << "<slip2>" << _msg.slip2() << "</slip2>";
      }
      if (_msg.has_fdir1())
      {
        stream << "<fdir1>" << msgs::Convert(_msg.fdir1()) << "</fdir1>";
      }
      stream
        << "</ode>"
        << "</friction>";
      return stream.str();
    }

    /////////////////////////////////////////////////
    sdf::ElementPtr CameraSensorToSDF(const msgs::CameraSensor &_msg,
        sdf::ElementPtr _sdf)
    {
      sdf::ElementPtr cameraSDF;

      if (_sdf)
      {
        cameraSDF = _sdf;
      }
      else
      {
        cameraSDF.reset(new sdf::Element);
        sdf::initFile("camera.sdf", cameraSDF);
      }

      if (_msg.has_horizontal_fov())
      {
        cameraSDF->GetElement("horizontal_fov")->Set(
            _msg.horizontal_fov());
      }
      if (_msg.has_image_size())
      {
        sdf::ElementPtr imageElem = cameraSDF->GetElement("image");
        imageElem->GetElement("width")->Set(_msg.image_size().x());
        imageElem->GetElement("height")->Set(_msg.image_size().y());
      }
      if (_msg.has_image_format())
      {
        sdf::ElementPtr imageElem = cameraSDF->GetElement("image");
        imageElem->GetElement("format")->Set(_msg.image_format());
      }
      if (_msg.has_near_clip() || _msg.has_far_clip())
      {
        sdf::ElementPtr clipElem = cameraSDF->GetElement("clip");
        if (_msg.has_near_clip())
          clipElem->GetElement("near")->Set(_msg.near_clip());
        if (_msg.has_far_clip())
          clipElem->GetElement("far")->Set(_msg.far_clip());
      }

      if (_msg.has_distortion())
      {
        msgs::Distortion distortionMsg = _msg.distortion();
        sdf::ElementPtr distortionElem =
            cameraSDF->GetElement("distortion");

        if (distortionMsg.has_center())
        {
          distortionElem->GetElement("center")->Set(
              msgs::Convert(distortionMsg.center()));
        }
        if (distortionMsg.has_k1())
        {
          distortionElem->GetElement("k1")->Set(distortionMsg.k1());
        }
        if (distortionMsg.has_k2())
        {
          distortionElem->GetElement("k2")->Set(distortionMsg.k2());
        }
        if (distortionMsg.has_k3())
        {
          distortionElem->GetElement("k3")->Set(distortionMsg.k3());
        }
        if (distortionMsg.has_p1())
        {
          distortionElem->GetElement("p1")->Set(distortionMsg.p1());
        }
        if (distortionMsg.has_p2())
        {
          distortionElem->GetElement("p2")->Set(distortionMsg.p2());
        }
      }
      return cameraSDF;
    }


    /////////////////////////////////////////////////
    sdf::ElementPtr CollisionToSDF(const msgs::Collision &_msg,
        sdf::ElementPtr _sdf)
    {
      sdf::ElementPtr collisionSDF;

      if (_sdf)
      {
        collisionSDF = _sdf;
      }
      else
      {
        collisionSDF.reset(new sdf::Element);
        sdf::initFile("collision.sdf", collisionSDF);
      }

      if (_msg.has_name())
        collisionSDF->GetAttribute("name")->Set(_msg.name());
      if (_msg.has_laser_retro())
        collisionSDF->GetElement("laser_retro")->Set(_msg.laser_retro());
      if (_msg.has_max_contacts())
        collisionSDF->GetElement("max_contacts")->Set(_msg.max_contacts());
      if (_msg.has_pose())
        collisionSDF->GetElement("pose")->Set(msgs::Convert(_msg.pose()));
      if (_msg.has_geometry())
      {
        sdf::ElementPtr geomElem = collisionSDF->GetElement("geometry");
        geomElem = GeometryToSDF(_msg.geometry(), geomElem);
      }
      if (_msg.has_surface())
      {
        sdf::ElementPtr surfaceElem = collisionSDF->GetElement("surface");
        surfaceElem = SurfaceToSDF(_msg.surface(), surfaceElem);
      }

      return collisionSDF;
    }

    /////////////////////////////////////////////////
    sdf::ElementPtr LinkToSDF(const msgs::Link &_msg,
        sdf::ElementPtr _sdf)
    {
      sdf::ElementPtr linkSDF;

      if (_sdf)
      {
        linkSDF = _sdf;
      }
      else
      {
        linkSDF.reset(new sdf::Element);
        sdf::initFile("link.sdf", linkSDF);
      }

      if (_msg.has_name())
        linkSDF->GetAttribute("name")->Set(_msg.name());
      if (_msg.has_gravity())
        linkSDF->GetElement("gravity")->Set(_msg.gravity());
      if (_msg.has_self_collide())
        linkSDF->GetElement("self_collide")->Set(_msg.self_collide());
      if (_msg.has_kinematic())
        linkSDF->GetElement("kinematic")->Set(_msg.kinematic());
      if (_msg.has_pose())
        linkSDF->GetElement("pose")->Set(msgs::Convert(_msg.pose()));
      if (_msg.has_inertial())
      {
        sdf::ElementPtr inertialElem = linkSDF->GetElement("inertial");
        inertialElem = InertialToSDF(_msg.inertial(), inertialElem);
      }
      while (linkSDF->HasElement("collision"))
        linkSDF->GetElement("collision")->RemoveFromParent();
      for (int i = 0; i < _msg.collision_size(); ++i)
      {
        sdf::ElementPtr collisionElem = linkSDF->AddElement("collision");
        collisionElem = CollisionToSDF(_msg.collision(i), collisionElem);
      }

      gzwarn << "msgs::LinkToSDF currently does not convert visual,"
          << " sensor, and projector data" << std::endl;

      return linkSDF;
    }

    /////////////////////////////////////////////////
    sdf::ElementPtr InertialToSDF(const msgs::Inertial &_msg,
        sdf::ElementPtr _sdf)
    {
      sdf::ElementPtr inertialSDF;

      if (_sdf)
      {
        inertialSDF = _sdf;
      }
      else
      {
        inertialSDF.reset(new sdf::Element);
        sdf::initFile("inertial.sdf", inertialSDF);
      }

      if (_msg.has_mass())
        inertialSDF->GetElement("mass")->Set(_msg.mass());
      if (_msg.has_pose())
        inertialSDF->GetElement("pose")->Set(msgs::Convert(_msg.pose()));

      sdf::ElementPtr inertiaSDF = inertialSDF->GetElement("inertia");
      if (_msg.has_ixx())
        inertiaSDF->GetElement("ixx")->Set(_msg.ixx());
      if (_msg.has_ixy())
        inertiaSDF->GetElement("ixy")->Set(_msg.ixy());
      if (_msg.has_ixz())
        inertiaSDF->GetElement("ixz")->Set(_msg.ixz());
      if (_msg.has_iyy())
        inertiaSDF->GetElement("iyy")->Set(_msg.iyy());
      if (_msg.has_iyz())
        inertiaSDF->GetElement("iyz")->Set(_msg.iyz());
      if (_msg.has_izz())
        inertiaSDF->GetElement("izz")->Set(_msg.izz());

      return inertialSDF;
    }

    /////////////////////////////////////////////////
    sdf::ElementPtr SurfaceToSDF(const msgs::Surface &_msg,
        sdf::ElementPtr _sdf)
    {
      sdf::ElementPtr surfaceSDF;

      if (_sdf)
      {
        surfaceSDF = _sdf;
      }
      else
      {
        surfaceSDF.reset(new sdf::Element);
        sdf::initFile("surface.sdf", surfaceSDF);
      }

      if (_msg.has_friction())
      {
        msgs::Friction friction = _msg.friction();
        sdf::ElementPtr frictionElem = surfaceSDF->GetElement("friction");
        sdf::ElementPtr physicsEngElem = frictionElem->GetElement("ode");
        if (friction.has_mu())
          physicsEngElem->GetElement("mu")->Set(friction.mu());
        if (friction.has_mu2())
          physicsEngElem->GetElement("mu2")->Set(friction.mu2());
        if (friction.has_fdir1())
        {
          physicsEngElem->GetElement("fdir1")->Set(
              msgs::Convert(friction.fdir1()));
        }
        if (friction.has_slip1())
          physicsEngElem->GetElement("slip1")->Set(friction.slip1());
        if (friction.has_slip2())
          physicsEngElem->GetElement("slip2")->Set(friction.slip2());
      }
      sdf::ElementPtr bounceElem = surfaceSDF->GetElement("bounce");
      if (_msg.has_restitution_coefficient())
      {
        bounceElem->GetElement("restitution_coefficient")->Set(
            _msg.restitution_coefficient());
      }
      if (_msg.has_bounce_threshold())
      {
        bounceElem->GetElement("threshold")->Set(
            _msg.bounce_threshold());
      }

      sdf::ElementPtr contactElem = surfaceSDF->GetElement("contact");

      if (_msg.has_collide_without_contact())
      {
        contactElem->GetElement("collide_without_contact")->Set(
            _msg.collide_without_contact());
      }
      if (_msg.has_collide_without_contact_bitmask())
      {
        contactElem->GetElement("collide_without_contact_bitmask")->Set(
            _msg.collide_without_contact_bitmask());
      }

      sdf::ElementPtr physicsEngElem = contactElem->GetElement("ode");
      if (_msg.has_soft_cfm())
        physicsEngElem->GetElement("soft_cfm")->Set(_msg.soft_cfm());
      if (_msg.has_soft_erp())
        physicsEngElem->GetElement("soft_erp")->Set(_msg.soft_erp());
      if (_msg.has_kp())
        physicsEngElem->GetElement("kp")->Set(_msg.kp());
      if (_msg.has_kd())
        physicsEngElem->GetElement("kd")->Set(_msg.kd());
      if (_msg.has_max_vel())
        physicsEngElem->GetElement("max_vel")->Set(_msg.max_vel());
      if (_msg.has_min_depth())
        physicsEngElem->GetElement("min_depth")->Set(_msg.min_depth());

      return surfaceSDF;
    }

    /////////////////////////////////////////////////
    sdf::ElementPtr GeometryToSDF(const msgs::Geometry &_msg,
        sdf::ElementPtr _sdf)
    {
      sdf::ElementPtr geometrySDF;

      if (_sdf)
      {
        geometrySDF = _sdf;
      }
      else
      {
        geometrySDF.reset(new sdf::Element);
        sdf::initFile("geometry.sdf", geometrySDF);
      }

      if (!_msg.has_type())
        return geometrySDF;

      if (_msg.type() == msgs::Geometry::BOX)
      {
        sdf::ElementPtr geom = geometrySDF->GetElement("box");
        msgs::BoxGeom boxGeom = _msg.box();
        if (boxGeom.has_size())
          geom->GetElement("size")->Set(msgs::Convert(boxGeom.size()));
      }
      else if (_msg.type() == msgs::Geometry::CYLINDER)
      {
        sdf::ElementPtr geom = geometrySDF->GetElement("cylinder");
        msgs::CylinderGeom cylinderGeom = _msg.cylinder();
        if (cylinderGeom.has_radius())
          geom->GetElement("radius")->Set(cylinderGeom.radius());
        if (cylinderGeom.has_length())
          geom->GetElement("length")->Set(cylinderGeom.length());
      }
      if (_msg.type() == msgs::Geometry::SPHERE)
      {
        sdf::ElementPtr geom = geometrySDF->GetElement("sphere");
        msgs::SphereGeom sphereGeom = _msg.sphere();
        if (sphereGeom.has_radius())
          geom->GetElement("radius")->Set(sphereGeom.radius());
      }
      if (_msg.type() == msgs::Geometry::PLANE)
      {
        sdf::ElementPtr geom = geometrySDF->GetElement("plane");
        msgs::PlaneGeom planeGeom = _msg.plane();
        if (planeGeom.has_normal())
        {
          geom->GetElement("normal")->Set(
              msgs::Convert(planeGeom.normal()));
        }
        if (planeGeom.has_size())
          geom->GetElement("size")->Set(msgs::Convert(planeGeom.size()));
      }
      if (_msg.type() == msgs::Geometry::IMAGE)
      {
        sdf::ElementPtr geom = geometrySDF->GetElement("image");
        msgs::ImageGeom imageGeom = _msg.image();
        if (imageGeom.has_scale())
          geom->GetElement("scale")->Set(imageGeom.scale());
        if (imageGeom.has_height())
          geom->GetElement("height")->Set(imageGeom.height());
        if (imageGeom.has_uri())
          geom->GetElement("uri")->Set(imageGeom.uri());
        if (imageGeom.has_threshold())
          geom->GetElement("threshold")->Set(imageGeom.threshold());
        if (imageGeom.has_granularity())
          geom->GetElement("granularity")->Set(imageGeom.granularity());
      }
      if (_msg.type() == msgs::Geometry::HEIGHTMAP)
      {
        sdf::ElementPtr geom = geometrySDF->GetElement("heightmap");
        msgs::HeightmapGeom heightmapGeom = _msg.heightmap();
        if (heightmapGeom.has_size())
        {
          geom->GetElement("size")->Set(
              msgs::Convert(heightmapGeom.size()));
        }
        if (heightmapGeom.has_origin())
        {
          geom->GetElement("pos")->Set(
              msgs::Convert(heightmapGeom.origin()));
        }
        if (heightmapGeom.has_use_terrain_paging())
        {
          geom->GetElement("use_terrain_paging")->Set(
              heightmapGeom.use_terrain_paging());
        }
        while (geom->HasElement("texture"))
          geom->GetElement("texture")->RemoveFromParent();
        for (int i = 0; i < heightmapGeom.texture_size(); ++i)
        {
          gazebo::msgs::HeightmapGeom_Texture textureMsg =
              heightmapGeom.texture(i);
          sdf::ElementPtr textureElem = geom->AddElement("texture");
          textureElem->GetElement("diffuse")->Set(textureMsg.diffuse());
          textureElem->GetElement("normal")->Set(textureMsg.normal());
          textureElem->GetElement("size")->Set(textureMsg.size());
        }
        while (geom->HasElement("blend"))
          geom->GetElement("blend")->RemoveFromParent();
        for (int i = 0; i < heightmapGeom.blend_size(); ++i)
        {
          gazebo::msgs::HeightmapGeom_Blend blendMsg =
              heightmapGeom.blend(i);
          sdf::ElementPtr blendElem = geom->AddElement("blend");
          blendElem->GetElement("min_height")->Set(blendMsg.min_height());
          blendElem->GetElement("fade_dist")->Set(blendMsg.fade_dist());
        }
        if (heightmapGeom.has_filename())
          geom->GetElement("uri")->Set(heightmapGeom.filename());
      }
      if (_msg.type() == msgs::Geometry::MESH)
      {
        sdf::ElementPtr geom = geometrySDF->GetElement("mesh");
        msgs::MeshGeom meshGeom = _msg.mesh();
        geom = msgs::MeshToSDF(meshGeom, geom);
      }
      if (_msg.type() == msgs::Geometry::POLYLINE)
      {
        sdf::ElementPtr geom = geometrySDF->GetElement("polyline");
        gazebo::msgs::Polyline polylineGeom = _msg.polyline();
        if (polylineGeom.has_height())
          geom->GetElement("height")->Set(polylineGeom.height());
        while (geom->HasElement("point"))
          geom->GetElement("point")->RemoveFromParent();

        for (int i = 0; i < polylineGeom.point_size(); ++i)
        {
          sdf::ElementPtr pointElem = geom->AddElement("point");
          pointElem->Set(msgs::Convert(polylineGeom.point(i)));
        }
      }
      return geometrySDF;
    }

    /////////////////////////////////////////////////
    sdf::ElementPtr MeshToSDF(const msgs::MeshGeom &_msg, sdf::ElementPtr _sdf)
    {
      sdf::ElementPtr meshSDF;

      if (_sdf)
      {
        meshSDF = _sdf;
      }
      else
      {
        meshSDF.reset(new sdf::Element);
        sdf::initFile("mesh_shape.sdf", meshSDF);
      }

      if (_msg.has_filename())
        meshSDF->GetElement("uri")->Set(_msg.filename());

      sdf::ElementPtr submeshElem = meshSDF->GetElement("submesh");
      if (_msg.has_submesh())
        submeshElem->GetElement("name")->Set(_msg.submesh());
      if (_msg.has_center_submesh())
        submeshElem->GetElement("center")->Set(_msg.center_submesh());
      if (_msg.has_scale())
      {
        meshSDF->GetElement("scale")->Set(msgs::Convert(_msg.scale()));
      }

      return meshSDF;
    }

    /////////////////////////////////////////////////
    sdf::ElementPtr PluginToSDF(const msgs::Plugin &_msg, sdf::ElementPtr _sdf)
    {
      sdf::ElementPtr pluginSDF;

      if (_sdf)
      {
        pluginSDF = _sdf;
      }
      else
      {
        pluginSDF.reset(new sdf::Element);
        sdf::initFile("plugin.sdf", pluginSDF);
      }

      // Use the SDF parser to read all the inner xml.
      std::string tmp = "<sdf version='1.5'>";
      tmp += "<plugin name='" + _msg.name() + "' filename='" +
        _msg.filename() + "'>";
      tmp += _msg.innerxml();
      tmp += "</plugin></sdf>";

      sdf::readString(tmp, pluginSDF);

      return pluginSDF;
    }
  }
}
