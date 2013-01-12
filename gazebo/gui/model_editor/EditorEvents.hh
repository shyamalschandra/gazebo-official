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
#ifndef _EDITOR_EVENTS_HH_
#define _EDITOR_EVENTS_HH_

#include <string>
#include "gazebo/common/Event.hh"

namespace gazebo
{
  namespace gui
  {
    namespace editor
    {
      class Events
      {
        /// \brief Connect a boost::slot to the create editor item signal
        /// \param[in] _subscriber the subscriber to this event
        /// \return a connection
        public: template<typename T>
            static event::ConnectionPtr ConnectCreateEditorItem(T _subscriber)
          { return createEditorItem.Connect(_subscriber); }

        /// \brief Disconnect a boost::slot to the create editor item signal
        /// \param[in] _subscriber the subscriber to this event
        public: static void DisconnectCreateEditorItem(
            event::ConnectionPtr _subscriber)
          { createEditorItem.Disconnect(_subscriber); }

        /// \brief Connect a boost::slot to the save model signal
        /// \param[in] _subscriber the subscriber to this event
        /// \return a connection
        public: template<typename T>
            static event::ConnectionPtr ConnectSaveModel(T _subscriber)
          { return saveModel.Connect(_subscriber); }

        /// \brief Disconnect a boost::slot to the save model signal
        /// \param[in] _subscriber the subscriber to this event
        public: static void DisconnectSaveModel(
            event::ConnectionPtr _subscriber)
          { saveModel.Disconnect(_subscriber); }

        /// \brief Connect a boost::slot to the finish model signal
        /// \param[in] _subscriber the subscriber to this event
        /// \return a connection
        public: template<typename T>
            static event::ConnectionPtr ConnectFinishModel(T _subscriber)
          { return finishModel.Connect(_subscriber); }

        /// \brief Disconnect a boost::slot to the finish model signal
        /// \param[in] _subscriber the subscriber to this event
        public: static void DisconnectFinishModel(
            event::ConnectionPtr _subscriber)
          { finishModel.Disconnect(_subscriber); }

        /// \brief Connect a boost::slot to the discard model signal
        /// \param[in] _subscriber the subscriber to this event
        /// \return a connection
        public: template<typename T>
            static event::ConnectionPtr ConnectDiscardModel(T _subscriber)
          { return discardModel.Connect(_subscriber); }

        /// \brief Disconnect a boost::slot to the discard model signal
        /// \param[in] _subscriber the subscriber to this event
        public: static void DisconnectDiscardModel(
            event::ConnectionPtr _subscriber)
          { discardModel.Disconnect(_subscriber); }

        /// \brief Connect a boost::slot to the change model signal
        /// \param[in] _subscriber the subscriber to this event
        /// \return a connection
        public: template<typename T>
            static event::ConnectionPtr ConnectChangeLevel(T _subscriber)
          { return changeLevel.Connect(_subscriber); }

        /// \brief Disconnect a boost::slot to the change level signal
        /// \param[in] _subscriber the subscriber to this event
        public: static void DisconnectChangeLevel(
            event::ConnectionPtr _subscriber)
          { changeLevel.Disconnect(_subscriber); }

        /// \brief Connect a boost::slot to the add level signal
        /// \param[in] _subscriber the subscriber to this event
        /// \return a connection
        public: template<typename T>
            static event::ConnectionPtr ConnectAddLevel(T _subscriber)
          { return addLevel.Connect(_subscriber); }

        /// \brief Disconnect a boost::slot to the add level signal
        /// \param[in] _subscriber the subscriber to this event
        public: static void DisconnectAddLevel(
            event::ConnectionPtr _subscriber)
          { addLevel.Disconnect(_subscriber); }

        /// \brief Connect a boost::slot to the delete level signal
        /// \param[in] _subscriber the subscriber to this event
        /// \return a connection
        public: template<typename T>
            static event::ConnectionPtr ConnectDeleteLevel(T _subscriber)
          { return deleteLevel.Connect(_subscriber); }

        /// \brief Disconnect a boost::slot to the delete level signal
        /// \param[in] _subscriber the subscriber to this event
        public: static void DisconnectDeleteLevel(
                    event::ConnectionPtr _subscriber)
                { deleteLevel.Disconnect(_subscriber); }

        /// \brief Connect a boost::slot to the change level name signal
        /// \param[in] _subscriber the subscriber to this event
        /// \return a connection
        public: template<typename T>
            static event::ConnectionPtr ConnectChangeLevelName(T _subscriber)
          { return changeLevelName.Connect(_subscriber); }

        /// \brief Disconnect a boost::slot to the change level name signal
        /// \param[in] _subscriber the subscriber to this event
        public: static void DisconnectChangeLevelName(
                    event::ConnectionPtr _subscriber)
                { changeLevelName.Disconnect(_subscriber); }

        /// \brief Connect a boost::slot to the change zoom signal
        /// \param[in] _subscriber the subscriber to this event
        /// \return a connection
        public: template<typename T>
          static event::ConnectionPtr ConnectChangeZoom(T _subscriber)
        { return changeZoom.Connect(_subscriber); }

        /// \brief Disconnect a boost::slot to the change zoom level signal
        /// \param[in] _subscriber the subscriber to this event
        public: static void DisconnectChangeZoom(
            event::ConnectionPtr _subscriber)
          { changeZoom.Disconnect(_subscriber); }

        /// \brief Connect a boost::slot to the save signal
        /// \param[in] _subscriber the subscriber to this event
        /// \return a connection
        public: template<typename T>
            static event::ConnectionPtr ConnectSave(T _subscriber)
          { return save.Connect(_subscriber); }

        /// \brief Disconnect a boost::slot to the save signal
        /// \param[in] _subscriber the subscriber to this event
        public: static void DisconnectSave(
                    event::ConnectionPtr _subscriber)
                { save.Disconnect(_subscriber); }

        /// \brief Connect a boost::slot to the done signal
        /// \param[in] _subscriber the subscriber to this event
        /// \return a connection
        public: template<typename T>
            static event::ConnectionPtr ConnectDone(T _subscriber)
          { return done.Connect(_subscriber); }

        /// \brief Disconnect a boost::slot to the done signal
        /// \param[in] _subscriber the subscriber to this event
        public: static void DisconnectDone(
                    event::ConnectionPtr _subscriber)
                { done.Disconnect(_subscriber); }

        /// \brief Connect a boost::slot to the discard signal
        /// \param[in] _subscriber the subscriber to this event
        /// \return a connection
        public: template<typename T>
            static event::ConnectionPtr ConnectDiscard(T _subscriber)
          { return discard.Connect(_subscriber); }

        /// \brief Disconnect a boost::slot to the discard signal
        /// \param[in] _subscriber the subscriber to this event
        public: static void DisconnectDiscard(
                    event::ConnectionPtr _subscriber)
                { discard.Disconnect(_subscriber); }

        /// \brief Connect a boost::slot to the exit signal
        /// \param[in] _subscriber the subscriber to this event
        /// \return a connection
        public: template<typename T>
            static event::ConnectionPtr ConnectExit(T _subscriber)
          { return exit.Connect(_subscriber); }

        /// \brief Disconnect a boost::slot to the exit signal
        /// \param[in] _subscriber the subscriber to this event
        public: static void DisconnectExit(
            event::ConnectionPtr _subscriber)
          { exit.Disconnect(_subscriber); }

        /// \brief An editor item is to be created
        public: static event::EventT<void (std::string)> createEditorItem;

        /// \brief A model has been saved with a name and a location
        public: static event::EventT<void (std::string, std::string)>
            saveModel;

        /// \brief A model has been completed and uploaded onto the server.
        public: static event::EventT<void ()> finishModel;

        /// \brief A model has been discarded
        public: static event::EventT<void ()> discardModel;

        /// \brief The current level has been changed
        public: static event::EventT<void (int)> changeLevel;

        /// \brief A new level has been added
        public: static event::EventT<void ()> addLevel;

        /// \brief A new level has been deleted
        public: static event::EventT<void (int)> deleteLevel;

        /// \brief The current level name has been changed
        public: static event::EventT<void (int, std::string)> changeLevelName;

        /// \brief The current zoom level has been changed
        public: static event::EventT<void (double)> changeZoom;

        /// \brief Save the model
        public: static event::EventT<void ()> save;

        /// \brief Discard the model
        public: static event::EventT<void ()> discard;

        /// \brief Finish creating the model, save, and exit
        public: static event::EventT<void ()> done;

        /// \brief Exit the editor mode with the option to save
        public: static event::EventT<void ()> exit;
      };
    }
  }
}
#endif
