/*
 * Copyright 2014 Open Source Robotics Foundation
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

#include <curl/curl.h>
#include <QMessageBox>
#include "RestUiWidget.hh"

using namespace gazebo;
using namespace std;



RestUiWidget::RestUiWidget(QWidget *_parent,
                            const char* _menuTitle,
                            const char* _loginTitle,
                            const char* _urlLabel,
                            const char* _defautlUrl)
  : QWidget(_parent),
    title(_menuTitle),
    node(new gazebo::transport::Node()),
    dialog(this, _loginTitle, _urlLabel, _defautlUrl)
{
  
  cout << "RestUiWidget::RestUiWidget node setup" << endl;
  node->Init( );
  cout << "advertizing on /gazebo/event/rest_login" << endl;
  pub = node->Advertise<gazebo::msgs::RestLogin>("/gazebo/event/rest_login");
  // this for a problem where the server cannot subscribe to the topic
  cout << "wait for connection..." << endl;
  pub->WaitForConnection();
  cout << "subscribing on /gazebo/event/rest_error" << endl;
  sub = node->Subscribe("/gazebo/event/rest_error", &RestUiWidget::OnResponse, this);
  cout << "done" << endl;
}
 

RestUiWidget::~RestUiWidget()
{
  cout << "RestUiWidget::~RestUiWidget()" << endl;
}

void RestUiWidget::LoginMOOC()
{
  std::cout << "MOOCUI login" << std::endl;

  if(dialog.exec() == QDialog::Rejected) {
    cout << "RestUiWidget::Login CANCELLED" << endl;
  } else {
    gazebo::msgs::RestLogin msg;
    msg.set_url(dialog.getUrl());
    msg.set_username(dialog.getUsername());
    msg.set_password(dialog.getPassword());
    cout << "RestUiWidget::LoginMOOC() Login  [";
    cout << dialog.getUrl() << ", ";
    cout << dialog.getUsername() << ", ";
    // cout << dialog.getPassword()
    cout << "*****"; 
    cout << "]"; 
    cout << endl;

    pub->Publish(msg);
  } 
}


void RestUiWidget::OnResponse(ConstRestErrorPtr &_msg )
{
  cout << "Error received:" << endl;
  cout << " type: " << _msg->type() << endl;
  cout << " msg:  " << _msg->msg() << endl;

  // add msg to queue for later processing from
  // the GUI thread
  // msgQueue.push_back(_msg);
  msgRespQ.push_back(_msg);
}

void  RestUiWidget::Update()
{
  // Login problem?
  while(!msgRespQ.empty()) {
    ConstRestErrorPtr msg = msgRespQ.front();
    msgRespQ.pop_front();
    if(msg->type().c_str() == string("Error")) 
      QMessageBox::critical(this, tr(this->title.c_str()),tr(msg->msg().c_str()) );
    else
      QMessageBox::information(this, tr(this->title.c_str()),tr(msg->msg().c_str()) );
  }
}
