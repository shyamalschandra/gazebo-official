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
#ifndef TOPICMANAGER_HH
#define TOPICMANAGER_HH

#include <map>
#include <list>
#include <boost/bind.hpp>

#include "common/Exception.hh"
#include "common/Messages.hh"
#include "common/SingletonT.hh"

#include "transport/TransportTypes.hh"
#include "transport/SubscribeOptions.hh"
#include "transport/SubscriptionTransport.hh"
#include "transport/PublicationTransport.hh"
#include "transport/ConnectionManager.hh"
//#include "transport/Connection.hh"
#include "transport/Publisher.hh"
#include "transport/Publication.hh"
#include "transport/Subscriber.hh"

namespace gazebo
{
  namespace transport
  {
    /// \brief Manages topics and their subscriptions
    class TopicManager : public SingletonT<TopicManager>
    {
      private: TopicManager();
      private: virtual ~TopicManager();

      public: void Init(unsigned short port);

      public: PublicationPtr FindPublication(const std::string &topic);

      /// \brief Subscribe to a topic
      //public: template<class M>
      public: SubscriberPtr Subscribe(const SubscribeOptions &options);
                  //const boost::function<void (const boost::shared_ptr<M const> &)> &callback)
                  //void(T::*fp)(const boost::shared_ptr<M const> &), T *obj)
              

      /// \brief Unsubscribe from a topic. Use a Subscriber rather than
      ///        calling this function directly
      public: void Unsubscribe(const std::string &topic, CallbackHelperPtr sub);

      /// \brief Advertise on a topic
      /// \param topic The name of the topic
      public: template<typename M>
              PublisherPtr Advertise(const std::string &topic)
              {
                google::protobuf::Message *msg = NULL;
                M msgtype;
                msg = dynamic_cast<google::protobuf::Message *>(&msgtype);
                if (!msg)
                  gzthrow("Advertise requires a google protobuf type");

                if (this->UpdatePublications(topic, msg->GetTypeName()))
                {
                  ConnectionManager::Instance()->Advertise(topic,
                                                           msg->GetTypeName());
                }

                // Connect all local subscription to the publisher
                PublicationPtr publication = this->FindPublication( topic );
                SubMap::iterator iter;
                for (iter = this->subscribed_topics.begin(); 
                     iter != this->subscribed_topics.end(); iter++)
                {
                  if ( iter->first == topic )
                  {
                    std::list<CallbackHelperPtr>::iterator liter;
                    for (liter = iter->second.begin(); 
                         liter != iter->second.end(); liter++)
                    {
                      publication->AddSubscription( *liter );
                    }
                  }
                }

                return PublisherPtr( new Publisher(topic, msg->GetTypeName()) );
              }

      /// \brief Stop advertising on a topic
      public: void Unadvertise(const std::string &topic);

      /// \brief Send a message. Use a Publisher instead of calling this
      ///        function directly.
      /// \param topic Name of the topic
      /// \param message The message to send.
      public: void Publish( const std::string &topic, 
                            google::protobuf::Message &message );

      /// \brief Connection a local Publisher to a remote Subscriber
      public: void ConnectPubToSub( const std::string &topic,
                                    const SubscriptionTransportPtr &sublink );

      /// \brief Connect a local Subscriber to a remote Publisher
      public: void ConnectSubToPub( const std::string &topic,
                                    const PublicationTransportPtr &publink );


      /// \brief Disconnect a local publisher from a remote subscriber
      public: void DisconnectPubFromSub( const std::string &topic, 
                                         const std::string &host, 
                                         unsigned int port);

      /// \brief Disconnection all local subscribers from a remote publisher
      public: void DisconnectSubFromPub( const std::string &topic, 
                                         const std::string &host, 
                                         unsigned int port);

      /// \brief Connect all subscribers on a topic to known publishers
      public: void ConnectSubscibers(const std::string &topic);

      /// \brief Update our list of advertised topics
      /// \return True if the provided params define a new publisher.
      public: bool UpdatePublications( const std::string &topic, 
                                       const std::string &msgType );

      private: void HandleIncoming();

      /// \brief A map <subscribed_topic_name, subscription_callbacks> of 
      ///        subscribers to topics
      typedef std::map< std::string, std::list<CallbackHelperPtr> > SubMap;

      private: std::vector<PublicationPtr> advertisedTopics;
      private: SubMap subscribed_topics; 

      //Singleton implementation
      private: friend class SingletonT<TopicManager>;
    };
  }
}

#endif

