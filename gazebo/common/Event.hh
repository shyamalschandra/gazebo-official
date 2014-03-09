/*
 * Copyright (C) 2012-2014 Open Source Robotics Foundation
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

#ifndef _EVENT_HH_
#define _EVENT_HH_

#include <iostream>
#include <vector>
#include <map>

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include <gazebo/gazebo_config.h>
#include <gazebo/common/Time.hh>
#include <gazebo/common/CommonTypes.hh>
#include <gazebo/math/Helpers.hh>

namespace gazebo
{
  /// \ingroup gazebo_event
  /// \brief Event namespace
  namespace event
  {
    /// \addtogroup gazebo_event Events
    /// \{

    /// \internal
    // Private data members for Event class.
    // This must be in the header due to templatization.
    class EventPrivate
    {
      // \brief Constructor
      public: EventPrivate();

      /// \brief True if the event has been signaled.
      public: bool signaled;
    };

    /// \class Event Event.hh common/common.hh
    /// \brief Base class for all events
    class Event
    {
      /// \brief Constructor
      public: Event();

      /// \brief Destructor
      public: virtual ~Event();

      /// \brief Disconnect
      /// \param[in] _c A pointer to a connection
      public: virtual void Disconnect(ConnectionPtr _c) = 0;

      /// \brief Disconnect
      /// \param[in] _id Integer ID of a connection
      public: virtual void Disconnect(int _id) = 0;

      /// \brief Get whether this event has been signaled.
      /// \return True if the event has been signaled.
      public: bool GetSignaled() const;

      /// \brief Allow subclasses to initialize their own data pointer.
      /// \param[in] _d Reference to data pointer.
      protected: Event(EventPrivate &_d);

      /// \brief Data pointer.
      protected: EventPrivate *dataPtr;
    };

    /// \internal
    // Private data members for Connection class.
    class ConnectionPrivate
    {
      /// \brief Constructor.
      public: ConnectionPrivate();

      /// \brief Constructor.
      /// \param[in] _e Event pointer to connect with
      /// \param[in] _i Unique id
      public: ConnectionPrivate(Event *_e, int _i);

      /// \brief the event for this connection
      public: Event *event;

      /// \brief the id set in the constructor
      public: int id;

      /// \brief set during the constructor
      public: common::Time creationTime;
    };

    /// \brief A class that encapsulates a connection.
    class Connection
    {
      /// \brief Constructor.
      public: Connection();

      /// \brief Constructor.
      /// \param[in] _e Event pointer to connect with.
      /// \param[in] _i Unique id.
      public: Connection(Event *_e, int _i);

      /// \brief Destructor.
      public: ~Connection();

      /// \brief Get the id of this connection.
      /// \return The id of this connection.
      public: int GetId() const;

      /// \brief Private data pointer.
      private: ConnectionPrivate *dataPtr;

      /// \brief Friend class.
      public: template<typename T> friend class EventT;
    };

    /// \internal
    // Private data members for EventT<T> class.
    template< typename T>
    class EventTPrivate : public EventPrivate
    {
      /// \def EvtConnectionMap
      /// \brief Event Connection map typedef.
      typedef std::map<int, boost::function<T>*> EvtConnectionMap;

      /// \brief Array of connection callbacks.
      public: EvtConnectionMap connections;

      /// \brief Set of connections to erased.
      public: std::vector<int> connectionsToErase;

      /// \brief A thread lock.
      public: boost::mutex connectionsEraseMutex;
    };

    /// \class EventT Event.hh common/common.hh
    /// \brief A class for event processing.
    template< typename T>
    class EventT : public Event
    {
      /// \def EvtConnectionMap.
      /// \brief Event Connection map typedef.
      typedef std::map<int, boost::function<T>*> EvtConnectionMap;

      /// \brief Constructor.
      public: EventT();

      /// \brief Destructor.
      public: virtual ~EventT();

      /// \brief Connect a callback to this event.
      /// \param[in] _subscriber Pointer to a callback function.
      /// \return A Connection object, which will automatically call
      /// Disconnect when it goes out of scope.
      public: ConnectionPtr Connect(const boost::function<T> &_subscriber);

      /// \brief Disconnect a callback to this event.
      /// \param[in] _c The connection to disconnect.
      public: virtual void Disconnect(ConnectionPtr _c);

      /// \brief Disconnect a callback to this event.
      /// \param[in] _id The id of the connection to disconnect.
      public: virtual void Disconnect(int _id);

      /// \brief Get the number of connections.
      /// \return Number of connection to this Event.
      public: unsigned int ConnectionCount() const;

      /// \brief Access the signal.
      public: void operator()()
              {this->Signal();}

      /// \brief Signal the event for all subscribers.
      public: void Signal()
              {
                this->myDataPtr->signaled = true;
                this->Cleanup();
                for (typename EvtConnectionMap::iterator iter =
                    this->myDataPtr->connections.begin();
                    iter != this->myDataPtr->connections.end(); ++iter)
                {
                  (*iter->second)();
                }
              }

      /// \brief Signal the event with one parameter.
      /// \param[in] _p the parameter.
      public: template< typename P >
              void operator()(const P &_p)
              { this->Signal(_p); }

      /// \brief Signal the event with two parameters.
      /// \param[in] _p1 the first parameter.
      /// \param[in] _p2 the second parameter.
      public: template< typename P1, typename P2 >
              void operator()(const P1 &_p1, const P2 &_p2)
              { this->Signal(_p1, _p2); }

      /// \brief Signal the event with three parameters.
      /// \param[in] _p1 the first parameter.
      /// \param[in] _p2 the second parameter.
      /// \param[in] _p3 the second parameter.
      public: template< typename P1, typename P2, typename P3 >
              void operator()(const P1 &_p1, const P2 &_p2, const P3 &_p3)
              { this->Signal(_p1, _p2, _p3); }

      /// \brief Signal the event with four parameters.
      /// \param[in] _p1 the first parameter.
      /// \param[in] _p2 the second parameter.
      /// \param[in] _p3 the second parameter.
      /// \param[in] _p4 the first parameter.
      public: template< typename P1, typename P2, typename P3, typename P4 >
              void operator()(const P1 &_p1, const P2 &_p2, const P3 &_p3,
                              const P4 &_p4)
              { this->Signal(_p1, _p2, _p3, _p4); }

      /// \brief Signal the event with five parameters.
      /// \param[in] _p1 the first parameter.
      /// \param[in] _p2 the second parameter.
      /// \param[in] _p3 the second parameter.
      /// \param[in] _p4 the first parameter.
      /// \param[in] _p5 the fift parameter.
      public: template< typename P1, typename P2, typename P3, typename P4,
                        typename P5 >
              void operator()(const P1 &_p1, const P2 &_p2, const P3 &_p3,
                              const P4 &_p4, const P5 &_p5)
              { this->Signal(_p1, _p2, _p3, _p4, _p5); }

      /// \brief Signal the event with six parameters.
      /// \param[in] _p1 the first parameter.
      /// \param[in] _p2 the second parameter.
      /// \param[in] _p3 the second parameter.
      /// \param[in] _p4 the first parameter.
      /// \param[in] _p5 the fift parameter.
      /// \param[in] _p6 the sixt parameter.
      public: template< typename P1, typename P2, typename P3, typename P4,
                        typename P5, typename P6 >
              void operator()(const P1 &_p1, const P2 &_p2, const P3 &_p3,
                              const P4 &_p4, const P5 &_p5, const P6 &_p6)
              { this->Signal(_p1, _p2, _p3, _p4, _p5, _p6); }

      /// \brief Signal the event with seven parameters.
      /// \param[in] _p1 the first parameter.
      /// \param[in] _p2 the second parameter.
      /// \param[in] _p3 the second parameter.
      /// \param[in] _p4 the first parameter.
      /// \param[in] _p5 the fifth parameter.
      /// \param[in] _p6 the sixth parameter.
      /// \param[in] _p7 the seventh parameter.
      public: template< typename P1, typename P2, typename P3, typename P4,
                        typename P5, typename P6, typename P7 >
              void operator()(const P1 &_p1, const P2 &_p2, const P3 &_p3,
                              const P4 &_p4, const P5 &_p5, const P6 &_p6,
                              const P7 &_p7)
              { this->Signal(_p1, _p2, _p3, _p4, _p5, _p6, _p7); }

      /// \brief Signal the event with eight parameters.
      /// \param[in] _p1 the first parameter.
      /// \param[in] _p2 the second parameter.
      /// \param[in] _p3 the second parameter.
      /// \param[in] _p4 the first parameter.
      /// \param[in] _p5 the fifth parameter.
      /// \param[in] _p6 the sixth parameter.
      /// \param[in] _p7 the seventh parameter.
      /// \param[in] _p8 the eighth parameter.
      public: template< typename P1, typename P2, typename P3, typename P4,
                        typename P5, typename P6, typename P7, typename P8 >
              void operator()(const P1 &_p1, const P2 &_p2, const P3 &_p3,
                              const P4 &_p4, const P5 &_p5, const P6 &_p6,
                              const P7 &_p7, const P8 &_p8)
              { this->Signal(_p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8); }

      /// \brief Signal the event with nine parameters.
      /// \param[in] _p1 the first parameter.
      /// \param[in] _p2 the second parameter.
      /// \param[in] _p3 the second parameter.
      /// \param[in] _p4 the first parameter.
      /// \param[in] _p5 the fifth parameter.
      /// \param[in] _p6 the sixth parameter.
      /// \param[in] _p7 the seventh parameter.
      /// \param[in] _p8 the eighth parameter.
      /// \param[in] _p9 the ninth parameter.
      public: template< typename P1, typename P2, typename P3, typename P4,
                        typename P5, typename P6, typename P7, typename P8,
                        typename P9 >
              void operator()(const P1 &_p1, const P2 &_p2, const P3 &_p3,
                              const P4 &_p4, const P5 &_p5, const P6 &_p6,
                              const P7 &_p7, const P8 &_p8, const P9 &_p9)
              { this->Signal(_p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8, _p9); }

      /// \brief Signal the event with ten parameters.
      /// \param[in] _p1 the first parameter.
      /// \param[in] _p2 the second parameter.
      /// \param[in] _p3 the second parameter.
      /// \param[in] _p4 the first parameter.
      /// \param[in] _p5 the fifth parameter.
      /// \param[in] _p6 the sixth parameter.
      /// \param[in] _p7 the seventh parameter.
      /// \param[in] _p8 the eighth parameter.
      /// \param[in] _p9 the ninth parameter.
      /// \param[in] _p10 the tenth parameter.
      public: template< typename P1, typename P2, typename P3, typename P4,
                        typename P5, typename P6, typename P7, typename P8,
                        typename P9, typename P10 >
              void operator()(const P1 &_p1, const P2 &_p2, const P3 &_p3,
                              const P4 &_p4, const P5 &_p5, const P6 &_p6,
                              const P7 &_p7, const P8 &_p8, const P9 &_p9,
                              const P10 &_p10)
              {
                this->Signal(_p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8, _p9, _p10);
              }

      /// \brief Signal the event with one parameter.
      /// \param[in] _p parameter.
      public: template< typename P >
              void Signal(const P &_p)
              {
                this->myDataPtr->signaled = true;
                this->Cleanup();
                for (typename EvtConnectionMap::iterator iter =
                    this->myDataPtr->connections.begin();
                    iter != this->myDataPtr->connections.end(); ++iter)
                {
                  (*iter->second)(_p);
                }
              }

      /// \brief Signal the event with two parameter.
      /// \param[in] _p1 the first parameter.
      /// \param[in] _p2 the second parameter.
      public: template< typename P1, typename P2 >
              void Signal(const P1 &_p1, const P2 &_p2)
              {
                this->myDataPtr->signaled = true;
                this->Cleanup();
                for (typename EvtConnectionMap::iterator iter =
                    this->myDataPtr->connections.begin();
                    iter != this->myDataPtr->connections.end(); ++iter)
                {
                  (*iter->second)(_p1, _p2);
                }
              }

      /// \brief Signal the event with three parameter.
      /// \param[in] _p1 the first parameter.
      /// \param[in] _p2 the second parameter.
      /// \param[in] _p3 the second parameter.
      public: template< typename P1, typename P2, typename P3 >
              void Signal(const P1 &_p1, const P2 &_p2, const P3 &_p3)
              {
                this->myDataPtr->signaled = true;
                this->Cleanup();
                for (typename EvtConnectionMap::iterator iter =
                    this->myDataPtr->connections.begin();
                      iter != this->myDataPtr->connections.end(); ++iter)
                {
                  (*iter->second)(_p1, _p2, _p3);
                }
              }

      /// \brief Signal the event with four parameter.
      /// \param[in] _p1 the first parameter.
      /// \param[in] _p2 the second parameter.
      /// \param[in] _p3 the second parameter.
      /// \param[in] _p4 the first parameter.
      public: template<typename P1, typename P2, typename P3, typename P4>
              void Signal(const P1 &_p1, const P2 &_p2, const P3 &_p3,
                          const P4 &_p4)
              {
                this->myDataPtr->signaled = true;
                this->Cleanup();
                for (typename EvtConnectionMap::iterator iter =
                    this->myDataPtr->connections.begin();
                        iter != this->myDataPtr->connections.end(); ++iter)
                {
                  (*iter->second)(_p1, _p2, _p3, _p4);
                }
              }

      /// \brief Signal the event with five parameter.
      /// \param[in] _p1 the first parameter.
      /// \param[in] _p2 the second parameter.
      /// \param[in] _p3 the second parameter.
      /// \param[in] _p4 the first parameter.
      /// \param[in] _p5 the fifth parameter.
      public: template<typename P1, typename P2, typename P3, typename P4,
                       typename P5>
              void Signal(const P1 &_p1, const P2 &_p2, const P3 &_p3,
                          const P4 &_p4, const P5 &_p5)
              {
                this->myDataPtr->signaled = true;
                this->Cleanup();
                for (typename EvtConnectionMap::iterator iter =
                    this->myDataPtr->connections.begin();
                          iter != this->myDataPtr->connections.end(); ++iter)
                {
                  (*iter->second)(_p1, _p2, _p3, _p4, _p5);
                }
              }


      /// \brief Signal the event with six parameter.
      /// \param[in] _p1 the first parameter.
      /// \param[in] _p2 the second parameter.
      /// \param[in] _p3 the second parameter.
      /// \param[in] _p4 the first parameter.
      /// \param[in] _p5 the fifth parameter.
      /// \param[in] _p6 the sixth parameter.
      public: template<typename P1, typename P2, typename P3, typename P4,
                       typename P5, typename P6>
              void Signal(const P1 &_p1, const P2 &_p2, const P3 &_p3,
                  const P4 &_p4, const P5 &_p5, const P6 &_p6)
              {
                this->myDataPtr->signaled = true;
                this->Cleanup();
                for (typename EvtConnectionMap::iterator iter =
                    this->myDataPtr->connections.begin();
                    iter != this->myDataPtr->connections.end(); ++iter)
                {
                  (*iter->second)(_p1, _p2, _p3, _p4, _p5, _p6);
                }
              }

      /// \brief Signal the event with seven parameter.
      /// \param[in] _p1 the first parameter.
      /// \param[in] _p2 the second parameter.
      /// \param[in] _p3 the second parameter.
      /// \param[in] _p4 the first parameter.
      /// \param[in] _p5 the fifth parameter.
      /// \param[in] _p6 the sixth parameter.
      /// \param[in] _p7 the seventh parameter.
      public: template<typename P1, typename P2, typename P3, typename P4,
                       typename P5, typename P6, typename P7>
              void Signal(const P1 &_p1, const P2 &_p2, const P3 &_p3,
                  const P4 &_p4, const P5 &_p5, const P6 &_p6, const P7 &_p7)
              {
                this->myDataPtr->signaled = true;
                this->Cleanup();
                for (typename EvtConnectionMap::iterator iter =
                    this->myDataPtr->connections.begin();
                    iter != this->myDataPtr->connections.end(); ++iter)
                {
                  (*iter->second)(_p1, _p2, _p3, _p4, _p5, _p6, _p7);
                }
              }

      /// \brief Signal the event with eight parameter.
      /// \param[in] _p1 the first parameter.
      /// \param[in] _p2 the second parameter.
      /// \param[in] _p3 the second parameter.
      /// \param[in] _p4 the first parameter.
      /// \param[in] _p5 the fifth parameter.
      /// \param[in] _p6 the sixth parameter.
      /// \param[in] _p7 the seventh parameter.
      /// \param[in] _p8 the eighth parameter.
      public: template<typename P1, typename P2, typename P3, typename P4,
                       typename P5, typename P6, typename P7, typename P8>
              void Signal(const P1 &_p1, const P2 &_p2, const P3 &_p3,
                  const P4 &_p4, const P5 &_p5, const P6 &_p6, const P7 &_p7,
                  const P8 &_p8)
              {
                this->myDataPtr->signaled = true;
                this->Cleanup();
                for (typename EvtConnectionMap::iterator iter =
                    this->myDataPtr->connections.begin();
                    iter != this->myDataPtr->connections.end(); ++iter)
                {
                  (*iter->second)(_p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8);
                }
              }

      /// \brief Signal the event with nine parameter.
      /// \param[in] _p1 the first parameter.
      /// \param[in] _p2 the second parameter.
      /// \param[in] _p3 the second parameter.
      /// \param[in] _p4 the first parameter.
      /// \param[in] _p5 the fifth parameter.
      /// \param[in] _p6 the sixth parameter.
      /// \param[in] _p7 the seventh parameter.
      /// \param[in] _p8 the eighth parameter.
      /// \param[in] _p9 the ninth parameter.
      public: template< typename P1, typename P2, typename P3, typename P4,
                        typename P5, typename P6, typename P7, typename P8,
                        typename P9 >
              void Signal(const P1 &_p1, const P2 &_p2, const P3 &_p3,
                  const P4 &_p4, const P5 &_p5, const P6 &_p6, const P7 &_p7,
                  const P8 &_p8, const P9 &_p9)
          {
            this->myDataPtr->signaled = true;
            this->Cleanup();
            for (typename EvtConnectionMap::iterator iter =
                this->myDataPtr->connections.begin();
                iter != this->myDataPtr->connections.end(); ++iter)
            {
              (*iter->second)(_p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8, _p9);
            }
          }

      /// \brief Signal the event with ten parameter.
      /// \param[in] _p1 the first parameter.
      /// \param[in] _p2 the second parameter.
      /// \param[in] _p3 the second parameter.
      /// \param[in] _p4 the first parameter.
      /// \param[in] _p5 the fifth parameter.
      /// \param[in] _p6 the sixth parameter.
      /// \param[in] _p7 the seventh parameter.
      /// \param[in] _p8 the eighth parameter.
      /// \param[in] _p9 the ninth parameter.
      /// \param[in] _p10 the tenth parameter.
      public: template< typename P1, typename P2, typename P3, typename P4,
                        typename P5, typename P6, typename P7, typename P8,
                        typename P9, typename P10 >
              void Signal(const P1 &_p1, const P2 &_p2, const P3 &_p3,
                  const P4 &_p4, const P5 &_p5, const P6 &_p6, const P7 &_p7,
                  const P8 &_p8, const P9 &_p9, const P10 &_p10)
              {
                this->myDataPtr->signaled = true;
                this->Cleanup();
                for (typename EvtConnectionMap::iterator iter =
                    this->myDataPtr->connections.begin();
                    iter != this->myDataPtr->connections.end(); ++iter)
                {
                  (*iter->second)(_p1, _p2, _p3, _p4, _p5,
                      _p6, _p7, _p8, _p9, _p10);
                }
              }

      /// \brief Cleanup disconnected connections.
      private: void Cleanup();

      /// \brief Private data pointer.
      private: EventTPrivate<T> *myDataPtr;
    };

    /// \brief Constructor.
    template<typename T>
    EventT<T>::EventT()
    : Event(*(new EventTPrivate<T>()))
    {
      this->myDataPtr = static_cast<EventTPrivate<T>*>(this->dataPtr);
    }

    /// \brief Destructor. Deletes all the associated connections.
    template<typename T>
    EventT<T>::~EventT()
    {
      for (typename EvtConnectionMap::iterator iter =
          this->myDataPtr->connections.begin();
          iter != this->myDataPtr->connections.end(); ++iter)
      {
        delete iter->second;
      }

      this->myDataPtr->connections.clear();
    }

    /// \brief Adds a connection.
    /// \param[in] _subscriber the subscriber to connect.
    template<typename T>
    ConnectionPtr EventT<T>::Connect(const boost::function<T> &_subscriber)
    {
      int index = 0;
      if (!this->myDataPtr->connections.empty())
      {
        typename EvtConnectionMap::reverse_iterator iter =
          this->myDataPtr->connections.rbegin();
        index = iter->first + 1;
      }
      this->myDataPtr->connections[index] = new boost::function<T>(_subscriber);
      return ConnectionPtr(new Connection(this, index));
    }

    /// \brief Removes a connection.
    /// \param[in] _c the connection.
    template<typename T>
    void EventT<T>::Disconnect(ConnectionPtr _c)
    {
      if (!_c)
        return;

      this->Disconnect(_c->GetId());
      _c->dataPtr->event = NULL;
      _c->dataPtr->id = -1;
    }

    /// \brief Get the number of connections.
    /// \return Number of connections.
    template<typename T>
    unsigned int EventT<T>::ConnectionCount() const
    {
      return this->myDataPtr->connections.size();
    }

    /// \brief Removes a connection.
    /// \param[in] _id the connection index.
    template<typename T>
    void EventT<T>::Disconnect(int _id)
    {
      boost::mutex::scoped_lock lock(this->myDataPtr->connectionsEraseMutex);
      this->myDataPtr->connectionsToErase.push_back(_id);
    }

    /// \brief Cleanup disconnected connections.
    template<typename T>
    void EventT<T>::Cleanup()
    {
      if (this->myDataPtr->connectionsToErase.empty())
        return;
      boost::mutex::scoped_lock lock(this->myDataPtr->connectionsEraseMutex);

      for (std::vector<int>::iterator iter =
          this->myDataPtr->connectionsToErase.begin();
          iter != this->myDataPtr->connectionsToErase.end(); ++iter)
      {
        typename EvtConnectionMap::iterator iter2 =
          this->myDataPtr->connections.find(*iter);
        if (iter2 != this->myDataPtr->connections.end())
        {
          delete iter2->second;
          this->myDataPtr->connections.erase(iter2);
        }
      }
      this->myDataPtr->connectionsToErase.clear();
    }
    /// \}
  }
}
#endif
