#ifndef NODE_HH
#define NODE_HH

#include <string>
#include <boost/shared_ptr.hpp>

#include "transport/Publisher.hh"
#include "transport/Client.hh"
#include "transport/TopicManager.hh"


namespace gazebo
{
  namespace common
  {
    class Node
    {
      public: Node();
      public: virtual ~Node();

      public: void Init(const std::string &master_host, unsigned short master_port);

      public: template<typename M, typename T>
              transport::SubscriberPtr Subscribe(const std::string topic, 
                  void(T::*fp)(const boost::shared_ptr<M const> &), T *obj)
              {
                transport::SubscriberPtr sub;
                sub = transport::TopicManager::Instance()->Subscribe(topic, fp, obj);

                msgs::Subscribe msg;
                msg.set_topic( sub->GetTopic() );
                msg.set_msg_type( sub->GetMsgType() );
                msg.set_host( this->client->GetConnection()->GetLocalAddress() );
                msg.set_port( this->client->GetConnection()->GetLocalPort() );

                this->client->Write(Message::Package("/gazebo/subscribe", msg));

                return sub;
              }
 
      public: template<typename M>
              transport::PublisherPtr Advertise(const std::string topic)
              {
                transport::PublisherPtr pub;
                pub = transport::TopicManager::Instance()->Advertise<M>(topic);

                msgs::Publish msg;
                msg.set_topic( pub->GetTopic() );
                msg.set_msg_type( pub->GetMsgType() );
                msg.set_host( this->client->GetConnection()->GetLocalAddress() );
                msg.set_port( this->client->GetConnection()->GetLocalPort() );

                this->client->Write(Message::Package("/gazebo/publish", msg));

                return pub;
              }

      private: transport::Client *client;
    };
    typedef boost::shared_ptr<Node> NodePtr;
  }
}

#endif
