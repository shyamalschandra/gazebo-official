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

#include "gazebo/transport/transport.hh"

#include "gazebo/common/Events.hh"
#include "gazebo/gui/JointControlWidget.hh"
#include "gazebo/gui/TopicSelector.hh"

using namespace gazebo;
using namespace gui;

/////////////////////////////////////////////////
TopicSelector::TopicSelector(QWidget *_parent)
  : QDialog(_parent)
{
  // This name is used in the qt style sheet
  this->setObjectName("topicSelector");
  this->setWindowIcon(QIcon(":/images/gazebo.svg"));
  this->setWindowTitle(tr("Gazebo: Topic Selector"));

  // Create the main layout for this widget
  QVBoxLayout *mainLayout = new QVBoxLayout;

  this->treeWidget = new QTreeWidget();
  this->treeWidget->setColumnCount(1);
  this->treeWidget->header()->hide();
  this->treeWidget->setMinimumSize(400, 400);
  this->treeWidget->setFocusPolicy(Qt::NoFocus);
  this->treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
  this->treeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  this->treeWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  connect(this->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
          this, SLOT(OnSelection(QTreeWidgetItem *, int)));

  this->GetTopicList();

  QFrame *frame = new QFrame;
  QVBoxLayout *frameLayout = new QVBoxLayout;

  QLabel *topicLabel = new QLabel("Topics:");

  frameLayout->addWidget(topicLabel);
  frameLayout->addWidget(this->treeWidget);
  frameLayout->setContentsMargins(4, 4, 4, 4);
  frame->setLayout(frameLayout);

  QHBoxLayout *buttonLayout = new QHBoxLayout;
  QPushButton *cancelButton = new QPushButton("Cancel");
  connect(cancelButton, SIGNAL(clicked()),
          this, SLOT(OnCancel()));

  this->okayButton = new QPushButton("Okay");
  this->okayButton->setEnabled(false);
  connect(this->okayButton, SIGNAL(clicked()),
          this, SLOT(OnOkay()));

  buttonLayout->addWidget(cancelButton);
  buttonLayout->addStretch(2);
  buttonLayout->addWidget(this->okayButton);

  mainLayout->addWidget(frame);
  mainLayout->addLayout(buttonLayout);

  // Let the stylesheet handle the margin sizes
  mainLayout->setContentsMargins(4, 4, 4, 4);

  // Assign the mainlayout to this widget
  this->setLayout(mainLayout);

}

/////////////////////////////////////////////////
TopicSelector::~TopicSelector()
{
}

/////////////////////////////////////////////////
void TopicSelector::GetTopicList()
{
  std::map<std::string, std::list<std::string> > topics;
  topics = transport::getAdvertisedTopics();

  std::list<std::string> validMsgTypes;
  validMsgTypes.push_back(
      msgs::ImageStamped::default_instance().GetTypeName());

  validMsgTypes.push_back(
      msgs::LaserScan::default_instance().GetTypeName());

  for (std::map<std::string, std::list<std::string> >::iterator
       iter = topics.begin(); iter != topics.end(); ++iter)
  {
    if (iter->first.find("__dbg") == std::string::npos &&
        std::find(validMsgTypes.begin(), validMsgTypes.end(), iter->first) !=
        validMsgTypes.end())
    {
      QTreeWidgetItem *topItem = new QTreeWidgetItem(
          static_cast<QTreeWidgetItem*>(0),
          QStringList(QString::fromStdString(iter->first)));
      this->treeWidget->addTopLevelItem(topItem);

      // Add all the topic names
      for (std::list<std::string>::iterator topicIter = iter->second.begin();
           topicIter != iter->second.end(); ++topicIter)
      {
        QTreeWidgetItem *topicItem = new QTreeWidgetItem(topItem,
          QStringList(QString::fromStdString(*topicIter)));
        this->treeWidget->addTopLevelItem(topicItem);
      }

      // Automatically expand the list of topics.
      topItem->setExpanded(true);
    }
  }
}

/////////////////////////////////////////////////
void TopicSelector::OnOkay()
{
  this->done(QDialog::Accepted);
}

/////////////////////////////////////////////////
void TopicSelector::OnCancel()
{
  this->done(QDialog::Rejected);
}

/////////////////////////////////////////////////
std::string TopicSelector::GetTopic() const
{
  return this->topicName;
}

/////////////////////////////////////////////////
void TopicSelector::OnSelection(QTreeWidgetItem *_item, int /*_column*/)
{
  if (_item->parent())
  {
    this->topicName = _item->text(0).toStdString();
    this->okayButton->setEnabled(true);
  }
  else
  {
    _item->setExpanded(!_item->isExpanded());
    this->topicName.clear();
    this->okayButton->setEnabled(false);
  }
}
