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
#ifndef CONNECTION_HH
#define CONNECTION_HH

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
//#include <boost/thread.hpp>
#include <boost/tuple/tuple.hpp>
#include <iostream>
#include <iomanip>

#include <google/protobuf/message.h>

#include "common/GazeboError.hh"

#define HEADER_LENGTH 8

namespace gazebo
{
  namespace transport
  {
    class Connection;
    typedef boost::shared_ptr<Connection> ConnectionPtr;

    class Connection
    {
      public: Connection();
      public: virtual ~Connection();

      /// \brief Connect to a remote host
      public: void Connect(const std::string &host,  unsigned short port);

      typedef boost::function<void(const ConnectionPtr&)> AcceptCallback;

      /// \brief Start a server that listens on a port
      public: void Listen(unsigned short port, const AcceptCallback &accept_cb);


      /// \brief Get the number of messages in the read buffer
      //public: unsigned int GetReadBufferSize();

      /// \brief Pop one message off the read buffer, and return the
      ///        serialized data in msg
      //public: void PopReadBuffer(std::string &msg);

      /// \brief Read data from the socket
      public: void Read(std::string &data);

      /// \brief Write out a message
      public: void Write(const google::protobuf::Message &msg);

      /// \brief Write data to the socket
      public: void Write(const std::string &buffer);


      /// \brief Get the address of this connection
      public: std::string GetLocalAddress() const;

      /// \brief Get the port of this connection
      public: unsigned short GetLocalPort() const;

      /// \brief Get the remote address
      public: std::string GetRemoteAddress() const;

      /// \brief Get the remote port number
      public: unsigned short GetRemotePort() const;


      public: template<typename Handler>
              void AsyncRead(Handler handler)
              {
                void (Connection::*f)(const boost::system::error_code &,
                    boost::tuple<Handler>) = &Connection::OnReadHeader<Handler>;

                boost::asio::async_read(this->socket,
                    boost::asio::buffer(this->inbound_header),
                    boost::bind(f, this, boost::asio::placeholders::error,
                                boost::make_tuple(handler)) );
              }

      // Handle a completed read of a message header. The handler is passed
      // using a tuple since boost::bind seems to have trouble binding
      // a function object created using boost::bind as a parameter
      private: template<typename Handler>
               void OnReadHeader(const boost::system::error_code &e,
                                 boost::tuple<Handler> handler)
              {
                if (e)
                {
                  std::cout << "An error occrured reading a header\n";
                  // Pass the error to the handler
                  //boost::get<0>(handler)(e);
                }
                else
                {
                  std::size_t inbound_data_size = 0;
                  try
                  {
                    inbound_data_size = this->ParseHeader(this->inbound_header);
                  }
                  catch (gazebo::common::GazeboError &e)
                  {
                    std::cerr << "Error[" << e << "]\n";
                  }

                  // Start the asynchronous call to receive data
                  this->inbound_data.resize(inbound_data_size);

                  void (Connection::*f)(const boost::system::error_code &e,
                     boost::tuple<Handler>) = &Connection::OnReadData<Handler>;

                  boost::asio::async_read( this->socket, 
                      boost::asio::buffer(this->inbound_data), 
                      boost::bind(f, this, boost::asio::placeholders::error, 
                                  handler) );
                }
              }

     private: template<typename Handler>
              void OnReadData(const boost::system::error_code &e,
                              boost::tuple<Handler> handler)
               {
                 if (e)
                 {
                   std::cerr << "Error:" << e.message() << std::endl;
                 }
                 else
                 {
                   // Inform caller that data has been received
                   std::string data(&this->inbound_data[0], 
                                    this->inbound_data.size());
                   boost::get<0>(handler)(data);
                 }
               }


      /// \brief Handle on write callbacks
     private: void OnWrite(const boost::system::error_code &e);

     private: void OnAccept(const boost::system::error_code &e,
                            ConnectionPtr newConnection);

     /// \brief Parse a header to get the size of a packet
     private: std::size_t ParseHeader( const std::string &header );

     //private: void ReadLoop();

      private: boost::asio::ip::tcp::socket socket;
      private: boost::asio::ip::tcp::acceptor *acceptor;

      private: std::string outbound_header;
      private: std::string outbound_data;

      // Called when a new connection is received
      private: AcceptCallback acceptCB;

      private: char inbound_header[HEADER_LENGTH];
      private: std::vector<char> inbound_data;

      /*private: boost::thread *readThread;
      private: boost::mutex *readBufferMutex;
      private: std::list< std::string > readBuffer;
      private: bool readQuit;
      */
    };

  }
}

#endif
