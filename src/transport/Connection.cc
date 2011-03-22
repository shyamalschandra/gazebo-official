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
#include <boost/thread.hpp>

#include "common/Messages.hh"
#include "transport/IOManager.hh"
#include "transport/Connection.hh"

using namespace gazebo;
using namespace transport;

Connection::Connection(boost::asio::io_service &io_service)
  : socket(io_service)
{
}

Connection::~Connection()
{
}

boost::asio::ip::tcp::socket &Connection::GetSocket()
{
  return this->socket;
}

void Connection::Connect(const std::string &host, const std::string &service)
{
  // Resolve the host name into an IP address
  boost::asio::ip::tcp::resolver resolver(IOManager::Instance()->GetIO());
  boost::asio::ip::tcp::resolver::query query(host, service);
  boost::asio::ip::tcp::resolver::iterator endpoint_iter = resolver.resolve(query);
  boost::asio::ip::tcp::resolver::iterator end;

  boost::system::error_code error = boost::asio::error::host_not_found;
  while (error && endpoint_iter != end)
  {
    socket.close();
    socket.connect(*endpoint_iter++, error);
  }
  if (error)
    throw boost::system::system_error(error);
}

std::string Connection::GetRemoteAddress() const
{
  return this->socket.remote_endpoint().address().to_string();
}

////////////////////////////////////////////////////////////////////////////////
/// Get the remote port number
unsigned short Connection::GetRemotePort() const
{
  return this->socket.remote_endpoint().port();
}

void Connection::StartReadThread()
{
  this->readThread = new boost::thread( boost::bind(&Connection::ReadLoop, this ) );
}

void Connection::ReadLoop()
{
  char header[HEADER_LENGTH];
  std::size_t data_size;
  std::vector<char> data;

  while (!this->readQuit)
  {
    std::size_t data_size;
    boost::system::error_code error;

    this->socket.read_some( boost::asio::buffer(header), error );

    data_size = this->ParseHeader( header );
    data.resize( data_size );

    this->socket.read_some( boost::asio::buffer(data), error );

    if (error == boost::asio::error::eof)
    {
      std::cout << "Server closed connection. Stopping read\n";
      break;
    }
    else if (error)
      throw boost::system::system_error(error);
  }
}

std::size_t Connection::ParseHeader( const std::string header )
{
  std::cout << "Parse Header[" << header << "]\n";
  std::size_t data_size = 0;

  std::istringstream is(header);

  if (!(is >> std::hex >> data_size))
  {
    // Header doesn't seem to be valid. Inform the caller
    boost::system::error_code error(boost::asio::error::invalid_argument);
    gzthrow("Invalid header[" + error.message() + "]");
  }

  return data_size;
}

void Connection::OnWrite(const boost::system::error_code &e)
{
  if (e)
    throw boost::system::system_error(e);
}

