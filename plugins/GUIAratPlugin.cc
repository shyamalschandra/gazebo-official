/*
 * Copyright (C) 2014 Open Source Robotics Foundation
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
#include <sstream>
#include "GUIAratPlugin.hh"

using namespace gazebo;

// Register this plugin with the simulator
GZ_REGISTER_GUI_PLUGIN(GUIAratPlugin)

QTaskButton::QTaskButton(){
  connect(this, SIGNAL(clicked()), this, SLOT(OnButton()));
}

void QTaskButton::SetTaskId(std::string task_id){
  this->id = task_id;
}

void QTaskButton::SetTaskInstructionsDocument(QTextDocument* instr){
  this->instructions = instr;
}

void QTaskButton::SetIndex(int i){
  this->index = i;
}

void QTaskButton::OnButton(){
  emit SendTask(this->id, this->instructions, this->index);
}


/////////////////////////////////////////////////
GUIAratPlugin::GUIAratPlugin()
  : GUIPlugin()
{

  // Read parameters
  common::SystemPaths* paths = common::SystemPaths::Instance();
  this->handImgFilename = paths->FindFileURI("file://media/gui/etc/handsim.png");
  this->configFilename = paths->FindFileURI("file://media/gui/etc/GUIAratPlugin.sdf");

  sdf::SDF parameters;
  
  std::ifstream fileinput(this->configFilename.c_str());
  std::stringstream inputStream;
  inputStream << fileinput.rdbuf();
  std::string sdfString = inputStream.str();
  fileinput.close();
  
  //Parameters for sensor contact visualization
  parameters.SetFromString(sdfString);
  sdf::ElementPtr elem = parameters.root->GetElement("world");
  elem = elem->GetElement("plugin");
  elem->GetElement("circleSize")->GetValue()->Get(circleSize);
  elem->GetElement("forceMin")->GetValue()->Get(forceMin);
  elem->GetElement("forceMax")->GetValue()->Get(forceMax);
  elem->GetElement("colorMin")->GetValue()->Get(colorMin);
  elem->GetElement("colorMax")->GetValue()->Get(colorMax);
  elem->GetElement("handSide")->GetValue()->Get(handSide);

  math::Vector2d handImgDims;
  elem->GetElement("handImgDimensions")->GetValue()->Get(handImgDims);
  handImgX = handImgDims[0];
  handImgY = handImgDims[1];

  elem->GetElement("iconDimensions")->GetValue()->Get(iconSize);

  for(int i = 0; i < 5; i++){
    std::string keyName = fingerNames[i]+"Pos";
    elem->GetElement(keyName)->GetValue()->Get(finger_points[fingerNames[i]]);
  }


  // Set the frame background and foreground colors
  this->setStyleSheet(
      "QFrame { background-color : rgba(100, 100, 100, 255); color : white; }");

  // Create the main layout
  QVBoxLayout *mainLayout = new QVBoxLayout;

  // Create a QGraphicsView to draw the finger force contacts
  this->handScene = new QGraphicsScene(QRectF(0, 0, handImgX, handImgY));
  QGraphicsView *handView = new QGraphicsView(handScene);

  // Load the hand image
  QPixmap* handImg = new QPixmap(QString(handImgFilename.c_str()));
  QGraphicsPixmapItem* handItem = new QGraphicsPixmapItem(*handImg);

  // Draw the hand on the canvas
  handScene->addItem(handItem);

  // Preallocate QGraphicsItems for each contact point
  for(int i = 0; i < 5; i++){
    int xpos = finger_points[fingerNames[i]][0];
    int ypos = finger_points[fingerNames[i]][1];
    this->contactGraphicsItems[fingerNames[i]] = new QGraphicsEllipseItem(xpos, ypos, circleSize, circleSize);
    this->handScene->addItem(this->contactGraphicsItems[fingerNames[i]]);

    this->contactGraphicsItems[fingerNames[i]]->setBrush(QBrush(QColor(255, 255, 255, 0)));
    this->contactGraphicsItems[fingerNames[i]]->setPen(QPen(QColor(153, 153, 153, 255)));
  }

  handScene->update();
  handView->show();

  // Add the frame to the main layout
  handView->setMaximumSize(handImgX+10, handImgY+10);
  mainLayout->addWidget(handView);

  QVBoxLayout *taskLayout = new QVBoxLayout();
  taskLayout->setContentsMargins(0, 0, 0, 0);
  
  QTabWidget *tabWidget = new QTabWidget();
  
  // Populate the tabWidget by parsing out SDF
  instructionsView = new QTextEdit();
  instructionsView->setReadOnly(true);
  instructionsView->setMaximumHeight(handImgY/3);
   
  sdf::ElementPtr taskGroup = elem->GetElement("taskGroup");
  while(taskGroup){
    std::string taskGroupName;
    taskGroup->GetAttribute("name")->Get(taskGroupName);
    // Create a QWidget for the task group
    // Insert this widget into the tabWidget
    QGroupBox *buttonGroup = new QGroupBox();
    QGridLayout *buttonLayout = new QGridLayout();
    
    sdf::ElementPtr task = taskGroup->GetElement("task");
    int i = 0;

    while(task){
      std::string id;
      std::string name;
      std::string icon_path;
      std::string instructions;
      task->GetAttribute("id")->Get(id);
      task->GetAttribute("name")->Get(name);
      task->GetAttribute("icon")->Get(icon_path);
      task->GetAttribute("instructions")->Get(instructions);

      QTaskButton *taskButton = new QTaskButton();
      taskButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
      taskButton->setMaximumWidth(handImgX/3);
      taskButton->setMaximumHeight(handImgX/3);
      taskButton->resize(handImgX/3, handImgY/3);
      taskButton->setText(QString(name.c_str()));
      taskButton->SetTaskId(id);
      QTextDocument* instructionsDocument = new QTextDocument(QString(instructions.c_str()));
      taskButton->SetTaskInstructionsDocument(instructionsDocument);
      taskButton->SetIndex(taskList.size());

      connect(taskButton, SIGNAL(SendTask(std::string, QTextDocument*, int)), this, SLOT(OnTaskSent(std::string, QTextDocument*, int)));

      int col = i%3;
      int row = i/3;
      buttonLayout->addWidget(taskButton, row, col);

      if(icon_path.compare("none") != 0){
        QPixmap icon_picture(QString(paths->FindFileURI(icon_path).c_str()));
        
        taskButton->setIcon(QIcon(icon_picture));
        taskButton->setIconSize(QSize(iconSize[0], iconSize[1]));
        taskButton->setMinimumSize(iconSize[0]+20, iconSize[1]+30);
      }
      if(taskList.empty()){
        instructionsView->setDocument(instructionsDocument);
      }
      taskList.push_back(id);
      this->instructionsList.push_back(instructionsDocument);

      task = task->GetNextElement();
      i++;
    }
    buttonGroup->setMinimumWidth(handImgX);
    buttonGroup->setContentsMargins(0, 0, 0, 0);
    buttonGroup->setLayout(buttonLayout);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(0);

    tabWidget->addTab((QWidget*) buttonGroup, QString(taskGroupName.c_str()));

    taskGroup = taskGroup->GetNextElement();
  }

  currentTaskIndex = 0;

  QFrame *taskFrame = new QFrame();
  tabWidget->setContentsMargins(0, 0, 0, 0);
  taskLayout->addWidget(tabWidget);
  taskLayout->addWidget(instructionsView);

  QHBoxLayout* cycleButtonLayout = new QHBoxLayout();
  QToolButton *resetButton = new QToolButton();
  resetButton->setText(QString("Reset Test"));
  connect(resetButton, SIGNAL(clicked()), this, SLOT(OnResetClicked()));
  cycleButtonLayout->addWidget(resetButton);
  resetButton->setMaximumWidth(handImgX/2);
  QToolButton *nextButton = new QToolButton();
  nextButton->setText(QString("Next Test"));
  connect(nextButton, SIGNAL(clicked()), this, SLOT(OnNextClicked()));
  cycleButtonLayout->addWidget(nextButton);
  nextButton->setMaximumWidth(handImgX/2);
  QFrame* cycleButtonFrame = new QFrame;
  cycleButtonFrame->setLayout(cycleButtonLayout);
  taskLayout->addWidget(cycleButtonFrame);

  taskFrame->setLayout(taskLayout);

  mainLayout->addWidget(taskFrame);
	

  // Remove margins to reduce space
  mainLayout->setContentsMargins(0, 0, 0, 0);

  this->setLayout(mainLayout);

  // Position and resize this widget
  this->setMaximumWidth(handImgX+10);

  // Create a node for transportation
  this->node = transport::NodePtr(new transport::Node());
  this->node->Init();
  this->taskPub = this->node->Advertise<msgs::GzString>("/gazebo/arat/control");
  this->taskNum = 0;
  this->maxTaskCount = 10;


  // Set up an array of subscribers for each contact sensor
  
  contactSubscribers.push_back(this->node->Subscribe(this->getTopicName("Th"),
                              &GUIAratPlugin::OnThumbContact, this));
  contactSubscribers.push_back(this->node->Subscribe(this->getTopicName("Ind"),
                              &GUIAratPlugin::OnIndexContact, this));
  contactSubscribers.push_back(this->node->Subscribe(this->getTopicName("Mid"),
                              &GUIAratPlugin::OnMiddleContact, this));
  contactSubscribers.push_back(this->node->Subscribe(this->getTopicName("Ring"),
                              &GUIAratPlugin::OnRingContact, this));
  contactSubscribers.push_back(this->node->Subscribe(this->getTopicName("Little"),
                              &GUIAratPlugin::OnLittleContact, this));

  this->connections.push_back(event::Events::ConnectPreRender(boost::bind(&GUIAratPlugin::PreRender, this)));

}

/////////////////////////////////////////////////
GUIAratPlugin::~GUIAratPlugin()
{
}

std::string GUIAratPlugin::getTopicName(std::string fingerName){
  std::string topicName = this->handSide+fingerName+"Distal";
  return "/gazebo/default/mpl/"+topicName+"/"+topicName+"_contact_sensor";
}

void GUIAratPlugin::OnThumbContact(ConstContactsPtr &msg){
  this->OnFingerContact(msg, "Th");
}

void GUIAratPlugin::OnIndexContact(ConstContactsPtr &msg){
  this->OnFingerContact(msg, "Ind");
}

void GUIAratPlugin::OnMiddleContact(ConstContactsPtr &msg){
  this->OnFingerContact(msg, "Mid");
}


void GUIAratPlugin::OnRingContact(ConstContactsPtr &msg){
  this->OnFingerContact(msg, "Ring");
}


void GUIAratPlugin::OnLittleContact(ConstContactsPtr &msg){
  this->OnFingerContact(msg, "Little");
}


void GUIAratPlugin::OnFingerContact(ConstContactsPtr &msg, std::string fingerName){
  this->msgQueue.push(ContactsWrapper(msg, fingerName));
}

void GUIAratPlugin::PreRender(){
  //Remove items from the scene
  for(int i = 0; i < 5; i ++){
    std::string fingerName = this->fingerNames[i];

    this->contactGraphicsItems[fingerName]->setBrush(QBrush(QColor(255, 255, 255, 1)));
    this->contactGraphicsItems[fingerName]->setPen(QPen(QColor(153, 153, 153, 255)));
    /*if(this->handScene->items().contains(this->contactGraphicsItems[fingerName])){
      this->handScene->removeItem(this->contactGraphicsItems[fingerName]);
    }*/
  }

  //TODO: position offset, multiple contacts

  //Clear queued messages and draw them
  while( !this->msgQueue.empty()){
    ContactsWrapper wrapper = msgQueue.front();
    ConstContactsPtr msg = wrapper.msg;
    std::string fingerName = wrapper.name;
    msgQueue.pop();
    int numContacts = msg->contact_size();
    if(numContacts > 0){
      // Calculate contact force
      msgs::Vector3d forceVector = msg->contact(0).wrench(0).body_1_wrench().force();
      float f = math::Vector3(forceVector.x(), forceVector.y(), forceVector.z()).GetLength();
      float colorArray[3];
      float forceRange = this->forceMax - this->forceMin;
      for(int i = 0; i < 3; i++){
        float colorRange = this->colorMax[i] - this->colorMin[i];
        colorArray[i] = colorRange/forceRange*f + colorMin[i];
        if(colorArray[i] > colorMin[i]){
          colorArray[i] = colorMin[i];
        } else if (colorArray[i] < colorMax[i]){

          colorArray[i] = colorMax[i];
        }
      }
      QBrush color(QColor(colorArray[0], colorArray[1], colorArray[2]));
      
      this->contactGraphicsItems[fingerName]->setBrush(color);
      this->contactGraphicsItems[fingerName]->setPen(QPen(QColor(0, 0, 0, 0)));
      // Draw on the corresponding spot
      /*if(!this->handScene->items().contains(this->contactGraphicsItems[fingerName])){
        this->handScene->addItem(this->contactGraphicsItems[fingerName]);
      }*/
    }

  }

}

////////////////////////////////////////////////

void GUIAratPlugin::PublishTaskMessage(std::string task_name)
{
  msgs::GzString msg;
  msg.set_data(task_name);
  this->taskPub->Publish(msg);

}

void GUIAratPlugin::OnResetClicked(){
  // Signal to the ArrangePlugin to set up the current task
  PublishTaskMessage(this->taskList[currentTaskIndex]);
}

void GUIAratPlugin::OnNextClicked(){
  this->currentTaskIndex = (this->currentTaskIndex+1) % this->taskList.size();
  PublishTaskMessage(this->taskList[currentTaskIndex]);
  this->instructionsView->setDocument(this->instructionsList[currentTaskIndex]);
}

void GUIAratPlugin::OnTaskSent(std::string id, QTextDocument* instructions, int index)
{
  // Show the instructions to the user
  this->instructionsView->setDocument(instructions);
  PublishTaskMessage(id);
  this->currentTaskIndex = index;
}
