#include <boost/bind.hpp>

#include "common/GazeboError.hh"
#include "common/Messages.hh"
#include "transport/IOManager.hh"
#include "transport/Client.hh"

#include "gazebo_config.h"

using namespace gazebo;
using namespace transport;

Client::Client( const std::string &host, const std::string &service)
{
  this->host = host;
  this->service = service;

  this->connection.reset( new Connection(IOManager::Instance()->GetIO()) );

  try
  {
    this->connection->Connect(this->host, this->service);
  }
  catch (boost::system::system_error e)
  {
    std::cerr << "Unable to connect to server[" << e.what() << "]\n";
  }

  std::cout << "Client connected to server[" << this->connection->GetRemoteAddress() << ":" << this->connection->GetRemotePort() << "]\n";

  // Read the version info
  this->connection->Read( boost::bind(&Client::OnReadInit, this, _1) );
}

void Client::OnReadInit(const std::string &data)
{
  msgs::String gazeboVersion;
  gazeboVersion.ParseFromString(data);

  if (gazeboVersion.data() != (std::string("gazebo ") + GAZEBO_VERSION))
  {
    gzthrow(std::string("Version mismatch. My version[") + GAZEBO_VERSION + "] Remote version[" + gazeboVersion.data() + "]\n");
  }

  this->connection->StartReadThread();
}


void Client::OnRead(const std::string &data)
{
  std::cout << "Client::OnRead[" << data << "]\n";
  //this->connection.async_read( boost::bind(&Client::OnRead, this, _1) );
  //this->callback->OnRead(data);
}
