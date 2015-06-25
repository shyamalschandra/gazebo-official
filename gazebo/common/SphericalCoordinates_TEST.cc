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

#include "gazebo/common/Console.hh"
#include "gazebo/common/SphericalCoordinates.hh"
#include "test/util.hh"

using namespace gazebo;

class SphericalCoordinatesTest : public gazebo::testing::AutoLogFixture { };

//////////////////////////////////////////////////
// Test different constructors, default parameters
TEST_F(SphericalCoordinatesTest, Constructor)
{
  // Default surface type
  common::SphericalCoordinates::SurfaceType st =
    common::SphericalCoordinates::EARTH_WGS84;

  // No arguments, default parameters
  {
    common::SphericalCoordinates sc;
    EXPECT_EQ(sc.GetSurfaceType(), st);
    EXPECT_EQ(sc.GetLatitudeReference(), math::Angle());
    EXPECT_EQ(sc.GetLongitudeReference(), math::Angle());
    EXPECT_EQ(sc.GetHeadingOffset(), math::Angle());
    EXPECT_NEAR(sc.GetElevationReference(), 0.0, 1e-6);
  }

  // SurfaceType argument, default parameters
  {
    common::SphericalCoordinates sc(st);
    EXPECT_EQ(sc.GetSurfaceType(), st);
    EXPECT_EQ(sc.GetLatitudeReference(), math::Angle());
    EXPECT_EQ(sc.GetLongitudeReference(), math::Angle());
    EXPECT_EQ(sc.GetHeadingOffset(), math::Angle());
    EXPECT_NEAR(sc.GetElevationReference(), 0.0, 1e-6);
  }

  // All arguments
  {
    math::Angle lat(0.3), lon(-1.2), heading(0.5);
    double elev = 354.1;
    common::SphericalCoordinates sc(st, lat, lon, elev, heading);
    EXPECT_EQ(sc.GetSurfaceType(), st);
    EXPECT_EQ(sc.GetLatitudeReference(), lat);
    EXPECT_EQ(sc.GetLongitudeReference(), lon);
    EXPECT_EQ(sc.GetHeadingOffset(), heading);
    EXPECT_NEAR(sc.GetElevationReference(), elev, 1e-6);
  }
}

//////////////////////////////////////////////////
// SurfaceType Convert function
TEST_F(SphericalCoordinatesTest, Convert)
{
  // Default surface type
  common::SphericalCoordinates::SurfaceType st =
    common::SphericalCoordinates::EARTH_WGS84;

  EXPECT_EQ(common::SphericalCoordinates::Convert("EARTH_WGS84"), st);
}

//////////////////////////////////////////////////
// Test Set functions
TEST_F(SphericalCoordinatesTest, SetFunctions)
{
  // Default surface type
  common::SphericalCoordinates::SurfaceType st =
    common::SphericalCoordinates::EARTH_WGS84;

  // Default parameters
  common::SphericalCoordinates sc;
  EXPECT_EQ(sc.GetSurfaceType(), st);
  EXPECT_EQ(sc.GetLatitudeReference(), math::Angle());
  EXPECT_EQ(sc.GetLongitudeReference(), math::Angle());
  EXPECT_EQ(sc.GetHeadingOffset(), math::Angle());
  EXPECT_NEAR(sc.GetElevationReference(), 0.0, 1e-6);

  {
    math::Angle lat(0.3), lon(-1.2), heading(0.5);
    double elev = 354.1;
    sc.SetSurfaceType(st);
    sc.SetLatitudeReference(lat);
    sc.SetLongitudeReference(lon);
    sc.SetHeadingOffset(heading);
    sc.SetElevationReference(elev);

    EXPECT_EQ(sc.GetSurfaceType(), st);
    EXPECT_EQ(sc.GetLatitudeReference(), lat);
    EXPECT_EQ(sc.GetLongitudeReference(), lon);
    EXPECT_EQ(sc.GetHeadingOffset(), heading);
    EXPECT_NEAR(sc.GetElevationReference(), elev, 1e-6);
  }
}

//////////////////////////////////////////////////
// Test coordinate transformations
TEST_F(SphericalCoordinatesTest, CoordinateTransforms)
{
  // Default surface type
  common::SphericalCoordinates::SurfaceType st =
    common::SphericalCoordinates::EARTH_WGS84;

  // Check velocity projection
  {
    // Parameters
    math::Angle lat(0.3), lon(-1.2), heading(math::Angle::HalfPi);
    double elev = 354.1;
    common::SphericalCoordinates sc(st, lat, lon, elev, heading);

    math::Vector3 xyz;
    math::Vector3 enu;

    xyz.Set(1, 0, 0);
    enu = sc.GlobalFromLocal(xyz);
    EXPECT_NEAR(enu.y, xyz.x, 1e-6);
    EXPECT_NEAR(enu.x, -xyz.y, 1e-6);

    xyz.Set(0, 1, 0);
    enu = sc.GlobalFromLocal(xyz);
    EXPECT_NEAR(enu.y, xyz.x, 1e-6);
    EXPECT_NEAR(enu.x, -xyz.y, 1e-6);

    xyz.Set(1, -1, 0);
    enu = sc.GlobalFromLocal(xyz);
    EXPECT_NEAR(enu.y, xyz.x, 1e-6);
    EXPECT_NEAR(enu.x, -xyz.y, 1e-6);

    xyz.Set(2243.52334, 556.35, 435.6553);
    enu = sc.GlobalFromLocal(xyz);
    EXPECT_NEAR(enu.y, xyz.x, 1e-6);
    EXPECT_NEAR(enu.x, -xyz.y, 1e-6);
  }

  // Check position projection
  {
    // GROUND TRUTH TEST POINTS ///////////////////////////////////////////////
    // WGS84 coordinate obtained from online mapping software
    // > gdaltransform -s_srs WGS84 -t_srs EPSG:4978
    // > latitude longitude altitude
    // > X Y Z
    math::Vector3 tmp;
    math::Vector3 osrf_s(37.3877349,-122.0651166,32.0);
    math::Vector3 osrf_e(-2693701.91434394,-4299942.14687992,3851691.0393571);
    math::Vector3 goog_s(37.4216719,-122.0821853,30.0);
    math::Vector3 goog_e(-2693766.71906146,-4297199.59926038,3854681.81878812);

    // Local tangent plane coordinates (ENU = GLOBAL) coordinates of Google when
    // OSRF is taken as the origin:
    // > proj +ellps=WGS84  +proj=tmerc +lat_0=37.3877349 +lon_0=-122.0651166 +k=1 +x_0=0 +y_0=0
    // > -122.0821853 37.4216719 (LON,LAT)
    // > -1510.88 3766.64 (EAST,NORTH)
    math::Vector3 vec(-1510.88,3766.64,-3.29);

    // Convert degrees to radians
    osrf_s.x *= 0.0174532925;
    osrf_s.y *= 0.0174532925;
    goog_s.x *= 0.0174532925;
    goog_s.y *= 0.0174532925;

    // Set the ORIGIN to be the Open Source Robotics Foundation
    common::SphericalCoordinates sc(st, math::Angle(osrf_s.x), 
      math::Angle(osrf_s.y), osrf_s.z, 0.0);

    // Check that SPHERICAL -> ECEF works (to 1e2 of a meter)
    tmp = sc.PositionTransform(osrf_s,common::SphericalCoordinates::SPHERICAL,
      common::SphericalCoordinates::ECEF);
    EXPECT_NEAR(tmp.x, osrf_e.x, 1e-1);
    EXPECT_NEAR(tmp.y, osrf_e.y, 1e-1);
    EXPECT_NEAR(tmp.z, osrf_e.z, 1e-1);

    // Check that ECEF -> SPHERICAL works (to 1e-6 of a degree)
    tmp = sc.PositionTransform(tmp,common::SphericalCoordinates::ECEF,
      common::SphericalCoordinates::SPHERICAL);
    EXPECT_NEAR(tmp.x, osrf_s.x, 1e-2);
    EXPECT_NEAR(tmp.y, osrf_s.y, 1e-2);
    EXPECT_NEAR(tmp.z, osrf_s.z, 1e-2);

    // Check that WGS84 -> GLOBAL works (to 1e-2 of a meter)
    tmp = sc.LocalFromSpherical(goog_s);
    EXPECT_NEAR(tmp.x, vec.x, 1e-1);
    EXPECT_NEAR(tmp.y, vec.y, 1e-1);
    EXPECT_NEAR(tmp.z, vec.z, 1e-1);

    // Check that WGS84 -> GLOBAL works (to 1e-2 of a meter)
    tmp = sc.SphericalFromLocal(tmp);
    EXPECT_NEAR(tmp.x, goog_s.x, 1e-2);
    EXPECT_NEAR(tmp.y, goog_s.y, 1e-2);
    EXPECT_NEAR(tmp.z, goog_s.z, 1e-2);
  }
}

//////////////////////////////////////////////////
// Test distance
TEST_F(SphericalCoordinatesTest, Distance)
{
  math::Angle latA, longA, latB, longB;
  latA.SetFromDegree(46.250944);
  longA.SetFromDegree(-122.249972);
  latB.SetFromDegree(46.124953);
  longB.SetFromDegree(-122.251683);
  double d = common::SphericalCoordinates::Distance(latA, longA, latB, longB);
  EXPECT_NEAR(14002, d, 20);
}

/////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
