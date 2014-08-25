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
#include "gazebo/common/Exception.hh"
#include "gazebo/util/LogRecord.hh"
#include "gazebo/common/Console.hh"
#include "gazebo/Server.hh"

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
  gazebo::Server *server = NULL;

  try
  {
    // Initialize the data logger. This will log state information.
    gazebo::util::LogRecord::Instance()->Init("gzserver");

    server = new gazebo::Server();
    if (!server->ParseArgs(argc, argv))
      return -1;

    // Initialize the informational logger. This will log warnings, and errors.
    gazebo::common::StrStr_M params = server->GetParams();
    if (params.find("server-logfile") != params.end())
      gzLogInit(params["server-logfile"]);
    else
      gzLogInit("gzserver.log");

    server->Run();
    server->Fini();

    delete server;
  }
  catch(gazebo::common::Exception &_e)
  {
    _e.Print();

    server->Fini();
    delete server;
    return -1;
  }

  return 0;
}
