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
#include <list>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <string>

#include "transport/Publisher.hh"
#include "transport/Subscriber.hh"
#include "transport/ConnectionManager.hh"
#include "Transport.hh"

using namespace gazebo;

boost::thread *g_runThread = NULL;

/// Get the hostname and port of the master from the GAZEBO_MASTER_URI
/// environment variable
bool transport::get_master_uri(std::string &master_host, 
                               unsigned short &master_port)
{
  char *char_uri = getenv("GAZEBO_MASTER_URI");

  // Set to default host and port
  if (!char_uri)
  {
    master_host = "localhost";
    master_port = 11345;
    return false;
  }

  std::string master_uri = char_uri;

  boost::replace_first(master_uri, "http://", "");
  int last_colon = master_uri.find_last_of(":");
  master_host = master_uri.substr(0,last_colon);
  master_port = boost::lexical_cast<unsigned short>( master_uri.substr(last_colon+1, master_uri.size() - (last_colon+1)) );

  return true;
}


void transport::init(const std::string &master_host, unsigned short master_port)
{
  std::string host = master_host;
  unsigned short port = master_port;

  if (host.empty())
    get_master_uri(host, port);

  transport::TopicManager::Instance()->Init();
  transport::ConnectionManager::Instance()->Init( host, port );
}

void transport::run()
{
  g_runThread = new boost::thread(&transport::ConnectionManager::Run,
                                transport::ConnectionManager::Instance());
}

void transport::stop()
{
  /*transport::ConnectionManager::Instance()->Stop();

  if (runThread)
  {
    runThread->join();
    delete runThread;
    runThread = NULL;
  }
  */
}

void transport::fini()
{
  transport::TopicManager::Instance()->Fini();
  transport::ConnectionManager::Instance()->Stop();

  if (g_runThread)
  {
    g_runThread->join();
    delete g_runThread;
    g_runThread = NULL;
  }
  transport::ConnectionManager::Instance()->Fini();
}
