/*
 * Copyright 2012 Open Source Robotics Foundation
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

#include "gazebo/msgs/msgs.hh"
#include "gazebo/common/Console.hh"
#include "gazebo/common/Events.hh"
#include "gazebo/transport/TopicManager.hh"
#include "gazebo/transport/ConnectionManager.hh"

#include "gazebo_config.h"

using namespace gazebo;
using namespace transport;

/// TBB task to process nodes.
class TopicManagerProcessTask : public tbb::task
{
  /// Implements the necessary execute function
  public: tbb::task *execute()
          {
            TopicManager::Instance()->ProcessNodes();
            return NULL;
          }
};

/// TBB task to establish subscriber to publisher connection.
class TopicManagerConnectionTask : public tbb::task
{
  /// \brief Constructor.
  /// \param[in] _pub Publish message
  public: TopicManagerConnectionTask(msgs::Publish _pub) : pub(_pub) {}

  /// Implements the necessary execute function
  public: tbb::task *execute()
          {
            TopicManager::Instance()->ConnectSubToPub(pub);
            return NULL;
          }

  /// \brief Publish message
  private: msgs::Publish pub;
};

//////////////////////////////////////////////////
ConnectionManager::ConnectionManager()
{
  this->tmpIndex = 0;
  this->initialized = false;
  this->stop = false;
  this->pause = false;
  this->stopped = true;

  this->serverConn = NULL;

  this->eventConnections.push_back(
      event::Events::ConnectStop(boost::bind(&ConnectionManager::Stop, this)));
}

//////////////////////////////////////////////////
ConnectionManager::~ConnectionManager()
{
  this->eventConnections.clear();

  delete this->serverConn;
  this->serverConn = NULL;
  this->Fini();
}

//////////////////////////////////////////////////
bool ConnectionManager::Init(const std::string &_masterHost,
                             unsigned int master_port)
{
  if (this->initialized)
    return true;

  this->stop = false;
  this->masterConn.reset(new Connection());
  delete this->serverConn;
  this->serverConn = new Connection();

  // Create a new TCP server on a free port
  this->serverConn->Listen(0,
      boost::bind(&ConnectionManager::OnAccept, this, _1));

  gzmsg << "Waiting for master";
  uint32_t timeoutCount = 0;
  uint32_t waitDurationMS = 1000;
  uint32_t timeoutCountMax = 30;

  while (!this->masterConn->Connect(_masterHost, master_port) &&
      this->IsRunning() && timeoutCount < timeoutCountMax)
  {
    if (!common::Console::Instance()->GetQuiet())
    {
      printf(".");
      fflush(stdout);
    }
    common::Time::MSleep(waitDurationMS);
    ++timeoutCount;
  }
  if (!common::Console::Instance()->GetQuiet())
    printf("\n");

  if (timeoutCount >= timeoutCountMax)
  {
    gzerr << "Failed to connect to master in "
          << (timeoutCount * waitDurationMS) / 1000.0 << " seconds.\n";
    return false;
  }

  if (!this->IsRunning())
  {
    gzerr << "Connection Manager is not running\n";
    return false;
  }

  std::string initData, namespacesData, publishersData;

  try
  {
    this->masterConn->Read(initData);
    this->masterConn->Read(namespacesData);
    this->masterConn->Read(publishersData);
  }
  catch(...)
  {
    gzerr << "Unable to read from master\n";
    return false;
  }

  msgs::Packet packet;
  packet.ParseFromString(initData);

  if (packet.type() == "version_init")
  {
    msgs::GzString msg;
    msg.ParseFromString(packet.serialized_data());
    if (msg.data() == std::string("gazebo ") + GAZEBO_VERSION)
    {
      // TODO: set some flag.. maybe start "serverConn" when initialized
      gzmsg << "Connected to gazebo master @ "
            << this->masterConn->GetRemoteURI() << "\n";
    }
    else
    {
      // TODO: MAke this a proper error
      gzerr << "Conflicting gazebo versions\n";
    }
  }
  else
    gzerr << "Didn't receive an init from the master\n";

  packet.ParseFromString(namespacesData);
  if (packet.type() == "topic_namepaces_init")
  {
    msgs::GzString_V result;
    result.ParseFromString(packet.serialized_data());
    boost::mutex::scoped_lock lock(this->namespaceMutex);

    for (int i = 0; i < result.data_size(); i++)
    {
      this->namespaces.push_back(std::string(result.data(i)));
    }
    this->namespaceCondition.notify_all();
  }
  else
    gzerr << "Did not get topic_namespaces_init msg from master\n";

  packet.ParseFromString(publishersData);
  if (packet.type() == "publishers_init")
  {
    msgs::Publishers pubs;
    pubs.ParseFromString(packet.serialized_data());

    boost::recursive_mutex::scoped_lock lock(this->listMutex);
    for (int i = 0; i < pubs.publisher_size(); i++)
    {
      const msgs::Publish &p = pubs.publisher(i);
      this->publishers.push_back(p);
    }
  }
  else
    gzerr << "Did not get publishers_init msg from master\n";

  this->masterConn->AsyncRead(
      boost::bind(&ConnectionManager::OnMasterRead, this, _1));

  this->initialized = true;

  // Tell the user what address will be publicized to other nodes.
  gzmsg << "Publicized address: "
        << this->masterConn->GetLocalHostname() << "\n";

  return true;
}

//////////////////////////////////////////////////
void ConnectionManager::Fini()
{
  if (!this->initialized)
    return;

  this->Stop();

  if (this->masterConn)
  {
    this->masterConn->ProcessWriteQueue();
    this->masterConn->Shutdown();
    this->masterConn.reset();
  }

  if (this->serverConn)
  {
    this->serverConn->ProcessWriteQueue();
    this->serverConn->Shutdown();
    delete this->serverConn;
    this->serverConn = NULL;
  }

  this->eventConnections.clear();
  this->connections.clear();
  this->publishers.clear();
  this->namespaces.clear();
  this->masterMessages.clear();

  this->initialized = false;
}

//////////////////////////////////////////////////
void ConnectionManager::Pause(bool _pause)
{
  if (this->pause && !_pause)
  {
    this->pause = _pause;
    boost::mutex::scoped_lock lock(this->updateMutex);
    this->updateCondition.notify_all();
  }
  else
    this->pause = _pause;
}

//////////////////////////////////////////////////
void ConnectionManager::Stop()
{
  this->stop = true;
  this->updateCondition.notify_all();
  if (this->initialized)
    while (this->stopped == false)
      common::Time::MSleep(100);
}

//////////////////////////////////////////////////
void ConnectionManager::RunUpdate()
{
  std::list<ConnectionPtr>::iterator iter;
  std::list<ConnectionPtr>::iterator endIter;

  unsigned int msize = 0;
  {
    boost::recursive_mutex::scoped_lock lock(this->masterMessagesMutex);
    msize = this->masterMessages.size();
  }

  while (msize > 0)
  {
    this->ProcessMessage(this->masterMessages.front());
    {
      boost::recursive_mutex::scoped_lock lock(this->masterMessagesMutex);
      this->masterMessages.pop_front();
      msize = this->masterMessages.size();
    }
  }

  if (this->masterConn)
    this->masterConn->ProcessWriteQueue();

  // Use TBB to process nodes. Need more testing to see if this makes
  // a difference.
  // TopicManagerProcessTask *task = new(tbb::task::allocate_root())
  //   TopicManagerProcessTask();
  // tbb::task::enqueue(*task);
  TopicManager::Instance()->ProcessNodes();
  {
    boost::recursive_mutex::scoped_lock lock(this->connectionMutex);
    iter = this->connections.begin();
    endIter = this->connections.end();
  }

  while (iter != endIter)
  {
    if ((*iter)->IsOpen())
    {
      (*iter)->ProcessWriteQueue();
      ++iter;
    }
    else
    {
      boost::recursive_mutex::scoped_lock lock(this->connectionMutex);
      iter = this->connections.erase(iter);
    }
  }
}

//////////////////////////////////////////////////
void ConnectionManager::Run()
{
  boost::mutex::scoped_lock lock(this->updateMutex);

  this->stopped = false;

  // Process all pending subscriptions.
  for (std::list<PendingSubscription>::iterator iter =
      this->pendingSubscriptions.begin();
      iter != this->pendingSubscriptions.end(); ++iter)
  {
    this->Subscribe((*iter).topic, (*iter).msgType, (*iter).latching);
  }

  // Process all pending advertisements.
  for (std::list<PendingAdvertisement>::iterator iter =
      this->pendingAdvertisements.begin();
      iter != this->pendingAdvertisements.end(); ++iter)
  {
    this->Advertise((*iter).topic, (*iter).msgType);
  }

  while (!this->stop && this->masterConn && this->masterConn->IsOpen())
  {
    if (!this->pause)
    {
      this->RunUpdate();
      this->updateCondition.timed_wait(lock,
          boost::posix_time::milliseconds(100));
    }
    else
      this->updateCondition.wait(lock);
  }
  this->RunUpdate();

  this->stopped = true;

  this->masterConn->Shutdown();
}

//////////////////////////////////////////////////
bool ConnectionManager::IsRunning() const
{
  return !this->stop;
}

//////////////////////////////////////////////////
void ConnectionManager::OnMasterRead(const std::string &_data)
{
  if (this->masterConn && this->masterConn->IsOpen())
    this->masterConn->AsyncRead(
        boost::bind(&ConnectionManager::OnMasterRead, this, _1));

  if (!_data.empty())
  {
    boost::recursive_mutex::scoped_lock lock(this->masterMessagesMutex);
    this->masterMessages.push_back(std::string(_data));
  }
  else
    gzerr << "ConnectionManager::OnMasterRead empty data\n";

  // Tell the ourself that we need an update
  this->TriggerUpdate();
}

/////////////////////////////////////////////////
void ConnectionManager::ProcessMessage(const std::string &_data)
{
  msgs::Packet packet;
  packet.ParseFromString(_data);

  if (packet.type() == "publisher_add")
  {
    msgs::Publish result;
    result.ParseFromString(packet.serialized_data());
    this->publishers.push_back(result);
  }
  else if (packet.type() == "publisher_del")
  {
    msgs::Publish result;
    result.ParseFromString(packet.serialized_data());

    std::list<msgs::Publish>::iterator iter = this->publishers.begin();
    while (iter != this->publishers.end())
    {
      if ((*iter).topic() == result.topic() &&
          (*iter).host() == result.host() &&
          (*iter).port() == result.port())
        iter = this->publishers.erase(iter);
      else
        ++iter;
    }
  }
  else if (packet.type() == "topic_namespace_add")
  {
    msgs::GzString result;
    result.ParseFromString(packet.serialized_data());

    boost::mutex::scoped_lock lock(this->namespaceMutex);
    this->namespaces.push_back(std::string(result.data()));
    this->namespaceCondition.notify_all();
  }
  // FIXME "publisher_update" is currently not used and have been separated out
  // into "publisher_subscribe" and "publisher_advertise". This is implemented
  // as a workaround to address transport blocking issue when gzclient connects
  // to gzserver, see issue #714. "publisher_advertise", intended
  // for gzserver when gzclient connects, is parallelized and made non-blocking.
  else if (packet.type() == "publisher_update")
  {
    msgs::Publish pub;
    pub.ParseFromString(packet.serialized_data());
    if (pub.host() != this->serverConn->GetLocalAddress() ||
        pub.port() != this->serverConn->GetLocalPort())
    {
      TopicManager::Instance()->ConnectSubToPub(pub);
    }
  }
  else if (packet.type() == "publisher_advertise")
  {
    msgs::Publish pub;
    pub.ParseFromString(packet.serialized_data());
    if (pub.host() != this->serverConn->GetLocalAddress() ||
        pub.port() != this->serverConn->GetLocalPort())
    {
      TopicManagerConnectionTask *task = new(tbb::task::allocate_root())
      TopicManagerConnectionTask(pub);
      tbb::task::enqueue(*task);
    }
  }
  // publisher_subscribe. This occurs when we try to subscribe to a topic, and
  // the master informs us of a remote host that is publishing on our
  // requested topic
  else if (packet.type() == "publisher_subscribe")
  {
    msgs::Publish pub;
    pub.ParseFromString(packet.serialized_data());
    if (pub.host() != this->serverConn->GetLocalAddress() ||
        pub.port() != this->serverConn->GetLocalPort())
    {
      TopicManager::Instance()->ConnectSubToPub(pub);
    }
  }
  else if (packet.type() == "unsubscribe")
  {
    msgs::Subscribe sub;
    sub.ParseFromString(packet.serialized_data());

    // Disconnect a local publisher from a remote subscriber
    TopicManager::Instance()->DisconnectPubFromSub(sub.topic(),
        sub.host(), sub.port());
  }
  else if (packet.type() == "unadvertise")
  {
    msgs::Publish pub;
    pub.ParseFromString(packet.serialized_data());

    // Disconnection all local subscribers from a remote publisher
    TopicManager::Instance()->DisconnectSubFromPub(pub.topic(),
        pub.host(), pub.port());
  }
  else
  {
    gzerr << "ConnectionManager::OnMasterRead unknown type["
          << packet.type() << "][" << packet.serialized_data()
          << "] Data[" << _data << "]\n";
  }
}

//////////////////////////////////////////////////
void ConnectionManager::OnAccept(const ConnectionPtr &_newConnection)
{
  _newConnection->AsyncRead(
      boost::bind(&ConnectionManager::OnRead, this, _newConnection, _1));

  // Add the connection to the list of connections
  boost::recursive_mutex::scoped_lock lock(this->connectionMutex);
  this->connections.push_back(_newConnection);
}

//////////////////////////////////////////////////
void ConnectionManager::OnRead(const ConnectionPtr &_connection,
                               const std::string &_data)
{
  if (_data.empty())
  {
    gzerr << "Data was empty, try again\n";
    _connection->AsyncRead(
        boost::bind(&ConnectionManager::OnRead, this, _connection, _1));
    return;
  }

  msgs::Packet packet;
  packet.ParseFromString(_data);

  // If we have an incoming (remote) subscription
  if (packet.type() == "sub")
  {
    msgs::Subscribe sub;
    sub.ParseFromString(packet.serialized_data());

    // Create a transport link for the publisher to the remote subscriber
    // via the connection
    SubscriptionTransportPtr subLink(new SubscriptionTransport());
    subLink->Init(_connection, sub.latching());

    // Connect the publisher to this transport mechanism
    TopicManager::Instance()->ConnectPubToSub(sub, subLink);
  }
}

//////////////////////////////////////////////////
void ConnectionManager::Advertise(const std::string &_topic,
                                  const std::string &_msgType)
{
  // We are not initialized, so add the advertisement to our pending list.
  if (!this->initialized)
  {
    this->pendingAdvertisements.push_back(
        PendingAdvertisement(_topic, _msgType));
    return;
  }

  msgs::Publish msg;
  msg.set_topic(_topic);
  msg.set_msg_type(_msgType);
  msg.set_host(this->serverConn->GetLocalAddress());
  msg.set_port(this->serverConn->GetLocalPort());

  this->masterConn->EnqueueMsg(msgs::Package("advertise", msg));
}

//////////////////////////////////////////////////
void ConnectionManager::RegisterTopicNamespace(const std::string &_name)
{
  if (!this->initialized)
    return;

  msgs::GzString msg;
  msg.set_data(_name);
  this->masterConn->EnqueueMsg(msgs::Package("register_topic_namespace", msg));
}

//////////////////////////////////////////////////
void ConnectionManager::Unadvertise(const std::string &_topic)
{
  msgs::Publish msg;
  msg.set_topic(_topic);
  msg.set_msg_type("");

  if (this->serverConn)
  {
    msg.set_host(this->serverConn->GetLocalAddress());
    msg.set_port(this->serverConn->GetLocalPort());
  }

  if (this->masterConn)
  {
    this->masterConn->EnqueueMsg(msgs::Package("unadvertise", msg), true);
  }
}

//////////////////////////////////////////////////
void ConnectionManager::GetAllPublishers(std::list<msgs::Publish> &_publishers)
{
  _publishers.clear();
  std::list<msgs::Publish>::iterator iter;

  boost::recursive_mutex::scoped_lock lock(this->listMutex);
  for (iter = this->publishers.begin(); iter != this->publishers.end(); ++iter)
    _publishers.push_back(*iter);
}

//////////////////////////////////////////////////
void ConnectionManager::GetTopicNamespaces(std::list<std::string> &_namespaces)
{
  if (!this->initialized)
    return;

  _namespaces.clear();

  boost::mutex::scoped_lock lock(this->namespaceMutex);

  if (this->namespaces.empty())
  {
    if (!this->namespaceCondition.timed_wait(lock,
          boost::posix_time::milliseconds(60000)))
    {
      gzerr << "Unable to get namespaces from master\n";
    }
  }

  for (std::list<std::string>::iterator iter = this->namespaces.begin();
       iter != this->namespaces.end(); ++iter)
  {
    _namespaces.push_back(*iter);
  }
}

//////////////////////////////////////////////////
void ConnectionManager::Unsubscribe(const msgs::Subscribe &_sub)
{
  // Inform the master that we want to unsubscribe from a topic.
  this->masterConn->EnqueueMsg(msgs::Package("unsubscribe", _sub), true);
}

//////////////////////////////////////////////////
void ConnectionManager::Unsubscribe(const std::string &_topic,
                                     const std::string &_msgType)
{
  if (this->serverConn)
  {
    msgs::Subscribe msg;
    msg.set_topic(_topic);
    msg.set_msg_type(_msgType);
    msg.set_host(this->serverConn->GetLocalAddress());
    msg.set_port(this->serverConn->GetLocalPort());

    // Inform the master that we want to unsubscribe from a topic.
    this->masterConn->EnqueueMsg(msgs::Package("unsubscribe", msg), true);
  }
}

//////////////////////////////////////////////////
void ConnectionManager::Subscribe(const std::string &_topic,
                                  const std::string &_msgType,
                                  bool _latching)
{
  // We are not initialized, so add the subscription to our pending list.
  if (!this->initialized)
  {
    this->pendingSubscriptions.push_back(
        PendingSubscription(_topic, _msgType, _latching));
    return;
  }

  // TODO:
  // Find a current connection on the topic
  // ConnectionPtr conn = this->FindConnection(topic);

  // If the connection to a remote publisher does not exist, then we need
  // to establish a connection.
  // if (!conn)
  {
    msgs::Subscribe msg;
    msg.set_topic(_topic);
    msg.set_msg_type(_msgType);
    msg.set_host(this->serverConn->GetLocalAddress());
    msg.set_port(this->serverConn->GetLocalPort());
    msg.set_latching(_latching);

    // Inform the master that we want to subscribe to a topic.
    // This will result in Connection::OnMasterRead getting called with a
    // packet type of "publisher_update"
    this->masterConn->EnqueueMsg(msgs::Package("subscribe", msg));
  }
}

//////////////////////////////////////////////////
ConnectionPtr ConnectionManager::ConnectToRemoteHost(const std::string &host,
                                                     unsigned int port)
{
  ConnectionPtr conn;

  if (!this->initialized)
    return conn;

  // Sharing connections is broken
  // conn = this->FindConnection(host, port);
  // if (!conn)
  {
    // Connect to the remote host
    conn.reset(new Connection());
    if (conn->Connect(host, port))
    {
      boost::recursive_mutex::scoped_lock lock(this->connectionMutex);
      this->connections.push_back(conn);
    }
    else
    {
      conn.reset();
      return ConnectionPtr();
    }
  }
  // else
  //  printf("Found Connections\n");

  return conn;
}

//////////////////////////////////////////////////
void ConnectionManager::RemoveConnection(ConnectionPtr &_conn)
{
  std::list<ConnectionPtr>::iterator iter;

  boost::recursive_mutex::scoped_lock lock(this->connectionMutex);
  iter = this->connections.begin();
  while (iter != this->connections.end())
  {
    if ((*iter) == _conn)
    {
      iter = this->connections.erase(iter);
    }
    else
      ++iter;
  }
}

//////////////////////////////////////////////////
ConnectionPtr ConnectionManager::FindConnection(const std::string &_host,
                                                 unsigned int _port)
{
  ConnectionPtr conn;

  std::list<ConnectionPtr>::iterator iter;

  boost::recursive_mutex::scoped_lock lock(this->connectionMutex);

  // Check to see if we are already connected to the remote publisher
  for (iter = this->connections.begin();
       iter != this->connections.end(); ++iter)
  {
    if ((*iter)->IsOpen() && (*iter)->GetRemoteAddress() == _host &&
        (*iter)->GetRemotePort() == _port)
      conn = *iter;
  }

  return conn;
}

//////////////////////////////////////////////////
void ConnectionManager::TriggerUpdate()
{
  this->updateCondition.notify_all();
}

//////////////////////////////////////////////////
void ConnectionManager::ClearBuffers()
{
  // this->namespaces.clear();
  // this->masterMessages.clear();

  this->publishers.clear();
  for (std::list<ConnectionPtr>::iterator iter = this->connections.begin();
       iter != this->connections.end(); ++iter)
  {
    (*iter)->ClearBuffers();
  }
}
