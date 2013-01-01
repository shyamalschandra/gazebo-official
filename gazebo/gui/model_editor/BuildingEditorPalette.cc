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

#include "gui/model_editor/BuildingEditorPalette.hh"
#include "gui/model_editor/FinishModelDialog.hh"
#include "gui/model_editor/EditorEvents.hh"

using namespace gazebo;
using namespace gui;

/////////////////////////////////////////////////
BuildingEditorPalette::BuildingEditorPalette(QWidget *_parent)
    : QWidget(_parent)
{
  this->setObjectName("buildingEditorPalette");

  this->modelName = "building";

  QVBoxLayout *mainLayout = new QVBoxLayout;

  QHBoxLayout *modelNameLayout = new QHBoxLayout;
  QLabel *modelLabel = new QLabel(tr("Model: "));
  modelNameLayout->addWidget(modelLabel);

  modelNameLayout->addItem(new QSpacerItem(10, 20, QSizePolicy::Expanding,
                      QSizePolicy::Minimum));

  QFont underLineFont;
  underLineFont.setUnderline(true);

  QGridLayout *floorPlanLayout = new QGridLayout;

  QLabel *floorPlanLabel = new QLabel(tr("Floor Plan"));
  floorPlanLabel->setFont(underLineFont);

  QLabel *drawWallsLabel = new QLabel;
  drawWallsLabel->setText("Draw Walls");
  QPushButton *addDrawWallButton = new QPushButton;
  addDrawWallButton->setIcon(QIcon(":/images/box.png"));
  addDrawWallButton->setIconSize(QSize(30,30));
  addDrawWallButton->setFlat(true);
  connect(addDrawWallButton, SIGNAL(clicked()), this, SLOT(OnDrawWall()));

  QVBoxLayout *drawWallLayout = new QVBoxLayout;
  drawWallLayout->addWidget(addDrawWallButton);
  drawWallLayout->addWidget(drawWallsLabel);
  floorPlanLayout->addLayout(drawWallLayout, 0, 0);
  floorPlanLayout->setAlignment(Qt::AlignLeft);

  QLabel *importImageLabel = new QLabel;
  importImageLabel->setText("Import Image");
  QPushButton *importImageButton = new QPushButton;
  importImageButton->setIcon(QIcon(":/images/box.png"));
  importImageButton->setIconSize(QSize(30,30));
  importImageButton->setFlat(true);
  connect(importImageButton, SIGNAL(clicked()), this, SLOT(OnImportImage()));

  QVBoxLayout *importImageLayout = new QVBoxLayout;
  importImageLayout->addWidget(importImageButton);
  importImageLayout->addWidget(importImageLabel);
  importImageLayout->setAlignment(Qt::AlignLeft);
  floorPlanLayout->addLayout(importImageLayout, 0, 1);

  QGridLayout *windowDoorLayout = new QGridLayout;
  QLabel *windowDoorLabel = new QLabel(tr("Windows & Doors"));
  windowDoorLabel->setFont(underLineFont);

  QLabel *addWindowLabel = new QLabel;
  addWindowLabel->setText("Add Window");
  QPushButton *addWindowButton = new QPushButton;
  addWindowButton->setIcon(QIcon(":/images/box.png"));
  addWindowButton->setIconSize(QSize(30,30));
  addWindowButton->setFlat(true);
  connect(addWindowButton, SIGNAL(clicked()), this, SLOT(OnAddWindow()));

  QVBoxLayout *addWindowLayout = new QVBoxLayout;
  addWindowLayout->addWidget(addWindowButton);
  addWindowLayout->addWidget(addWindowLabel);
  addWindowLayout->setAlignment(Qt::AlignLeft);
  windowDoorLayout->addLayout(addWindowLayout, 0, 0);

  QLabel *addDoorLabel = new QLabel;
  addDoorLabel->setText("Add Door");
  QPushButton *addDoorButton = new QPushButton;
  addDoorButton->setIcon(QIcon(":/images/box.png"));
  addDoorButton->setIconSize(QSize(30,30));
  addDoorButton->setFlat(true);
  connect(addDoorButton, SIGNAL(clicked()), this, SLOT(OnAddDoor()));

  QVBoxLayout *addDoorLayout = new QVBoxLayout;
  addDoorLayout->addWidget(addDoorButton);
  addDoorLayout->addWidget(addDoorLabel);
  addDoorLayout->setAlignment(Qt::AlignLeft);
  windowDoorLayout->addLayout(addDoorLayout, 0, 1);
  windowDoorLayout->setAlignment(Qt::AlignLeft);

  QLabel *otherLabel = new QLabel(tr("Other"));
  otherLabel->setFont(underLineFont);

  QGridLayout *otherLayout = new QGridLayout;

  QLabel *addStairsLabel = new QLabel;
  addStairsLabel->setText("Add Stairs");
  QPushButton *addStairsButton = new QPushButton;
  addStairsButton->setIcon(QIcon(":/images/box.png"));
  addStairsButton->setIconSize(QSize(30,30));
  addStairsButton->setFlat(true);
  connect(addStairsButton, SIGNAL(clicked()), this, SLOT(OnAddStairs()));

  QVBoxLayout *addStairsLayout = new QVBoxLayout;
  addStairsLayout->addWidget(addStairsButton);
  addStairsLayout->addWidget(addStairsLabel);
  otherLayout->addLayout(addStairsLayout, 0, 0);
  otherLayout->setAlignment(Qt::AlignLeft);

  QPushButton *discardButton = new QPushButton(tr("Discard"));
  connect(discardButton, SIGNAL(clicked()), this, SLOT(OnDiscard()));
  QPushButton *saveButton = new QPushButton(tr("Save"));
  connect(saveButton, SIGNAL(clicked()), this, SLOT(OnSave()));
  QPushButton *finishButton = new QPushButton(tr("Finish"));
  connect(finishButton, SIGNAL(clicked()), this, SLOT(OnFinish()));

  QHBoxLayout *buttonsLayout = new QHBoxLayout;
  buttonsLayout->addWidget(discardButton);
  buttonsLayout->addWidget(saveButton);
  buttonsLayout->addWidget(finishButton);
  buttonsLayout->setAlignment(Qt::AlignCenter);

  mainLayout->addLayout(modelNameLayout);
  mainLayout->addWidget(floorPlanLabel);
  mainLayout->addLayout(floorPlanLayout);
  mainLayout->addWidget(windowDoorLabel);
  mainLayout->addLayout(windowDoorLayout);
  mainLayout->addWidget(otherLabel);
  mainLayout->addLayout(otherLayout);
  mainLayout->addLayout(buttonsLayout);
  mainLayout->setAlignment(Qt::AlignTop);

  this->setLayout(mainLayout);
  this->layout()->setContentsMargins(0, 0, 0, 0);
}

/////////////////////////////////////////////////
BuildingEditorPalette::~BuildingEditorPalette()
{
}

/////////////////////////////////////////////////
void BuildingEditorPalette::OnDrawWall()
{
  gui::Events::createEditorItem("Wall");
}

/////////////////////////////////////////////////
void BuildingEditorPalette::OnImportImage()
{
//  gui::Events::createEditorItem("Image");
}


/////////////////////////////////////////////////
void BuildingEditorPalette::OnAddWindow()
{
  gui::Events::createEditorItem("Window");
}

/////////////////////////////////////////////////
void BuildingEditorPalette::OnAddDoor()
{
  gui::Events::createEditorItem("Door");
}


/////////////////////////////////////////////////
void BuildingEditorPalette::OnAddStairs()
{
  gui::Events::createEditorItem("Stairs");
}

/////////////////////////////////////////////////
void BuildingEditorPalette::OnDiscard()
{
  int ret = QMessageBox::warning(this, tr("Discard"),
      tr("Are you sure you want to discard\n"
      "your model? All of your work will\n"
      "be lost."),
      QMessageBox::Discard | QMessageBox::Cancel,
      QMessageBox::Discard);

  switch (ret)
  {
    case QMessageBox::Discard:
      gui::Events::discardModel();
      break;
    case QMessageBox::Cancel:
    // Do nothing
    break;
    default:
    break;
  }

}

/////////////////////////////////////////////////
void BuildingEditorPalette::OnSave()
{
  bool ok;
  QString text = QInputDialog::getText(this, tr("Save"),
      tr("Please give your model a name:"), QLineEdit::Normal,
      QString(this->modelName.c_str()), &ok);
  if (ok && !text.isEmpty())
  {
    /// TODO
  }
}

/////////////////////////////////////////////////
void BuildingEditorPalette::OnFinish()
{
  FinishModelDialog dialog(this);
  if (dialog.exec() == QDialog::Accepted)
  {
    gui::Events::finishModel(dialog.GetModelName(), dialog.GetSaveLocation());
  }
}
