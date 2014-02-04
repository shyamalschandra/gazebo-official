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

#include <boost/filesystem.hpp>

#include "ServerFixture.hh"
#include "gazebo/common/common.hh"

using namespace gazebo;
class FileHandling : public ServerFixture
{
};

TEST_F(FileHandling, Save)
{
  // Get a path suitable for temporary files
  boost::system::error_code ec;
  boost::filesystem::path tmpDir = boost::filesystem::temp_directory_path(ec);
  if (ec != 0)
  {
    gzerr << "Failed creating temp directory. Reason: " << ec.message() << "\n";
    FAIL();
  }

  // Cleanup test directory.
  boost::filesystem::remove_all(tmpDir / "gazebo_test");
  boost::filesystem::create_directories(tmpDir / "gazebo_test");

  Load("worlds/empty.world");

  transport::PublisherPtr serverControlPub =
    node->Advertise<msgs::ServerControl>("/gazebo/server/control");

  transport::PublisherPtr requestPub =
    node->Advertise<msgs::Request>("~/request");


  // Find a valid filename
  FILE *file = NULL;
  std::ostringstream filename;
  int i = 0;
  do
  {
    filename.str("");
    filename << (tmpDir / "gazebo_test").string() + "/test_" << i << ".world";
    i++;
  } while ((file = fopen(filename.str().c_str(), "r")) != NULL);

  msgs::ServerControl msg;
  msg.set_save_world_name("default");
  msg.set_save_filename(filename.str());
  serverControlPub->Publish(msg);

  // Wait until the file exists
  i = 0;
  while (i < 10 && (file = fopen(filename.str().c_str(), "r")) == NULL)
  {
    i++;
    common::Time::MSleep(100);
  }
  fclose(file);

  EXPECT_LT(i, 10);

  // Cleanup test directory.
  boost::filesystem::remove_all(tmpDir / "gazebo_test");
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
