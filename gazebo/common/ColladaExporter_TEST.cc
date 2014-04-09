/*
 * Copyright (C) 2012-2014 Open Source Robotics Foundation
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

#include "test_config.h"
#include "gazebo/common/Mesh.hh"
#include "gazebo/common/ColladaLoader.hh"
#include "gazebo/common/ColladaExporter.hh"
#include "test/util.hh"

using namespace gazebo;

class ColladaExporter : public gazebo::testing::AutoLogFixture { };

/////////////////////////////////////////////////
TEST_F(ColladaExporter, ExportBox)
{
  common::ColladaLoader loader;
  const common::Mesh *meshOriginal = loader.Load(
      std::string(PROJECT_SOURCE_PATH) + "/test/data/box.dae");

  common::ColladaExporter exporter;
  exporter.Export(meshOriginal, std::string(PROJECT_SOURCE_PATH) + "/test/data/exported.dae");

  const common::Mesh *meshReloaded = loader.Load(
      std::string(PROJECT_SOURCE_PATH) + "/test/data/exported.dae");

  EXPECT_EQ(meshOriginal->GetName(), meshReloaded->GetName());
  EXPECT_EQ(meshOriginal->GetMax(), meshReloaded->GetMax());
  EXPECT_EQ(meshOriginal->GetMin(), meshReloaded->GetMin());
  EXPECT_EQ(meshOriginal->GetSubMeshCount(), meshReloaded->GetSubMeshCount());
  EXPECT_EQ(meshOriginal->GetMaterialCount(), meshReloaded->GetMaterialCount());
  EXPECT_EQ(meshOriginal->GetVertexCount(), meshReloaded->GetVertexCount());
  EXPECT_EQ(meshOriginal->GetNormalCount(), meshReloaded->GetNormalCount());
  EXPECT_EQ(meshOriginal->GetTexCoordCount(), meshReloaded->GetTexCoordCount());
  EXPECT_EQ(meshOriginal->GetIndexCount(), meshReloaded->GetIndexCount());
}

/////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
