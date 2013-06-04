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
/* Desc: Handles pushing messages out on a named topic
 * Author: Nate Koenig
 */

#include "gazebo/common/Exception.hh"
#include "gazebo/transport/Node.hh"
#include "gazebo/transport/TopicManager.hh"
#include "gazebo/transport/Publisher.hh"

using namespace gazebo;
using namespace transport;

unsigned int Publisher::idCounter = 0;

//////////////////////////////////////////////////
Publisher::Publisher(const std::string &_topic, const std::string &_msgType,
                     unsigned int _limit, bool /*_latch*/)
  : topic(_topic), msgType(_msgType), queueLimit(_limit)
{
  this->id = idCounter++;
  this->queueLimitWarned = false;
  this->updatePeriod = 0;
}

//////////////////////////////////////////////////
Publisher::Publisher(const std::string &_topic, const std::string &_msgType,
                     unsigned int _limit, double _hzRate)
  : topic(_topic), msgType(_msgType), queueLimit(_limit),
    updatePeriod(0)
{
  this->id = idCounter++;
  if (!math::equal(_hzRate, 0.0))
    this->updatePeriod = 1.0 / _hzRate;

  this->queueLimitWarned = false;
}

//////////////////////////////////////////////////
Publisher::~Publisher()
{
  bool empty = false;
  {
    boost::mutex::scoped_lock lock(this->mutex);
    empty = this->messages.empty();
  }

  if (!empty)
    this->SendMessage();

  if (!this->topic.empty())
    TopicManager::Instance()->Unadvertise(this->topic);
}

//////////////////////////////////////////////////
bool Publisher::HasConnections() const
{
  return (this->publication &&
      (this->publication->GetCallbackCount() > 0 ||
       this->publication->GetNodeCount() > 0));
}

//////////////////////////////////////////////////
void Publisher::WaitForConnection() const
{
  while (!this->HasConnections())
    common::Time::MSleep(100);
}

//////////////////////////////////////////////////
void Publisher::PublishImpl(const google::protobuf::Message &_message,
                            bool _block)
{
  if (_message.GetTypeName() != this->msgType)
    gzthrow("Invalid message type\n");

  if (!_message.IsInitialized())
  {
    gzerr << "Publishing an uninitialized message on topic[" <<
        this->topic << "]. Required field [" <<
        _message.InitializationErrorString() << "] missing.\n";
    return;
  }

  // if (!this->HasConnections())
  // return;

  // Check if a throttling rate has been set
  if (this->updatePeriod > 0)
  {
    // Get the current time
    this->currentTime = common::Time::GetWallTime();

    // Skip publication if the time difference is less than the update period.
    if (this->prevPublishTime != common::Time(0, 0) &&
        (this->currentTime - this->prevPublishTime).Double() <
         this->updatePeriod)
    {
      return;
    }

    // Set the previous time a message was published
    this->prevPublishTime = this->currentTime;
  }

  // Save the latest message
  MessagePtr msgPtr(_message.New());
  msgPtr->CopyFrom(_message);

  {
    boost::mutex::scoped_lock lock(this->mutex);
    if (!this->prevMsg)
      this->prevMsg = msgPtr;

    this->messages.push_back(msgPtr);

    if (this->messages.size() > this->queueLimit)
    {
      this->messages.pop_front();

      if (!queueLimitWarned)
      {
        gzwarn << "Queue limit reached for topic "
               << this->topic
               << ", deleting message. "
               << "This warning is printed only once." << std::endl;
        queueLimitWarned = true;
      }
    }
  }

  TopicManager::Instance()->AddNodeToProcess(this->node);

  if (_block)
  {
    this->SendMessage();
  }
  else
  {
    // Tell the connection manager that it needs to update
    ConnectionManager::Instance()->TriggerUpdate();
  }
}

//////////////////////////////////////////////////
void Publisher::SendMessage()
{
  std::list<MessagePtr> localBuffer;

  {
    boost::mutex::scoped_lock lock(this->mutex);

    std::copy(this->messages.begin(), this->messages.end(),
        std::back_inserter(localBuffer));
    this->messages.clear();
  }

  // Only send messages if there is something to send
  if (!localBuffer.empty())
  {
    // Send all the current messages
    for (std::list<MessagePtr>::iterator iter = localBuffer.begin();
        iter != localBuffer.end(); ++iter)
    {
      // Send the latest message.
      TopicManager::Instance()->Publish(this->topic, *iter);
    }

    // Clear the local buffer.
    localBuffer.clear();
  }
}

//////////////////////////////////////////////////
void Publisher::SetNode(NodePtr _node)
{
  this->node = _node;
}

//////////////////////////////////////////////////
unsigned int Publisher::GetOutgoingCount() const
{
  boost::mutex::scoped_lock lock(this->mutex);
  return this->messages.size();
}

//////////////////////////////////////////////////
std::string Publisher::GetTopic() const
{
  return this->topic;
}

//////////////////////////////////////////////////
std::string Publisher::GetMsgType() const
{
  return this->msgType;
}

//////////////////////////////////////////////////
void Publisher::OnPublishComplete()
{
}

//////////////////////////////////////////////////
void Publisher::SetPublication(PublicationPtr &_publication, int _i)
{
  if (_i == 0)
    this->publication = _publication;
}

//////////////////////////////////////////////////
void Publisher::SetPublication(PublicationPtr _publication)
{
  this->publication = _publication;
}

//////////////////////////////////////////////////
bool Publisher::GetLatching() const
{
  return false;
}

//////////////////////////////////////////////////
std::string Publisher::GetPrevMsg() const
{
  std::string result;
  boost::mutex::scoped_lock lock(this->mutex);
  if (this->prevMsg)
    this->prevMsg->SerializeToString(&result);
  return result;
}

//////////////////////////////////////////////////
MessagePtr Publisher::GetPrevMsgPtr() const
{
  boost::mutex::scoped_lock lock(this->mutex);
  if (this->prevMsg)
    return this->prevMsg;
  return MessagePtr();
}

//////////////////////////////////////////////////
void Publisher::ClearBuffers()
{
  boost::mutex::scoped_lock lock(this->mutex);

  this->messages.clear();
  this->prevMsg.reset();
  this->prevPublishTime = common::Time::Zero;
}
