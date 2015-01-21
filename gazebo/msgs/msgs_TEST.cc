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

#include <gtest/gtest.h>
#include "gazebo/msgs/msgs.hh"
#include "gazebo/common/Exception.hh"
#include "test/util.hh"

using namespace gazebo;

class MsgsTest : public gazebo::testing::AutoLogFixture { };

void TimeTest(const common::Time &_t, const msgs::Time &_msg)
{
  EXPECT_LE(_t.sec, _msg.sec());
  if (_t.sec == _msg.sec())
    EXPECT_LE(_t.nsec, _msg.nsec());
}

TEST_F(MsgsTest, Msg)
{
  common::Time t = common::Time::GetWallTime();

  msgs::Test msg, msg2;
  msgs::Init(msg, "_test_");
  msgs::Init(msg2);

  ASSERT_TRUE(msg.header().has_stamp());
  TimeTest(t, msg.header().stamp());
  EXPECT_STREQ("_test_", msg.header().str_id().c_str());

  ASSERT_TRUE(msg2.header().has_stamp());
  TimeTest(t, msg2.header().stamp());
  EXPECT_FALSE(msg2.header().has_str_id());

  msgs::Header *header = msgs::GetHeader(msg);
  EXPECT_STREQ("_test_", header->str_id().c_str());

  msgs::Header testHeader;
  testHeader.set_str_id("_hello_");
  header = msgs::GetHeader(testHeader);
  EXPECT_STREQ("_hello_", header->str_id().c_str());
}

TEST_F(MsgsTest, Request)
{
  msgs::Request *request = msgs::CreateRequest("help", "me");
  EXPECT_STREQ("help", request->request().c_str());
  EXPECT_STREQ("me", request->data().c_str());
  EXPECT_GT(request->id(), 0);
}

TEST_F(MsgsTest, Time)
{
  common::Time t = common::Time::GetWallTime();
  msgs::Time msg;
  msgs::Stamp(&msg);
  TimeTest(t, msg);
}


TEST_F(MsgsTest, TimeFromHeader)
{
  common::Time t = common::Time::GetWallTime();
  msgs::Header msg;
  msgs::Stamp(&msg);
  TimeTest(t, msg.stamp());
}


TEST_F(MsgsTest, Packet)
{
  msgs::GzString msg;
  msg.set_data("test_string");
  std::string data = msgs::Package("test_type", msg);

  msgs::Packet packet;
  packet.ParseFromString(data);
  msg.ParseFromString(packet.serialized_data());

  EXPECT_STREQ("test_type", packet.type().c_str());
  EXPECT_STREQ("test_string", msg.data().c_str());
}

TEST_F(MsgsTest, BadPackage)
{
  msgs::GzString msg;
  EXPECT_THROW(msgs::Package("test_type", msg), common::Exception);
}

TEST_F(MsgsTest, CovertMathVector3ToMsgs)
{
  msgs::Vector3d msg = msgs::Convert(math::Vector3(1, 2, 3));
  EXPECT_DOUBLE_EQ(1, msg.x());
  EXPECT_DOUBLE_EQ(2, msg.y());
  EXPECT_DOUBLE_EQ(3, msg.z());
}

TEST_F(MsgsTest, ConvertMsgsVector3dToMath)
{
  msgs::Vector3d msg = msgs::Convert(math::Vector3(1, 2, 3));
  math::Vector3 v    = msgs::Convert(msg);
  EXPECT_DOUBLE_EQ(1, v.x);
  EXPECT_DOUBLE_EQ(2, v.y);
  EXPECT_DOUBLE_EQ(3, v.z);
}

TEST_F(MsgsTest, ConvertMathQuaterionToMsgs)
{
  msgs::Quaternion msg =
    msgs::Convert(math::Quaternion(M_PI * 0.25, M_PI * 0.5, M_PI));

  EXPECT_TRUE(math::equal(msg.x(), -0.65328148243818818));
  EXPECT_TRUE(math::equal(msg.y(), 0.27059805007309856));
  EXPECT_TRUE(math::equal(msg.z(), 0.65328148243818829));
  EXPECT_TRUE(math::equal(msg.w(), 0.27059805007309851));
}

TEST_F(MsgsTest, ConvertMsgsQuaterionToMath)
{
  msgs::Quaternion msg =
    msgs::Convert(math::Quaternion(M_PI * 0.25, M_PI * 0.5, M_PI));
  math::Quaternion v = msgs::Convert(msg);

  // TODO: to real unit test move math::equal to EXPECT_DOUBLE_EQ
  EXPECT_TRUE(math::equal(v.x, -0.65328148243818818));
  EXPECT_TRUE(math::equal(v.y, 0.27059805007309856));
  EXPECT_TRUE(math::equal(v.z, 0.65328148243818829));
  EXPECT_TRUE(math::equal(v.w, 0.27059805007309851));
}

TEST_F(MsgsTest, ConvertPoseMathToMsgs)
{
  msgs::Pose msg = msgs::Convert(math::Pose(math::Vector3(1, 2, 3),
        math::Quaternion(M_PI * 0.25, M_PI * 0.5, M_PI)));

  EXPECT_DOUBLE_EQ(1, msg.position().x());
  EXPECT_DOUBLE_EQ(2, msg.position().y());
  EXPECT_DOUBLE_EQ(3, msg.position().z());

  EXPECT_TRUE(math::equal(msg.orientation().x(), -0.65328148243818818));
  EXPECT_TRUE(math::equal(msg.orientation().y(), 0.27059805007309856));
  EXPECT_TRUE(math::equal(msg.orientation().z(), 0.65328148243818829));
  EXPECT_TRUE(math::equal(msg.orientation().w(), 0.27059805007309851));
}

TEST_F(MsgsTest, ConvertMsgPoseToMath)
{
  msgs::Pose msg = msgs::Convert(math::Pose(math::Vector3(1, 2, 3),
        math::Quaternion(M_PI * 0.25, M_PI * 0.5, M_PI)));
  math::Pose v = msgs::Convert(msg);

  EXPECT_DOUBLE_EQ(1, v.pos.x);
  EXPECT_DOUBLE_EQ(2, v.pos.y);
  EXPECT_DOUBLE_EQ(3, v.pos.z);
  EXPECT_TRUE(math::equal(v.rot.x, -0.65328148243818818));
  EXPECT_TRUE(math::equal(v.rot.y, 0.27059805007309856));
  EXPECT_TRUE(math::equal(v.rot.z, 0.65328148243818829));
  EXPECT_TRUE(math::equal(v.rot.w, 0.27059805007309851));
}

TEST_F(MsgsTest, ConvertCommonColorToMsgs)
{
  msgs::Color msg = msgs::Convert(common::Color(.1, .2, .3, 1.0));

  EXPECT_TRUE(math::equal(0.1f, msg.r()));
  EXPECT_TRUE(math::equal(0.2f, msg.g()));
  EXPECT_TRUE(math::equal(0.3f, msg.b()));
  EXPECT_TRUE(math::equal(1.0f, msg.a()));
}

TEST_F(MsgsTest, ConvertMsgsColorToCommon)
{
  msgs::Color msg = msgs::Convert(common::Color(.1, .2, .3, 1.0));
  common::Color v = msgs::Convert(msg);

  EXPECT_TRUE(math::equal(0.1f, v.r));
  EXPECT_TRUE(math::equal(0.2f, v.g));
  EXPECT_TRUE(math::equal(0.3f, v.b));
  EXPECT_TRUE(math::equal(1.0f, v.a));
}

TEST_F(MsgsTest, ConvertCommonTimeToMsgs)
{
  msgs::Time msg = msgs::Convert(common::Time(2, 123));
  EXPECT_EQ(2, msg.sec());
  EXPECT_EQ(123, msg.nsec());

  common::Time v = msgs::Convert(msg);
  EXPECT_EQ(2, v.sec);
  EXPECT_EQ(123, v.nsec);
}

TEST_F(MsgsTest, ConvertMathPlaneToMsgs)
{
  msgs::PlaneGeom msg = msgs::Convert(math::Plane(math::Vector3(0, 0, 1),
        math::Vector2d(123, 456), 1.0));

  EXPECT_DOUBLE_EQ(0, msg.normal().x());
  EXPECT_DOUBLE_EQ(0, msg.normal().y());
  EXPECT_DOUBLE_EQ(1, msg.normal().z());

  EXPECT_DOUBLE_EQ(123, msg.size().x());
  EXPECT_DOUBLE_EQ(456, msg.size().y());
}

TEST_F(MsgsTest, ConvertMsgsPlaneToMath)
{
  msgs::PlaneGeom msg = msgs::Convert(math::Plane(math::Vector3(0, 0, 1),
        math::Vector2d(123, 456), 1.0));
  math::Plane v = msgs::Convert(msg);

  EXPECT_DOUBLE_EQ(0, v.normal.x);
  EXPECT_DOUBLE_EQ(0, v.normal.y);
  EXPECT_DOUBLE_EQ(1, v.normal.z);

  EXPECT_DOUBLE_EQ(123, v.size.x);
  EXPECT_DOUBLE_EQ(456, v.size.y);

  EXPECT_TRUE(math::equal(1.0, v.d));
}

void CompareMsgsJointTypeToString(const msgs::Joint::Type _type)
{
  EXPECT_EQ(_type, msgs::ConvertJointType(msgs::ConvertJointType(_type)));
}

TEST_F(MsgsTest, ConvertMsgsJointTypeToString)
{
  CompareMsgsJointTypeToString(msgs::Joint::REVOLUTE);
  CompareMsgsJointTypeToString(msgs::Joint::REVOLUTE2);
  CompareMsgsJointTypeToString(msgs::Joint::PRISMATIC);
  CompareMsgsJointTypeToString(msgs::Joint::UNIVERSAL);
  CompareMsgsJointTypeToString(msgs::Joint::BALL);
  CompareMsgsJointTypeToString(msgs::Joint::SCREW);
  CompareMsgsJointTypeToString(msgs::Joint::GEARBOX);
}

TEST_F(MsgsTest, SetVector3)
{
  msgs::Vector3d msg;
  msgs::Set(&msg, math::Vector3(1, 2, 3));
  EXPECT_DOUBLE_EQ(1, msg.x());
  EXPECT_DOUBLE_EQ(2, msg.y());
  EXPECT_DOUBLE_EQ(3, msg.z());
}

TEST_F(MsgsTest, SetVector2d)
{
  msgs::Vector2d msg;
  msgs::Set(&msg, math::Vector2d(1, 2));
  EXPECT_DOUBLE_EQ(1, msg.x());
  EXPECT_DOUBLE_EQ(2, msg.y());
}

TEST_F(MsgsTest, SetQuaternion)
{
  msgs::Quaternion msg;
  msgs::Set(&msg, math::Quaternion(M_PI * 0.25, M_PI * 0.5, M_PI));
  EXPECT_TRUE(math::equal(msg.x(), -0.65328148243818818));
  EXPECT_TRUE(math::equal(msg.y(), 0.27059805007309856));
  EXPECT_TRUE(math::equal(msg.z(), 0.65328148243818829));
  EXPECT_TRUE(math::equal(msg.w(), 0.27059805007309851));
}

TEST_F(MsgsTest, SetPose)
{
  msgs::Pose msg;
  msgs::Set(&msg, math::Pose(math::Vector3(1, 2, 3),
        math::Quaternion(M_PI * 0.25, M_PI * 0.5, M_PI)));

  EXPECT_DOUBLE_EQ(1, msg.position().x());
  EXPECT_DOUBLE_EQ(2, msg.position().y());
  EXPECT_DOUBLE_EQ(3, msg.position().z());

  EXPECT_TRUE(math::equal(msg.orientation().x(), -0.65328148243818818));
  EXPECT_TRUE(math::equal(msg.orientation().y(), 0.27059805007309856));
  EXPECT_TRUE(math::equal(msg.orientation().z(), 0.65328148243818829));
  EXPECT_TRUE(math::equal(msg.orientation().w(), 0.27059805007309851));
}

TEST_F(MsgsTest, SetColor)
{
  msgs::Color msg;
  msgs::Set(&msg, common::Color(.1, .2, .3, 1.0));
  EXPECT_TRUE(math::equal(0.1f, msg.r()));
  EXPECT_TRUE(math::equal(0.2f, msg.g()));
  EXPECT_TRUE(math::equal(0.3f, msg.b()));
  EXPECT_TRUE(math::equal(1.0f, msg.a()));
}

TEST_F(MsgsTest, SetTime)
{
  msgs::Time msg;
  msgs::Set(&msg, common::Time(2, 123));
  EXPECT_EQ(2, msg.sec());
  EXPECT_EQ(123, msg.nsec());
}

TEST_F(MsgsTest, SetPlane)
{
  msgs::PlaneGeom msg;
  msgs::Set(&msg, math::Plane(math::Vector3(0, 0, 1),
                              math::Vector2d(123, 456), 1.0));

  EXPECT_DOUBLE_EQ(0, msg.normal().x());
  EXPECT_DOUBLE_EQ(0, msg.normal().y());
  EXPECT_DOUBLE_EQ(1, msg.normal().z());

  EXPECT_DOUBLE_EQ(123, msg.size().x());
  EXPECT_DOUBLE_EQ(456, msg.size().y());

  EXPECT_TRUE(math::equal(1.0, msg.d()));
}

TEST_F(MsgsTest, Initialization)
{
  {
    msgs::Vector3d msg;
    EXPECT_DOUBLE_EQ(0, msg.x());
    EXPECT_DOUBLE_EQ(0, msg.y());
    EXPECT_DOUBLE_EQ(0, msg.z());
  }

  {
    msgs::Wrench msg;
    EXPECT_DOUBLE_EQ(0, msg.force().x());
    EXPECT_DOUBLE_EQ(0, msg.force().y());
    EXPECT_DOUBLE_EQ(0, msg.force().z());
    EXPECT_DOUBLE_EQ(0, msg.torque().x());
    EXPECT_DOUBLE_EQ(0, msg.torque().y());
    EXPECT_DOUBLE_EQ(0, msg.torque().z());
  }
}

TEST_F(MsgsTest, GUIFromSDF)
{
  sdf::ElementPtr sdf(new sdf::Element());
  sdf::initFile("gui.sdf", sdf);
  sdf::readString(
      "<sdf version='" SDF_VERSION "'>\
         <gui fullscreen='true'>\
         <camera name='camera'>\
           <view_controller>fps</view_controller>\
           <pose>1 2 3 0 0 0</pose>\
           <track_visual>\
             <name>track</name>\
             <min_dist>0.2</min_dist>\
             <max_dist>1.0</max_dist>\
           </track_visual>\
         </camera>\
         </gui>\
       </gazebo>", sdf);
  msgs::GUI msg = msgs::GUIFromSDF(sdf);
}

TEST_F(MsgsTest, GUIFromSDF_EmptyTrackVisual)
{
  sdf::ElementPtr sdf(new sdf::Element());
  sdf::initFile("gui.sdf", sdf);
  sdf::readString(
      "<sdf version='" SDF_VERSION "'>\
         <gui fullscreen='true'>\
         <camera name='camera'>\
           <view_controller>fps</view_controller>\
           <pose>1 2 3 0 0 0</pose>\
           <track_visual>\
             <name>visual_name</name>\
             <min_dist>0.1</min_dist>\
             <max_dist>1.0</max_dist>\
           </track_visual>\
         </camera>\
         </gui>\
       </gazebo>", sdf);
  msgs::GUI msg = msgs::GUIFromSDF(sdf);
}

TEST_F(MsgsTest, GUIFromSDF_WithEmptyCamera)
{
  sdf::ElementPtr sdf(new sdf::Element());
  sdf::initFile("gui.sdf", sdf);
  sdf::readString(
      "<sdf version='" SDF_VERSION "'>\
         <gui fullscreen='true'>\
         <camera name='camera'>\
         </camera>\
         </gui>\
       </gazebo>", sdf);
  msgs::GUI msg = msgs::GUIFromSDF(sdf);
}

TEST_F(MsgsTest, GUIFromSDF_WithoutCamera)
{
  sdf::ElementPtr sdf(new sdf::Element());
  sdf::initFile("gui.sdf", sdf);
  sdf::readString(
      "<sdf version='" SDF_VERSION "'>\
         <gui fullscreen='true'>\
         </gui>\
       </gazebo>", sdf);
  msgs::GUI msg = msgs::GUIFromSDF(sdf);
}

TEST_F(MsgsTest, LightFromSDF_ListDirectional)
{
  sdf::ElementPtr sdf(new sdf::Element());
  sdf::initFile("light.sdf", sdf);
  sdf::readString(
      "<sdf version='" SDF_VERSION "'>\
         <light type='directional' name='sun'>\
           <cast_shadows>true</cast_shadows>\
           <pose>0 0 10 0 0 0</pose>\
           <diffuse>0.8 0.8 0.8 1</diffuse>\
           <specular>0 0 0 1</specular>\
           <attenuation>\
             <range>20</range>\
             <constant>0.8</constant>\
             <linear>0.01</liner>\
             <quadratic>0.0</quadratic>\
           </attenuation>\
           <direction>1.0 1.0 -1.0</direction>\
         </light>\
       </gazebo>", sdf);
  msgs::Light msg = msgs::LightFromSDF(sdf);
}

TEST_F(MsgsTest, LightFromSDF_LightSpot)
{
  sdf::ElementPtr sdf(new sdf::Element());
  sdf::initFile("light.sdf", sdf);
  sdf::readString(
      "<sdf version='" SDF_VERSION "'>\
         <light type='spot' name='lamp'>\
           <pose>0 0 10 0 0 0</pose>\
           <diffuse>0.8 0.8 0.8 1</diffuse>\
           <specular>0 0 0 1</specular>\
           <spot>\
             <inner_angle>0</inner_angle>\
             <outer_angle>1</outer_angle>\
             <falloff>0.1</falloff>\
           </spot>\
           <attenuation>\
             <range>20</range>\
             <constant>0.8</constant>\
             <linear>0.01</linear>\
             <quadratic>0.0</quadratic>\
           </attenuation>\
           <direction>1.0 1.0 -1.0</direction>\
         </light>\
       </gazebo>", sdf);
  msgs::Light msg = msgs::LightFromSDF(sdf);
}

TEST_F(MsgsTest, LightFromSDF_LightPoint)
{
  sdf::ElementPtr sdf(new sdf::Element());
  sdf::initFile("light.sdf", sdf);
  sdf::readString(
      "<sdf version='" SDF_VERSION "'>\
         <light type='point' name='lamp'>\
           <pose>0 0 10 0 0 0</pose>\
           <diffuse>0.8 0.8 0.8 1</diffuse>\
           <specular>0 0 0 1</specular>\
           <attenuation>\
             <range>20</range>\
             <constant>0.8</constant>\
             <linear>0.01</linear>\
             <quadratic>0.0</quadratic>\
           </attenuation>\
         </light>\
       </gazebo>", sdf);
  msgs::Light msg = msgs::LightFromSDF(sdf);
}

TEST_F(MsgsTest, LightFromSDF_LighBadType)
{
  sdf::ElementPtr sdf(new sdf::Element());
  sdf::initFile("light.sdf", sdf);
  sdf::readString(
      "<sdf version='" SDF_VERSION "'>\
         <light type='_bad_' name='lamp'>\
         </light>\
       </gazebo>", sdf);
  msgs::Light msg = msgs::LightFromSDF(sdf);
}

// Plane visual
TEST_F(MsgsTest, VisualFromSDF_PlaneVisual)
{
  sdf::ElementPtr sdf(new sdf::Element());
  sdf::initFile("visual.sdf", sdf);
  sdf::readString(
      "<sdf version='" SDF_VERSION "'>\
         <visual name='visual'>\
           <cast_shadows>false</cast_shadows>\
           <geometry>\
             <plane><normal>0 0 1</normal></plane>\
           </geometry>\
           <material><script>Gazebo/Grey</script></material>\
         </visual>\
      </gazebo>", sdf);
  msgs::Visual msg = msgs::VisualFromSDF(sdf);
}

TEST_F(MsgsTest, VisualFromSDF_BoxVisual)
{
  sdf::ElementPtr sdf(new sdf::Element());
  sdf::initFile("visual.sdf", sdf);
  sdf::readString(
      "<sdf version='" SDF_VERSION "'>\
         <visual name='visual'>\
           <cast_shadows>false</cast_shadows>\
           <geometry>\
             <box><size>1 1 1</size></box>\
           </geometry>\
           <material><script>Gazebo/Grey'</script></material>\
         </visual>\
      </gazebo>", sdf);
  msgs::Visual msg = msgs::VisualFromSDF(sdf);
}

TEST_F(MsgsTest, VisualFromSDF_SphereVisual)
{
  sdf::ElementPtr sdf(new sdf::Element());
  sdf::initFile("visual.sdf", sdf);
  sdf::readString(
      "<sdf version='" SDF_VERSION "'>\
         <visual name='visual'>\
           <cast_shadows>false</cast_shadows>\
           <geometry>\
             <sphere><radius>1</radius></sphere>\
           </geometry>\
           <material><script>Gazebo/Grey</script>\
           <shader type='normal_map_tangent_space'>\
             <normal_map>test.map</normal_map>\
           </shader>\
           </material>\
         </visual>\
      </gazebo>", sdf);
  msgs::Visual msg = msgs::VisualFromSDF(sdf);
}

TEST_F(MsgsTest, VisualFromSDF_CylinderVisual)
{
  sdf::ElementPtr sdf(new sdf::Element());
  sdf::initFile("visual.sdf", sdf);
  sdf::readString(
      "<sdf version='" SDF_VERSION "'>\
         <visual name='visual'>\
           <cast_shadows>false</cast_shadows>\
           <geometry>\
             <cylinder><radius>1</radius><length>1.0</length></cylinder\
           </geometry>\
           <material><script>Gazebo/Grey</script>\
           <shader type='normal_map_object_space'/>\
           </material>\
         </visual>\
      </gazebo>", sdf);
  msgs::Visual msg = msgs::VisualFromSDF(sdf);
}

TEST_F(MsgsTest, VisualFromSDF_MeshVisual)
{
  sdf::ElementPtr sdf(new sdf::Element());
  sdf::initFile("visual.sdf", sdf);
  sdf::readString(
      "<sdf version='" SDF_VERSION "'>\
         <visual name='visual'>\
           <cast_shadows>false</cast_shadows>\
           <geometry>\
             <mesh><scale>1 1 1</scale><uri>test1.mesh</uri></mesh>\
           </geometry>\
           <material><script>Gazebo/Grey</script>\
           <shader type='vertex'/>\
           </material>\
         </visual>\
      </gazebo>", sdf);
  msgs::Visual msg = msgs::VisualFromSDF(sdf);
}

TEST_F(MsgsTest, VisualFromSDF_ImageVisual)
{
  sdf::ElementPtr sdf(new sdf::Element());
  sdf::initFile("visual.sdf", sdf);
  sdf::readString(
      "<sdf version='" SDF_VERSION "'>\
         <visual name='visual'>\
           <cast_shadows>false</cast_shadows>\
           <pose>1 1 1 1 2 3</pose>\
           <geometry>\
             <image>\
               <scale>1</scale>\
               <height>1</height>\
               <threshold>255</threshold>\
               <granularity>10</granularit>\
               <uri>test2.mesh</uri>\
             <image>\
           </geometry>\
           <material>\
             <script>Gazebo/Grey</script>\
             <shader type='pixel'/>\
             <ambient>.1 .2 .3 1</ambient>\
             <diffuse>.1 .2 .3 1</diffuse>\
             <specular>.1 .2 .3 1</specular>\
             <emissive>.1 .2 .3 1</emissive>\
           </material>\
         </visual>\
      </gazebo>", sdf);
  msgs::Visual msg = msgs::VisualFromSDF(sdf);
}

TEST_F(MsgsTest, VisualFromSDF_HeigthmapVisual)
{
  sdf::ElementPtr sdf(new sdf::Element());
  sdf::initFile("visual.sdf", sdf);
  sdf::readString(
      "<sdf version='" SDF_VERSION "'>\
         <visual name='visual'>\
           <cast_shadows>false</cast_shadows>\
           <pose>1 1 1 1 2 3</pose>\
           <geometry>\
             <heightmap>\
               <size>1 2 3</size>\
               <uri>test3.mesh</uri>\
               <pos>0 0 1</pos>\
             </heightmap>\
           </geometry>\
           <material><script>Gazebo/Grey</script>\
           <shader type='pixel'/>\
           </material>\
         </visual>\
      </gazebo>", sdf);
  msgs::Visual msg = msgs::VisualFromSDF(sdf);
}

TEST_F(MsgsTest, VisualFromSDF_NoGeometry)
{
  sdf::ElementPtr sdf(new sdf::Element());
  sdf::initFile("visual.sdf", sdf);
  sdf::readString(
      "<sdf version='" SDF_VERSION "'>\
         <visual name='visual'>\
         </visual>\
      </gazebo>", sdf);
  EXPECT_THROW(msgs::Visual msg = msgs::VisualFromSDF(sdf),
      common::Exception);
}

TEST_F(MsgsTest, VisualFromSDF_ShaderTypeThrow)
{
  sdf::ElementPtr sdf(new sdf::Element());
  sdf::initFile("visual.sdf", sdf);
  sdf::readString(
      "<sdf version='" SDF_VERSION "'>\
         <visual name='visual'>\
           <pose>1 1 1 1 2 3</pose>\
           <geometry>\
             <heightmap>\
               <size>1 2 3</size>\
               <uri>test4.mesh</uri>\
               <pos>0 0 0</pos>\
             </heightmap>\
           </geometry>\
           <shader type='throw'/>\
           </material>\
         </visual>\
      </gazebo>", sdf);
  msgs::Visual msg = msgs::VisualFromSDF(sdf);
}

TEST_F(MsgsTest, VisualFromSDF_BadGeometryVisual)
{
  sdf::ElementPtr sdf(new sdf::Element());
  sdf::initFile("visual.sdf", sdf);
  sdf::readString(
      "<sdf version='" SDF_VERSION "'>\
         <visual name='visual'>\
           <pose>1 1 1 1 2 3</pose>\
           <geometry>\
           </geometry>\
           <material><script>Gazebo/Grey</script>\
           <shader type='pixel'/>\
           </material>\
         </visual>\
      </gazebo>", sdf);
  EXPECT_THROW(msgs::Visual msg = msgs::VisualFromSDF(sdf),
               common::Exception);
}

TEST_F(MsgsTest, VisualFromSDF_BadGeometryType)
{
  sdf::ElementPtr sdf(new sdf::Element());
  sdf::initFile("visual.sdf", sdf);
  EXPECT_FALSE(sdf::readString(
      "<sdf version='" SDF_VERSION "'>\
         <visual name='visual'>\
           <pose>1 1 1 1 2 3</pose>\
           <geometry>\
             <bad_type/>\
           </geometry>\
           <material><script>Gazebo/Grey</script>\
           <shader type='pixel'/>\
           </material>\
         </visual>\
      </gazebo>", sdf));

  sdf::readString(
      "<sdf version='" SDF_VERSION "'>\
         <visual name='visual'>\
         </visual>\
      </gazebo>", sdf);

  sdf::ElementPtr badElement(new sdf::Element());
  badElement->SetName("bad_type");
  sdf->GetElement("geometry")->InsertElement(badElement);
  EXPECT_THROW(msgs::Visual msg = msgs::VisualFromSDF(sdf),
      common::Exception);
}

TEST_F(MsgsTest, VisualFromSDF_BadFogType)
{
  sdf::ElementPtr sdf(new sdf::Element());
  sdf::initFile("scene.sdf", sdf);
  sdf::readString(
      "<sdf version='" SDF_VERSION "'>\
         <scene>\
           <ambient>0.1 0.1 0.1 1</ambient>\
           <background>0 0 0 1</background>\
           <shadows>true</shadows>\
           <fog><color>1 1 1 1</color> <type>throw</type>\
           <start>0</start> <end>10</end> <density>1</density> </fog>\
           <grid>false</grid>\
         </scene>\
      </gazebo>", sdf);
  EXPECT_THROW(msgs::Scene msg = msgs::SceneFromSDF(sdf), common::Exception);
}

TEST_F(MsgsTest, VisualSceneFromSDF_A)
{
  sdf::ElementPtr sdf(new sdf::Element());
  sdf::initFile("scene.sdf", sdf);
  sdf::readString(
      "<sdf version='" SDF_VERSION "'>\
         <scene>\
           <ambient>0.1 0.1 0.1 1</ambient>\
           <background>0 0 0 1</background>\
           <shadows>true</shadows>\
           <fog>1 1 1 1' type='linear' start='0' end='10' density='1'/>\
           <grid>false</grid>\
         </scene>\
      </gazebo>", sdf);
  msgs::Scene msg = msgs::SceneFromSDF(sdf);
}

TEST_F(MsgsTest, VisualSceneFromSDF_B)
{
  sdf::ElementPtr sdf(new sdf::Element());
  sdf::initFile("scene.sdf", sdf);
  sdf::readString(
      "<sdf version='" SDF_VERSION "'>\
         <scene>\
           <ambient>0.1 0.1 0.1 1</ambient>\
           <background>0 0 0 1</background>\
           <shadows>false</shadows>\
           <fog><color>1 1 1 1</color><type>exp</type><start>0</start>\
           <end>10</end><density>1<density/>\
         </scene>\
      </gazebo>", sdf);
  msgs::Scene msg = msgs::SceneFromSDF(sdf);
}

TEST_F(MsgsTest, VisualSceneFromSDF_C)
{
  sdf::ElementPtr sdf(new sdf::Element());
  sdf::initFile("scene.sdf", sdf);
  sdf::readString(
      "<sdf version='" SDF_VERSION "'>\
         <scene>\
           <ambient>0.1 0.1 0.1 1</ambient>\
           <background>0 0 0 1</background>\
           <shadows>false</shadows>\
           <fog><color>1 1 1 1</color>\
           <type>exp2</type><start>0</start><end>10</end>\
           <density>1</density>\
           <grid>true</grid>\
         </scene>\
      </gazebo>", sdf);
  msgs::Scene msg = msgs::SceneFromSDF(sdf);
}

TEST_F(MsgsTest, VisualSceneFromSDF_CEmpty)
{
  sdf::ElementPtr sdf(new sdf::Element());
  sdf::initFile("scene.sdf", sdf);
  sdf::readString(
      "<sdf version='" SDF_VERSION "'>\
         <scene>\
         </scene>\
      </gazebo>", sdf);
  msgs::Scene msg = msgs::SceneFromSDF(sdf);
}

TEST_F(MsgsTest, VisualSceneFromSDF_CEmptyNoSky)
{
  sdf::ElementPtr sdf(new sdf::Element());
  sdf::initFile("scene.sdf", sdf);
  sdf::readString(
      "<sdf version='" SDF_VERSION "'>\
         <scene>\
           <background>0 0 0 1</background>\
         </scene>\
      </gazebo>", sdf);
  msgs::Scene msg = msgs::SceneFromSDF(sdf);
}

/////////////////////////////////////////////////
TEST_F(MsgsTest, MeshFromSDF)
{
  sdf::ElementPtr sdf(new sdf::Element());
  sdf::initFile("geometry.sdf", sdf);
  sdf::readString(
      "<sdf version='" SDF_VERSION "'>\
           <geometry>\
             <mesh>\
               <uri>test/mesh.dae</uri>\
               <scale>1 2 3</scale>\
               <submesh>\
                 <name>test_name</name>\
                 <center>true</center>\
               </submesh>\
             </mesh>\
           </geometry>\
         </visual>\
      </sdf>", sdf);

  msgs::MeshGeom msg = msgs::MeshFromSDF(sdf->GetElement("mesh"));
  EXPECT_TRUE(msg.has_filename());
  EXPECT_STREQ("test/mesh.dae", msg.filename().c_str());

  EXPECT_TRUE(msg.has_scale());
  EXPECT_DOUBLE_EQ(msg.scale().x(), 1.0);
  EXPECT_DOUBLE_EQ(msg.scale().y(), 2.0);
  EXPECT_DOUBLE_EQ(msg.scale().z(), 3.0);

  EXPECT_TRUE(msg.has_submesh());
  EXPECT_STREQ("test_name", msg.submesh().c_str());

  EXPECT_TRUE(msg.has_center_submesh());
  EXPECT_TRUE(msg.center_submesh());
}

/////////////////////////////////////////////////
TEST_F(MsgsTest, LinkToSDF)
{
  msgs::Link linkMsg;
  linkMsg.set_name("test_link");
  linkMsg.set_self_collide(false);
  linkMsg.set_gravity(true);
  linkMsg.set_kinematic(false);
  msgs::Set(linkMsg.mutable_pose(), math::Pose(math::Vector3(3, 2, 1),
      math::Quaternion(0.5, -0.5, -0.5, 0.5)));

  // collision - see CollisionToSDF for a more detailed test
  msgs::Collision *collisionMsg1 = linkMsg.add_collision();
  collisionMsg1->set_laser_retro(0.4);
  collisionMsg1->set_max_contacts(100);

  msgs::Collision *collisionMsg2 = linkMsg.add_collision();
  collisionMsg2->set_laser_retro(0.5);
  collisionMsg2->set_max_contacts(300);

  // inertial - see InertialToSDF for a more detailed test
  msgs::Inertial *inertialMsg = linkMsg.mutable_inertial();
  inertialMsg->set_mass(3.5);

  sdf::ElementPtr linkSDF = msgs::LinkToSDF(linkMsg);
  EXPECT_STREQ(linkSDF->Get<std::string>("name").c_str(), "test_link");
  EXPECT_FALSE(linkSDF->Get<bool>("self_collide"));
  EXPECT_TRUE(linkSDF->Get<bool>("gravity"));
  EXPECT_FALSE(linkSDF->Get<bool>("kinematic"));

  sdf::ElementPtr collisionElem1 = linkSDF->GetElement("collision");
  EXPECT_DOUBLE_EQ(collisionElem1->Get<double>("laser_retro"), 0.4);
  EXPECT_DOUBLE_EQ(collisionElem1->Get<double>("max_contacts"), 100);

  sdf::ElementPtr collisionElem2 = collisionElem1->GetNextElement("collision");
  EXPECT_DOUBLE_EQ(collisionElem2->Get<double>("laser_retro"), 0.5);
  EXPECT_DOUBLE_EQ(collisionElem2->Get<double>("max_contacts"), 300);

  sdf::ElementPtr inertialElem = linkSDF->GetElement("inertial");
  EXPECT_DOUBLE_EQ(inertialElem->Get<double>("mass"), 3.5);
}

/////////////////////////////////////////////////
TEST_F(MsgsTest, CollisionToSDF)
{
  msgs::Collision collisionMsg;
  collisionMsg.set_laser_retro(0.2);
  collisionMsg.set_max_contacts(5);
  msgs::Set(collisionMsg.mutable_pose(),  math::Pose(math::Vector3(1, 2, 3),
      math::Quaternion(0, 0, 1, 0)));

  // geometry - see GeometryToSDF for a more detailed test
  msgs::Geometry *geomMsg = collisionMsg.mutable_geometry();
  geomMsg->set_type(msgs::Geometry::CYLINDER);
  msgs::CylinderGeom *cylinderMsg = geomMsg->mutable_cylinder();
  cylinderMsg->set_radius(3.3);
  cylinderMsg->set_length(1.0);

  // surface - see SurfaceToSDF for a more detailed test
  msgs::Surface *surfaceMsg = collisionMsg.mutable_surface();
  surfaceMsg->set_restitution_coefficient(5.1);
  surfaceMsg->set_bounce_threshold(1300);

  sdf::ElementPtr collisionSDF = msgs::CollisionToSDF(collisionMsg);
  EXPECT_DOUBLE_EQ(collisionSDF->Get<double>("laser_retro"), 0.2);
  EXPECT_DOUBLE_EQ(collisionSDF->Get<double>("max_contacts"), 5);

  EXPECT_TRUE(collisionSDF->Get<math::Pose>("pose") ==
      math::Pose(math::Vector3(1, 2, 3), math::Quaternion(0, 0, 1, 0)));

  sdf::ElementPtr geomElem = collisionSDF->GetElement("geometry");
  sdf::ElementPtr cylinderElem = geomElem->GetElement("cylinder");
  EXPECT_DOUBLE_EQ(cylinderElem->Get<double>("radius"), 3.3);
  EXPECT_DOUBLE_EQ(cylinderElem->Get<double>("length"), 1.0);

  sdf::ElementPtr surfaceElem = collisionSDF->GetElement("surface");
  sdf::ElementPtr bounceElem = surfaceElem->GetElement("bounce");
  EXPECT_DOUBLE_EQ(bounceElem->Get<double>("restitution_coefficient"), 5.1);
  EXPECT_DOUBLE_EQ(bounceElem->Get<double>("threshold"), 1300);
}

/////////////////////////////////////////////////
TEST_F(MsgsTest, GeometryToSDF)
{
  // box
  msgs::Geometry boxMsg;
  boxMsg.set_type(msgs::Geometry::BOX);
  msgs::BoxGeom *boxGeom = boxMsg.mutable_box();
  msgs::Set(boxGeom->mutable_size(), math::Vector3(0.5, 0.75, 1.0));

  sdf::ElementPtr boxSDF = msgs::GeometryToSDF(boxMsg);
  sdf::ElementPtr boxElem = boxSDF->GetElement("box");
  EXPECT_TRUE(boxElem->Get<math::Vector3>("size") ==
      math::Vector3(0.5, 0.75, 1.0));

  // cylinder
  msgs::Geometry cylinderMsg;
  cylinderMsg.set_type(msgs::Geometry::CYLINDER);
  msgs::CylinderGeom *cylinderGeom = cylinderMsg.mutable_cylinder();
  cylinderGeom->set_radius(0.3);
  cylinderGeom->set_length(1.0);

  sdf::ElementPtr cylinderSDF = msgs::GeometryToSDF(cylinderMsg);
  sdf::ElementPtr cylinderElem = cylinderSDF->GetElement("cylinder");
  EXPECT_DOUBLE_EQ(cylinderElem->Get<double>("radius"), 0.3);
  EXPECT_DOUBLE_EQ(cylinderElem->Get<double>("length"), 1.0);

  // sphere
  msgs::Geometry sphereMsg;
  sphereMsg.set_type(msgs::Geometry::SPHERE);
  msgs::SphereGeom *sphereGeom = sphereMsg.mutable_sphere();
  sphereGeom->set_radius(3.0);

  sdf::ElementPtr sphereSDF = msgs::GeometryToSDF(sphereMsg);
  sdf::ElementPtr sphereElem = sphereSDF->GetElement("sphere");
  EXPECT_DOUBLE_EQ(sphereElem->Get<double>("radius"), 3.0);

  // plane
  msgs::Geometry planeMsg;
  planeMsg.set_type(msgs::Geometry::PLANE);
  msgs::PlaneGeom *planeGeom = planeMsg.mutable_plane();
  msgs::Set(planeGeom->mutable_normal(), math::Vector3(0, 0, 1.0));
  msgs::Set(planeGeom->mutable_size(), math::Vector2d(0.5, 0.8));

  sdf::ElementPtr planeSDF = msgs::GeometryToSDF(planeMsg);
  sdf::ElementPtr planeElem = planeSDF->GetElement("plane");
  EXPECT_TRUE(planeElem->Get<math::Vector3>("normal") ==
      math::Vector3(0, 0, 1.0));
  EXPECT_TRUE(planeElem->Get<math::Vector2d>("size") ==
      math::Vector2d(0.5, 0.8));

  // image
  msgs::Geometry imageMsg;
  imageMsg.set_type(msgs::Geometry::IMAGE);
  msgs::ImageGeom *imageGeom = imageMsg.mutable_image();
  imageGeom->set_uri("test_uri");
  imageGeom->set_scale(1.8);
  imageGeom->set_threshold(255);
  imageGeom->set_height(1.3);
  imageGeom->set_granularity(2);

  sdf::ElementPtr imageSDF = msgs::GeometryToSDF(imageMsg);
  sdf::ElementPtr imageElem = imageSDF->GetElement("image");
  EXPECT_STREQ(imageElem->Get<std::string>("uri").c_str(), "test_uri");
  EXPECT_DOUBLE_EQ(imageElem->Get<double>("scale"), 1.8);
  EXPECT_DOUBLE_EQ(imageElem->Get<double>("threshold"), 255);
  EXPECT_DOUBLE_EQ(imageElem->Get<double>("height"), 1.3);
  EXPECT_DOUBLE_EQ(imageElem->Get<int>("granularity"), 2);

  // heightmap
  msgs::Geometry heightmapMsg;
  heightmapMsg.set_type(msgs::Geometry::HEIGHTMAP);
  msgs::HeightmapGeom *heightmapGeom = heightmapMsg.mutable_heightmap();
  heightmapGeom->set_filename("test_heightmap_filename");
  msgs::Set(heightmapGeom->mutable_size(), math::Vector3(100, 200, 30));
  msgs::Set(heightmapGeom->mutable_origin(), math::Vector3(50, 100, 15));
  heightmapGeom->set_use_terrain_paging(true);

  msgs::HeightmapGeom_Texture *texture1 = heightmapGeom->add_texture();
  texture1->set_diffuse("test_diffuse1");
  texture1->set_normal("test_normal1");
  texture1->set_size(10);

  msgs::HeightmapGeom_Texture *texture2 = heightmapGeom->add_texture();
  texture2->set_diffuse("test_diffuse2");
  texture2->set_normal("test_normal2");
  texture2->set_size(20);

  msgs::HeightmapGeom_Blend *blend = heightmapGeom->add_blend();
  blend->set_min_height(25);
  blend->set_fade_dist(5);

  sdf::ElementPtr heightmapSDF = msgs::GeometryToSDF(heightmapMsg);
  sdf::ElementPtr heightmapElem = heightmapSDF->GetElement("heightmap");
  EXPECT_STREQ(heightmapElem->Get<std::string>("uri").c_str(),
      "test_heightmap_filename");
  EXPECT_TRUE(heightmapElem->Get<math::Vector3>("size") ==
      math::Vector3(100, 200, 30));
  EXPECT_TRUE(heightmapElem->Get<math::Vector3>("pos") ==
      math::Vector3(50, 100, 15));
  EXPECT_TRUE(heightmapElem->Get<bool>("use_terrain_paging"));

  sdf::ElementPtr textureElem1 = heightmapElem->GetElement("texture");
  EXPECT_STREQ(textureElem1->Get<std::string>("diffuse").c_str(),
      "test_diffuse1");
  EXPECT_STREQ(textureElem1->Get<std::string>("normal").c_str(),
      "test_normal1");
  EXPECT_DOUBLE_EQ(textureElem1->Get<double>("size"), 10);
  sdf::ElementPtr textureElem2 = textureElem1->GetNextElement("texture");
  EXPECT_STREQ(textureElem2->Get<std::string>("diffuse").c_str(),
      "test_diffuse2");
  EXPECT_STREQ(textureElem2->Get<std::string>("normal").c_str(),
      "test_normal2");
  EXPECT_DOUBLE_EQ(textureElem2->Get<double>("size"), 20);

  sdf::ElementPtr blendElem = heightmapElem->GetElement("blend");
  EXPECT_DOUBLE_EQ(blendElem->Get<double>("min_height"), 25);
  EXPECT_DOUBLE_EQ(blendElem->Get<double>("fade_dist"), 5);

  // mesh
  msgs::Geometry meshMsg;
  meshMsg.set_type(msgs::Geometry::MESH);
  msgs::MeshGeom *meshGeom = meshMsg.mutable_mesh();
  meshGeom->set_filename("test_mesh_filename");
  msgs::Set(meshGeom->mutable_scale(), math::Vector3(2.3, 1.2, 2.9));
  meshGeom->set_submesh("test_mesh_submesh");
  meshGeom->set_center_submesh(false);

  sdf::ElementPtr meshSDF = msgs::GeometryToSDF(meshMsg);
  sdf::ElementPtr meshElem = meshSDF->GetElement("mesh");
  EXPECT_STREQ(meshElem->Get<std::string>("uri").c_str(),
      "test_mesh_filename");
  EXPECT_TRUE(meshElem->Get<math::Vector3>("scale") ==
      math::Vector3(2.3, 1.2, 2.9));
  sdf::ElementPtr submeshElem = meshElem->GetElement("submesh");
  EXPECT_STREQ(submeshElem->Get<std::string>("name").c_str(),
      "test_mesh_submesh");
  EXPECT_TRUE(!submeshElem->Get<bool>("center"));

  // polyline
  msgs::Geometry polylineMsg;
  polylineMsg.set_type(msgs::Geometry::POLYLINE);
  msgs::Polyline *polylineGeom = polylineMsg.mutable_polyline();
  polylineGeom->set_height(2.33);
  msgs::Set(polylineGeom->add_point(), math::Vector2d(0.5, 0.7));
  msgs::Set(polylineGeom->add_point(), math::Vector2d(3.5, 4.7));
  msgs::Set(polylineGeom->add_point(), math::Vector2d(1000, 2000));

  sdf::ElementPtr polylineSDF = msgs::GeometryToSDF(polylineMsg);
  sdf::ElementPtr polylineElem = polylineSDF->GetElement("polyline");
  EXPECT_DOUBLE_EQ(polylineElem->Get<double>("height"), 2.33);

  sdf::ElementPtr pointElem1 = polylineElem->GetElement("point");
  EXPECT_TRUE(pointElem1->Get<math::Vector2d>() == math::Vector2d(0.5, 0.7));
  sdf::ElementPtr pointElem2 = pointElem1->GetNextElement("point");
  EXPECT_TRUE(pointElem2->Get<math::Vector2d>() == math::Vector2d(3.5, 4.7));
  sdf::ElementPtr pointElem3 = pointElem2->GetNextElement("point");
  EXPECT_TRUE(pointElem3->Get<math::Vector2d>() == math::Vector2d(1000, 2000));
}

/////////////////////////////////////////////////
TEST_F(MsgsTest, MeshToSDF)
{
  msgs::MeshGeom msg;
  msg.set_filename("test_filename");
  msgs::Set(msg.mutable_scale(), math::Vector3(0.1, 0.2, 0.3));
  msg.set_submesh("test_submesh");
  msg.set_center_submesh(true);

  sdf::ElementPtr meshSDF = msgs::MeshToSDF(msg);

  EXPECT_STREQ(meshSDF->Get<std::string>("uri").c_str(), "test_filename");
  math::Vector3 scale = meshSDF->Get<math::Vector3>("scale");
  EXPECT_DOUBLE_EQ(scale.x, 0.1);
  EXPECT_DOUBLE_EQ(scale.y, 0.2);
  EXPECT_DOUBLE_EQ(scale.z, 0.3);

  sdf::ElementPtr submeshElem = meshSDF->GetElement("submesh");
  EXPECT_STREQ(submeshElem->Get<std::string>("name").c_str(), "test_submesh");
  EXPECT_TRUE(submeshElem->Get<bool>("center"));
}

/////////////////////////////////////////////////
TEST_F(MsgsTest, InertialToSDF)
{
  msgs::Inertial msg;
  msg.set_mass(3.4);
  msgs::Set(msg.mutable_pose(), math::Pose(math::Vector3(1.2, 3.4, 5.6),
      math::Quaternion(0.7071, 0.0, 0.7071, 0.0)));
  msg.set_ixx(0.0133);
  msg.set_ixy(-0.0003);
  msg.set_ixz(-0.0004);
  msg.set_iyy(0.0116);
  msg.set_iyz(0.0008);
  msg.set_izz(0.0038);

  sdf::ElementPtr inertialSDF = msgs::InertialToSDF(msg);

  EXPECT_DOUBLE_EQ(inertialSDF->Get<double>("mass"), 3.4);

  EXPECT_TRUE(inertialSDF->Get<math::Pose>("pose") ==
      math::Pose(math::Vector3(1.2, 3.4, 5.6),
      math::Quaternion(0.7071, 0.0, 0.7071, 0.0)));

  // inertia
  sdf::ElementPtr inertiaElem = inertialSDF->GetElement("inertia");
  EXPECT_DOUBLE_EQ(inertiaElem->Get<double>("ixx"), 0.0133);
  EXPECT_DOUBLE_EQ(inertiaElem->Get<double>("ixy"), -0.0003);
  EXPECT_DOUBLE_EQ(inertiaElem->Get<double>("ixz"), -0.0004);
  EXPECT_DOUBLE_EQ(inertiaElem->Get<double>("iyy"), 0.0116);
  EXPECT_DOUBLE_EQ(inertiaElem->Get<double>("iyz"), 0.0008);
  EXPECT_DOUBLE_EQ(inertiaElem->Get<double>("izz"), 0.0038);
}

/////////////////////////////////////////////////
TEST_F(MsgsTest, SurfaceToSDF)
{
  msgs::Surface msg;

  // friction
  msgs::Friction *friction = msg.mutable_friction();
  friction->set_mu(0.1);
  friction->set_mu2(0.2);
  msgs::Set(friction->mutable_fdir1(), math::Vector3(0.3, 0.4, 0.5));
  friction->set_slip1(0.6);
  friction->set_slip2(0.7);

  // bounce
  msg.set_restitution_coefficient(1.1);
  msg.set_bounce_threshold(1000);

  // other ode surface properties
  msg.set_soft_cfm(0.9);
  msg.set_soft_erp(0.3);
  msg.set_kp(0.4);
  msg.set_kd(0.8);
  msg.set_max_vel(3.8);
  msg.set_min_depth(0.0001);
  msg.set_collide_without_contact(true);
  msg.set_collide_without_contact_bitmask(0x0004);

  sdf::ElementPtr surfaceSDF = msgs::SurfaceToSDF(msg);
  sdf::ElementPtr frictionElem = surfaceSDF->GetElement("friction");
  sdf::ElementPtr frictionPhysicsElem = frictionElem->GetElement("ode");
  EXPECT_DOUBLE_EQ(frictionPhysicsElem->Get<double>("mu"), 0.1);
  EXPECT_DOUBLE_EQ(frictionPhysicsElem->Get<double>("mu2"), 0.2);
  EXPECT_TRUE(frictionPhysicsElem->Get<math::Vector3>("fdir1") ==
      math::Vector3(0.3, 0.4, 0.5));
  EXPECT_DOUBLE_EQ(frictionPhysicsElem->Get<double>("slip1"), 0.6);
  EXPECT_DOUBLE_EQ(frictionPhysicsElem->Get<double>("slip2"), 0.7);

  sdf::ElementPtr bounceElem = surfaceSDF->GetElement("bounce");
  EXPECT_DOUBLE_EQ(bounceElem->Get<double>("restitution_coefficient"), 1.1);
  EXPECT_DOUBLE_EQ(bounceElem->Get<double>("threshold"), 1000);

  sdf::ElementPtr contactElem = surfaceSDF->GetElement("contact");
  sdf::ElementPtr contactPhysicsElem = contactElem->GetElement("ode");
  EXPECT_DOUBLE_EQ(contactPhysicsElem->Get<double>("soft_cfm"), 0.9);
  EXPECT_DOUBLE_EQ(contactPhysicsElem->Get<double>("soft_erp"), 0.3);
  EXPECT_DOUBLE_EQ(contactPhysicsElem->Get<double>("kp"), 0.4);
  EXPECT_DOUBLE_EQ(contactPhysicsElem->Get<double>("kd"), 0.8);
  EXPECT_DOUBLE_EQ(contactPhysicsElem->Get<double>("max_vel"), 3.8);
  EXPECT_DOUBLE_EQ(contactPhysicsElem->Get<double>("min_depth"), 0.0001);

  EXPECT_TRUE(contactElem->Get<bool>("collide_without_contact"));
  EXPECT_EQ(contactElem->Get<unsigned int>("collide_without_contact_bitmask"),
      static_cast<unsigned int>(0x0004));
}
