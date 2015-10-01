/*
 * Copyright (C) 2015 Open Source Robotics Foundation
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

#include <boost/range/adaptor/reversed.hpp>

#include "gazebo/transport/Node.hh"

#include "gazebo/gui/Actions.hh"
#include "gazebo/gui/UserCmdHistoryPrivate.hh"
#include "gazebo/gui/UserCmdHistory.hh"

using namespace gazebo;
using namespace gui;

/////////////////////////////////////////////////
UserCmdHistory::UserCmdHistory()
  : dataPtr(new UserCmdHistoryPrivate)
{
  if (!g_undoAct || !g_redoAct || !g_undoHistoryAct || !g_redoHistoryAct)
  {
    gzerr << "Action missing, not initializing UserCmdHistory" << std::endl;
    return;
  }

  // Action groups
  this->dataPtr->undoActions = new QActionGroup(this);
  this->dataPtr->undoActions->setExclusive(false);

  this->dataPtr->redoActions = new QActionGroup(this);
  this->dataPtr->redoActions->setExclusive(false);

  // Pub / sub
  this->dataPtr->node = transport::NodePtr(new transport::Node());
  this->dataPtr->node->Init();

  this->dataPtr->undoRedoPub =
      this->dataPtr->node->Advertise<msgs::UndoRedo>("~/undo_redo");
  this->dataPtr->userCmdStatsSub =
      this->dataPtr->node->Subscribe("~/user_cmd_stats",
      &UserCmdHistory::OnUserCmdStatsMsg, this);

  // Qt connections
  connect(this, SIGNAL(StatsSignal()), this, SLOT(OnStatsSlot()));

  connect(g_undoAct, SIGNAL(triggered()), this, SLOT(OnUndo()));
  connect(g_redoAct, SIGNAL(triggered()), this, SLOT(OnRedo()));
  connect(g_undoHistoryAct, SIGNAL(triggered()), this,
      SLOT(OnUndoCmdHistory()));
  connect(g_redoHistoryAct, SIGNAL(triggered()), this,
      SLOT(OnRedoCmdHistory()));

  connect(this->dataPtr->undoActions, SIGNAL(triggered(QAction *)), this,
      SLOT(OnUndoCommand(QAction *)));
  connect(this->dataPtr->undoActions, SIGNAL(hovered(QAction *)), this,
      SLOT(OnUndoHovered(QAction *)));

  connect(this->dataPtr->redoActions, SIGNAL(triggered(QAction *)), this,
      SLOT(OnRedoCommand(QAction *)));
  connect(this->dataPtr->redoActions, SIGNAL(hovered(QAction *)), this,
      SLOT(OnRedoHovered(QAction *)));
}

/////////////////////////////////////////////////
UserCmdHistory::~UserCmdHistory()
{
  delete this->dataPtr;
  this->dataPtr = NULL;
}

/////////////////////////////////////////////////
void UserCmdHistory::OnUndo()
{
  this->OnUndoCommand(NULL);
}

/////////////////////////////////////////////////
void UserCmdHistory::OnUndoCommand(QAction *_action)
{
  msgs::UndoRedo msg;
  msg.set_undo(true);

  if (_action)
  {
    msg.set_id(_action->data().toString().toStdString());
  }

  this->dataPtr->undoRedoPub->Publish(msg);
}

/////////////////////////////////////////////////
void UserCmdHistory::OnUndoHovered(QAction *_action)
{
  bool beforeThis = true;
  for (auto action : this->dataPtr->undoActions->actions())
  {
    action->blockSignals(true);
    action->setChecked(beforeThis);
    action->blockSignals(false);

    if (action->data() == _action->data())
      beforeThis = false;
  }
}

/////////////////////////////////////////////////
void UserCmdHistory::OnRedo()
{
  msgs::UndoRedo msg;
  msg.set_undo(false);
  // ID
  this->dataPtr->undoRedoPub->Publish(msg);
}

/////////////////////////////////////////////////
void UserCmdHistory::OnRedoCommand(QAction *_action)
{
  msgs::UndoRedo msg;
  msg.set_undo(false);

  if (_action)
  {
    msg.set_id(_action->data().toString().toStdString());
  }

  this->dataPtr->undoRedoPub->Publish(msg);
}

/////////////////////////////////////////////////
void UserCmdHistory::OnRedoHovered(QAction *_action)
{
  bool beforeThis = true;
  for (auto action : this->dataPtr->redoActions->actions())
  {
    action->blockSignals(true);
    action->setChecked(beforeThis);
    action->blockSignals(false);

    if (action->data() == _action->data())
      beforeThis = false;
  }
}

/////////////////////////////////////////////////
void UserCmdHistory::OnUserCmdStatsMsg(ConstUserCmdStatsPtr &_msg)
{
  this->dataPtr->msg.CopyFrom(*_msg);

  this->StatsSignal();
}

/////////////////////////////////////////////////
void UserCmdHistory::OnStatsSlot()
{
  g_undoAct->setEnabled(this->dataPtr->msg.undo_cmd_count() > 0);
  g_redoAct->setEnabled(this->dataPtr->msg.redo_cmd_count() > 0);
  g_undoHistoryAct->setEnabled(this->dataPtr->msg.undo_cmd_count() > 0);
  g_redoHistoryAct->setEnabled(this->dataPtr->msg.redo_cmd_count() > 0);
}

/////////////////////////////////////////////////
void UserCmdHistory::OnUndoCmdHistory()
{
  // Clear undo action group
  for (auto action : this->dataPtr->undoActions->actions())
  {
    this->dataPtr->undoActions->removeAction(action);
  }

  // Create new menu
  QMenu menu;
  for (auto cmd : boost::adaptors::reverse(this->dataPtr->msg.undo_cmd()))
  {
    QAction *action = new QAction(QString::fromStdString(cmd.description()),
        this);
    action->setData(QVariant(QString::fromStdString(cmd.id())));
    action->setCheckable(true);
    menu.addAction(action);
    this->dataPtr->undoActions->addAction(action);
  }

  menu.exec(QCursor::pos());
}

/////////////////////////////////////////////////
void UserCmdHistory::OnRedoCmdHistory()
{
  // Clear redo action group
  for (auto action : this->dataPtr->redoActions->actions())
  {
    this->dataPtr->redoActions->removeAction(action);
  }

  // Create new menu
  QMenu menu;
  for (auto cmd : boost::adaptors::reverse(this->dataPtr->msg.redo_cmd()))
  {
    QAction *action = new QAction(QString::fromStdString(cmd.description()),
        this);
    action->setData(QVariant(QString::fromStdString(cmd.id())));
    action->setCheckable(true);
    menu.addAction(action);
    this->dataPtr->redoActions->addAction(action);
  }

  menu.exec(QCursor::pos());
}

