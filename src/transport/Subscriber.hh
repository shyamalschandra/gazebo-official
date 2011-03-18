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
/* Desc: Handles a subscription to a topic
 * Author: Nate Koenig
 */

#ifndef SUBSCRIBER_HH
#define SUBSCRIBER_HH

#include <string>
#include <boost/shared_ptr.hpp>

#include "transport/Subscription.hh"

namespace gazebo
{
  namespace transport
  {
    class Subscriber
    {
      /// \brief Constructor
      public: Subscriber(const std::string &topic, SubscriptionPtr sub);

      /// \brief Destructor
      public: virtual ~Subscriber();

      /// \brief Get the topic name
      public: std::string GetTopic() const;

      /// \brief Unsubscribe from the topic
      public: void Unsubscribe() const;

      private: std::string topic;
      private: SubscriptionPtr subscription;
    };
    typedef boost::shared_ptr<Subscriber> SubscriberPtr;
  }
}

#endif
