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

#include "transport/TopicManager.hh"
#include "transport/Subscriber.hh"

using namespace gazebo;
using namespace transport;

//////////////////////////////////////////////////
Subscriber::Subscriber(const std::string &t, NodePtr _node)
  : topic(t), node(_node)
{
}

//////////////////////////////////////////////////
Subscriber::~Subscriber()
{
  this->Unsubscribe();
  this->node.reset();
}

//////////////////////////////////////////////////
std::string Subscriber::GetTopic() const
{
  return this->topic;
}

//////////////////////////////////////////////////
std::string Subscriber::GetMsgType() const
{
  return std::string();
}


//////////////////////////////////////////////////
void Subscriber::Unsubscribe() const
{
  if (this->node)
  {
    TopicManager::Instance()->Unsubscribe(this->topic, this->node);
  }
}


