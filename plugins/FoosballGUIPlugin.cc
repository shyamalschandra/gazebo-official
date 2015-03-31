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

#include <gazebo/common/Plugin.hh>
#include <gazebo/gui/GuiPlugin.hh>
#include "FoosballGUIPlugin.hh"

using namespace gazebo;

// Register this plugin with the simulator
GZ_REGISTER_GUI_PLUGIN(FoosballGUIPlugin)

/////////////////////////////////////////////////
FoosballGUIPlugin::FoosballGUIPlugin()
  : GUIPlugin()
{
  this->setStyleSheet(
      "QFrame {\
         background-color: rgba(100, 100, 100, 255);\
         border: 0px solid white;\
         font-size: 50px;\
         font-family: 'Impact';\
      }");

  // Time
  QLabel *timeLabel = new QLabel();
  connect(this, SIGNAL(SetTime(QString)), timeLabel,
      SLOT(setText(QString)), Qt::QueuedConnection);

  // Score
  QLabel *scoreLabel = new QLabel();
  connect(this, SIGNAL(SetScore(QString)), scoreLabel,
      SLOT(setText(QString)), Qt::QueuedConnection);

  // Blue team
  QPixmap bluePixmap(100, 100);
  bluePixmap.fill(QColor(100, 100, 100));
  QPainter bluePainter(&bluePixmap);
  bluePainter.setRenderHint(QPainter::Antialiasing, true);
  QPen bluePen(Qt::blue, 2);
  bluePainter.setPen(bluePen);
  QBrush blueBrush(Qt::blue);
  bluePainter.setBrush(blueBrush);
  bluePainter.drawEllipse(10, 10, 80, 80);
  QLabel *blueLabel = new QLabel();
  blueLabel->setPixmap(bluePixmap);

  // Red team
  QPixmap redPixmap(100, 100);
  redPixmap.fill(QColor(100, 100, 100));
  QPainter redPainter(&redPixmap);
  redPainter.setRenderHint(QPainter::Antialiasing, true);
  QPen redPen(Qt::red, 2);
  redPainter.setPen(redPen);
  QBrush redBrush(Qt::red);
  redPainter.setBrush(redBrush);
  redPainter.drawEllipse(10, 10, 80, 80);
  QLabel *redLabel = new QLabel();
  redLabel->setPixmap(redPixmap);

  // State
  QLabel *stateLabel = new QLabel();
  connect(this, SIGNAL(SetState(QString)), stateLabel,
      SLOT(setText(QString)), Qt::QueuedConnection);

  // Frame Layout
  QHBoxLayout *frameLayout = new QHBoxLayout();
  frameLayout->setContentsMargins(0, 0, 0, 0);
  frameLayout->addWidget(timeLabel);
  frameLayout->addWidget(blueLabel);
  frameLayout->addWidget(scoreLabel);
  frameLayout->addWidget(redLabel);
  frameLayout->addWidget(stateLabel);

  frameLayout->setAlignment(timeLabel, Qt::AlignCenter);
  frameLayout->setAlignment(blueLabel, Qt::AlignRight);
  frameLayout->setAlignment(scoreLabel, Qt::AlignCenter);
  frameLayout->setAlignment(redLabel, Qt::AlignLeft);
  frameLayout->setAlignment(stateLabel, Qt::AlignCenter);

  // Frame
  QFrame *mainFrame = new QFrame();
  mainFrame->setLayout(frameLayout);

  // Main Layout
  QHBoxLayout *mainLayout = new QHBoxLayout();
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->addWidget(mainFrame);
  this->setLayout(mainLayout);
  this->move(0, 0);
  this->resize(1920, 100);

  // Initialize transport.
  this->gzNode = transport::NodePtr(new transport::Node());
  this->gzNode->Init();

  this->timeSub =
    this->gzNode->Subscribe("~/foosball_demo/time",
    &FoosballGUIPlugin::OnTime, this);

  this->scoreSub =
    this->gzNode->Subscribe("~/foosball_demo/score",
    &FoosballGUIPlugin::OnScore, this);

  this->stateSub =
    this->gzNode->Subscribe("~/foosball_demo/state",
    &FoosballGUIPlugin::OnState, this);

  this->restartBallPub =
    this->gzNode->Advertise<msgs::Int>("~/foosball_demo/restart_ball");

  this->restartGamePub =
    this->gzNode->Advertise<msgs::Int>("~/foosball_demo/restart_game");

  // Connect hotkeys
  QShortcut *restartGame = new QShortcut(QKeySequence("Ctrl+G"), this);
  QObject::connect(restartGame, SIGNAL(activated()), this,
      SLOT(OnRestartGame()));

  QShortcut *restartBall = new QShortcut(QKeySequence("Ctrl+H"), this);
  QObject::connect(restartBall, SIGNAL(activated()), this,
      SLOT(OnRestartBall()));
}

/////////////////////////////////////////////////
FoosballGUIPlugin::~FoosballGUIPlugin()
{
}

/////////////////////////////////////////////////
void FoosballGUIPlugin::Load(sdf::ElementPtr _sdf)
{
  GZ_ASSERT(_sdf, "FoosballGUIPlugin _sdf pointer is NULL");
  this->sdf = _sdf;
}

/////////////////////////////////////////////////
void FoosballGUIPlugin::OnTime(ConstTimePtr &_msg)
{
  std::ostringstream stream;
  unsigned int min, sec;

  stream.str("");

  sec = _msg->sec();
  min = sec / 60;
  sec -= min * 60;

  stream << std::setw(2) << std::setfill('0') << min << ":";
  stream << std::setw(2) << std::setfill('0') << sec;

  this->SetTime(QString::fromStdString(stream.str()));
}

/////////////////////////////////////////////////
void FoosballGUIPlugin::OnScore(ConstGzStringPtr &_msg)
{
  this->SetScore(QString::fromStdString(_msg->data()));
}

/////////////////////////////////////////////////
void FoosballGUIPlugin::OnState(ConstGzStringPtr &_msg)
{
  std::string state = _msg->data();

  if (_msg->data() == "play")
  {
    state = "Play!";
  }
  else if (_msg->data() == "kickoff")
  {
    state = "Kickoff!";
  }
  else if (_msg->data() == "goalA")
  {
    state = "Blue GOAL!";
  }
  else if (_msg->data() == "goalB")
  {
    state = "Red GOAL!";
  }
  else if (_msgs->data() == "finished")
  {
    state = "Game Over!";
  }

  this->SetState(QString::fromStdString(state));
}

/////////////////////////////////////////////////
void FoosballGUIPlugin::OnRestartGame()
{
  msgs::Int msg;
  msg.set_data(1);
  this->restartGamePub->Publish(msg);
}

/////////////////////////////////////////////////
void FoosballGUIPlugin::OnRestartBall()
{
  msgs::Int msg;
  msg.set_data(1);
  this->restartBallPub->Publish(msg);
}
