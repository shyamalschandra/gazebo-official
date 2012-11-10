/*
 * Copyright 2011 Nate Koenig
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
#ifndef CONNECTION_MANAGER_HH
#define CONNECTION_MANAGER_HH


#include <boost/shared_ptr.hpp>
#include <string>
#include <list>
#include <vector>

#include "msgs/msgs.hh"
#include "common/SingletonT.hh"

#include "transport/Publisher.hh"
#include "transport/Connection.hh"

namespace gazebo
{
  namespace transport
  {
    /// \addtogroup gazebo_transport
    /// \{

    /// \class ConnectionManager ConnectionManager.hh transport/transport.hh
    /// \brief Manager of connections
    class ConnectionManager : public SingletonT<ConnectionManager>
    {
      /// \brief Constructor
      private: ConnectionManager();

      /// \brief Destructor
      private: virtual ~ConnectionManager();

      /// \brief Initialize the connection manager
      /// \param[in] _master_host Host where the master is running
      /// \param[in] _master_port Port where the master is running
      /// \return true if initialization succeeded, false otherwise
      public: bool Init(const std::string &_master_host,
                        unsigned int _master_port);

      /// \brief Run the connection manager loop.  Does not return until stopped.
      public: void Run();

      /// \brief Is the manager running?
      /// \return true if running, false otherwise
      public: bool IsRunning() const;

      /// \brief Finalize the connection manager
      public: void Fini();

      /// \brief Stop the conneciton manager
      public: void Stop();

      /// \brief Subscribe to a topic
      /// \param[in] _topic The topic to subscribe to
      /// \param[in] _msgType The type of the topic
      /// \param[in] _latching If true, latch the latest incoming message; otherwise don't
      public: void Subscribe(const std::string &_topic,
                              const std::string &_msgType,
                              bool _latching);

      /// \brief Unsubscribe from a topic
      /// \param[in] _sub A subscription object
      public: void Unsubscribe(const msgs::Subscribe &_sub);

      /// \brief Unsubscribe from a topic
      /// \param[in] _topic The topic to unsubscribe from
      /// \param[in] _msgType The type of the topic
      public: void Unsubscribe(const std::string &_topic,
                                const std::string &_msgType);

      /// \brief Advertise a topic
      /// \param[in] _topic The topic to advertise
      /// \param[in] _msgType The type of the topic
      public: void Advertise(const std::string &_topic,
                              const std::string &_msgType);

      /// \brief Unadvertise a topic
      /// \param[in] _topic The topic to unadvertise
      public: void Unadvertise(const std::string &_topic);

      /// \brief Explicitly update the publisher list
      /// \param[out] _publishers The updated list of publishers is written here
      public: void GetAllPublishers(std::list<msgs::Publish> &_publishers);

      /// \brief Remove a connection from the manager
      /// \param[in] _conn The connection to be removed
      public: void RemoveConnection(ConnectionPtr &_conn);

      /// \brief Register a new topic namespace
      /// \param[in] _name The name of the topic namespace to be registered
      public: void RegisterTopicNamespace(const std::string &_name);

      /// \brief Get all the topic namespaces
      /// \param[out] _namespaces The list of namespace is written here
      public: void GetTopicNamespaces(std::list<std::string> &_namespaces);

      /// \brief Find a connection that matches a host and port
      /// \param[in] _host The host of the connection
      /// \param[in] _port The port of the connection
      /// \return Pointer to the connection; can be null (if no match was found)
      private: ConnectionPtr FindConnection(const std::string &_host,
                                            unsigned int _port);

      /// \brief Connect to a remote server
      /// \param[in] _host Host to connect to
      /// \param[in] _port Port to connect to
      /// \return Pointer to the connection; can be null (if connection failed)
      public: ConnectionPtr ConnectToRemoteHost(const std::string &_host,
                                                  unsigned int _port);

      private: void OnMasterRead(const std::string &data);

      private: void OnAccept(const ConnectionPtr &new_connection);

      private: void OnRead(const ConnectionPtr &new_connection,
                            const std::string &data);

      private: void ProcessMessage(const std::string &_packet);

      /// \brief Run the manager update loop once
      public: void RunUpdate();

      private: ConnectionPtr masterConn;
      private: Connection *serverConn;

      private: std::list<ConnectionPtr> connections;
      protected: std::vector<event::ConnectionPtr> eventConnections;

      private: bool initialized;
      private: bool stop, stopped;
      private: boost::thread *thread;

      private: unsigned int tmpIndex;
      private: boost::recursive_mutex *listMutex;
      private: boost::recursive_mutex *masterMessagesMutex;
      private: boost::recursive_mutex *connectionMutex;

      private: std::list<msgs::Publish> publishers;
      private: std::list<std::string> namespaces;
      private: std::list<std::string> masterMessages;

      // Singleton implementation
      private: friend class SingletonT<ConnectionManager>;
    };
    /// \}
  }
}

#endif


