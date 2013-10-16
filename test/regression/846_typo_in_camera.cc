/*
 * Copyright 2013 Open Source Robotics Foundation
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
#include <gazebo/rendering/rendering.hh>

/////////////////////////////////////////////////
// \brief Test for issue #846
TEST(issue_846_typo_in_camera, CaptureData)
{
  gazebo::rendering::CameraPtr camera(new gazebo::rendering::Camera("",
        gazebo::rendering::ScenePtr(), false));

  EXPECT_FALSE(camera->GetCaptureData());
  camera->EnableSaveFrame(true);
  EXPECT_TRUE(camera->GetCaptureData());
  camera->EnableSaveFrame(false);
  EXPECT_FALSE(camera->GetCaptureData());
}

/////////////////////////////////////////////////
/// Main
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
