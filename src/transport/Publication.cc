#include "SubscriptionTransport.hh"
#include "Publication.hh"

using namespace gazebo;
using namespace transport;

////////////////////////////////////////////////////////////////////////////////
// Constructor
Publication::Publication( const std::string &topic, const std::string &msgType )
  : topic(topic), msgType(msgType)
{
  this->prevMsg = NULL;
}

////////////////////////////////////////////////////////////////////////////////
// Destructor
Publication::~Publication()
{
}
        
////////////////////////////////////////////////////////////////////////////////
/// Get the topic for this publication
std::string Publication::GetTopic() const
{
  return this->topic;
}

////////////////////////////////////////////////////////////////////////////////
// Add a subscription callback
void Publication::AddSubscription(const CallbackHelperPtr &callback)
{
  this->callbacks.push_back(callback);

  if (this->prevMsg && this->prevMsg->IsInitialized())
  {
    callback->HandleMessage(this->prevMsg);
  }
}

////////////////////////////////////////////////////////////////////////////////
/// Publish data
void Publication::Publish(const std::string &data)
{
  std::list< CallbackHelperPtr >::iterator iter;
  for (iter = this->callbacks.begin(); iter != this->callbacks.end(); iter++)
  {
    (*iter)->HandleData(data);
  }
}

////////////////////////////////////////////////////////////////////////////////
void Publication::PublishData(const std::string &data)
{
  this->Publish(data);
}

////////////////////////////////////////////////////////////////////////////////
void Publication::Publish(const google::protobuf::Message &msg)
{
  std::list< CallbackHelperPtr >::iterator iter;

  for (iter = this->callbacks.begin(); iter != this->callbacks.end(); iter++)
  {
    (*iter)->HandleMessage(&msg);
  }

  if (!this->prevMsg)
    this->prevMsg = msg.New();

  this->prevMsg->CopyFrom(msg);
}

////////////////////////////////////////////////////////////////////////////////
/// Get the type of message
std::string Publication::GetMsgType() const
{
  return this->msgType;
}

void Publication::AddTransport( const PublicationTransportPtr &publink)
{
  publink->AddCallback( boost::bind(&Publication::PublishData, this, _1) );
  this->transports.push_back( publink );
}

