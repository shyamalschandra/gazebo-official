/*
 * Copyright (C) 2012-2015 Open Source Robotics Foundation
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

#include "gazebo/common/Assert.hh"
#include "gazebo/gui/building/StairsInspectorDialog.hh"

using namespace gazebo;
using namespace gui;

/////////////////////////////////////////////////
StairsInspectorDialog::StairsInspectorDialog(QWidget *_parent)
  : QDialog(_parent)
{
  this->setObjectName("stairsInspectorDialog");

  this->setWindowTitle(tr("Stairs Inspector"));
  this->setWindowFlags(Qt::WindowStaysOnTopHint);

  QLabel *stairsLabel = new QLabel(tr("Stairs Name: "));
  this->stairsNameLabel = new QLabel(tr(""));

  QHBoxLayout *nameLayout = new QHBoxLayout;
  nameLayout->addWidget(stairsLabel);
  nameLayout->addWidget(this->stairsNameLabel);

  QLabel *startXLabel = new QLabel(tr("x: "));
  QLabel *startYLabel = new QLabel(tr("y: "));

  this->startXSpinBox = new QDoubleSpinBox;
  this->startXSpinBox->setRange(-1000, 1000);
  this->startXSpinBox->setSingleStep(0.001);
  this->startXSpinBox->setDecimals(3);
  this->startXSpinBox->setValue(0.000);
  this->startXSpinBox->setAlignment(Qt::AlignRight);
  QLabel *startXUnitLabel = new QLabel(tr("m"));
  startXUnitLabel->setMaximumWidth(40);

  this->startYSpinBox = new QDoubleSpinBox;
  this->startYSpinBox->setRange(-1000, 1000);
  this->startYSpinBox->setSingleStep(0.001);
  this->startYSpinBox->setDecimals(3);
  this->startYSpinBox->setValue(0.000);
  this->startYSpinBox->setAlignment(Qt::AlignRight);
  QLabel *startYUnitLabel = new QLabel(tr("m"));
  startYUnitLabel->setMaximumWidth(40);

  QGridLayout *startXYLayout = new QGridLayout;
  startXYLayout->addWidget(startXLabel, 0, 0);
  startXYLayout->addWidget(this->startXSpinBox, 0, 1);
  startXYLayout->addWidget(startXUnitLabel, 0, 2);
  startXYLayout->addWidget(startYLabel, 1, 0);
  startXYLayout->addWidget(this->startYSpinBox, 1, 1);
  startXYLayout->addWidget(startYUnitLabel, 1, 2);

  QGroupBox *positionGroupBox = new QGroupBox(tr("Position"));
  positionGroupBox->setLayout(startXYLayout);

  QLabel *widthLabel = new QLabel(tr("Width: "));
  QLabel *depthLabel = new QLabel(tr("Depth: "));
  QLabel *heightLabel = new QLabel(tr("Height: "));

  this->widthSpinBox = new QDoubleSpinBox;
  this->widthSpinBox->setRange(0, 1000);
  this->widthSpinBox->setSingleStep(0.001);
  this->widthSpinBox->setDecimals(3);
  this->widthSpinBox->setValue(0.000);
  this->widthSpinBox->setAlignment(Qt::AlignRight);
  QLabel *widthUnitLabel = new QLabel(tr("m"));
  widthUnitLabel->setMaximumWidth(40);

  this->depthSpinBox = new QDoubleSpinBox;
  this->depthSpinBox->setRange(0, 1000);
  this->depthSpinBox->setSingleStep(0.001);
  this->depthSpinBox->setDecimals(3);
  this->depthSpinBox->setValue(0.000);
  this->depthSpinBox->setAlignment(Qt::AlignRight);
  QLabel *depthUnitLabel = new QLabel(tr("m"));
  depthUnitLabel->setMaximumWidth(40);

  this->heightSpinBox = new QDoubleSpinBox;
  this->heightSpinBox->setRange(0, 1000);
  this->heightSpinBox->setSingleStep(0.001);
  this->heightSpinBox->setDecimals(3);
  this->heightSpinBox->setValue(0.000);
  this->heightSpinBox->setAlignment(Qt::AlignRight);
  QLabel *heightUnitLabel = new QLabel(tr("m"));
  heightUnitLabel->setMaximumWidth(40);

  QLabel *stepsLabel = new QLabel(tr("# Steps: "));
  this->stepsSpinBox = new QSpinBox;
  this->stepsSpinBox->setRange(1, 1000);
  this->stepsSpinBox->setSingleStep(1);
  this->stepsSpinBox->setValue(1);
  this->stepsSpinBox->setAlignment(Qt::AlignRight);
  QLabel *stepsDummyLabel = new QLabel(tr(" "));

  QGridLayout *sizeLayout = new QGridLayout;
  sizeLayout->addWidget(widthLabel, 0, 0);
  sizeLayout->addWidget(this->widthSpinBox, 0, 1);
  sizeLayout->addWidget(widthUnitLabel, 0, 2);
  sizeLayout->addWidget(depthLabel, 1, 0);
  sizeLayout->addWidget(this->depthSpinBox, 1, 1);
  sizeLayout->addWidget(depthUnitLabel, 1, 2);
  sizeLayout->addWidget(heightLabel, 2, 0);
  sizeLayout->addWidget(this->heightSpinBox, 2, 1);
  sizeLayout->addWidget(heightUnitLabel, 2, 2);
  sizeLayout->addWidget(stepsLabel, 3, 0);
  sizeLayout->addWidget(this->stepsSpinBox, 3, 1);
  sizeLayout->addWidget(stepsDummyLabel, 3, 2);

  QGroupBox *sizeGroupBox = new QGroupBox(tr("Size"));
  sizeGroupBox->setLayout(sizeLayout);

  QLabel *colorLabel = new QLabel(tr("Color: "));
  this->colorComboBox = new QComboBox;
  this->colorComboBox->setIconSize(QSize(15, 15));
  this->colorComboBox->setMinimumWidth(50);
  this->colorComboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  QPixmap colorIcon(15, 15);
  this->colorList.push_back(QColor(255, 255, 255, 255));
  this->colorList.push_back(QColor(194, 169, 160, 255));
  this->colorList.push_back(QColor(235, 206, 157, 255));
  this->colorList.push_back(QColor(254, 121,   5, 255));
  this->colorList.push_back(QColor(255, 195,  78, 255));
  this->colorList.push_back(QColor(111, 203, 172, 255));
  for (unsigned int i = 0; i < this->colorList.size(); ++i)
  {
    colorIcon.fill(this->colorList.at(i));
    this->colorComboBox->addItem(colorIcon, QString(""));
  }

  QHBoxLayout *colorLayout = new QHBoxLayout;
  colorLayout->addWidget(colorLabel);
  colorLayout->addWidget(this->colorComboBox);

  QLabel *textureLabel = new QLabel(tr("Texture: "));
  this->textureComboBox = new QComboBox;
  this->textureComboBox->setIconSize(QSize(30, 30));
  this->textureComboBox->setMinimumWidth(50);
  this->textureComboBox->setMinimumHeight(50);
  this->textureComboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  this->textureList.push_back(":wood.jpg");
  this->textureList.push_back(":tiles.jpg");
  this->textureList.push_back(":bricks.png");
  for (unsigned int i = 0; i < this->textureList.size(); ++i)
  {
    this->textureComboBox->addItem(QPixmap(this->textureList[i]).scaled(
        QSize(90, 90), Qt::IgnoreAspectRatio), QString(""));
  }
  this->textureComboBox->addItem("X");
  this->textureComboBox->setCurrentIndex(this->textureComboBox->count()-1);

  QHBoxLayout *textureLayout = new QHBoxLayout;
  textureLayout->addWidget(textureLabel);
  textureLayout->addWidget(this->textureComboBox);

  QHBoxLayout *buttonsLayout = new QHBoxLayout;
  QPushButton *cancelButton = new QPushButton(tr("&Cancel"));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(OnCancel()));
  QPushButton *applyButton = new QPushButton(tr("&Apply"));
  connect(applyButton, SIGNAL(clicked()), this, SLOT(OnApply()));
  QPushButton *OKButton = new QPushButton(tr("&OK"));
  OKButton->setDefault(true);
  connect(OKButton, SIGNAL(clicked()), this, SLOT(OnOK()));
  buttonsLayout->addWidget(cancelButton);
  buttonsLayout->addWidget(applyButton);
  buttonsLayout->addWidget(OKButton);
  buttonsLayout->setAlignment(Qt::AlignRight);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(nameLayout);
  mainLayout->addWidget(positionGroupBox);
  mainLayout->addWidget(sizeGroupBox);
  mainLayout->addLayout(colorLayout);
  mainLayout->addLayout(textureLayout);
  mainLayout->addLayout(buttonsLayout);

  this->setLayout(mainLayout);
  this->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

/////////////////////////////////////////////////
StairsInspectorDialog::~StairsInspectorDialog()
{
}

/////////////////////////////////////////////////
QPointF StairsInspectorDialog::GetStartPosition() const
{
  return QPointF(this->startXSpinBox->value(),
      this->startYSpinBox->value());
}

/////////////////////////////////////////////////
double StairsInspectorDialog::GetWidth() const
{
  return this->widthSpinBox->value();
}

/////////////////////////////////////////////////
double StairsInspectorDialog::GetDepth() const
{
  return this->depthSpinBox->value();
}

/////////////////////////////////////////////////
double StairsInspectorDialog::GetHeight() const
{
  return this->heightSpinBox->value();
}

/////////////////////////////////////////////////
int StairsInspectorDialog::GetSteps() const
{
  return this->stepsSpinBox->value();
}

/////////////////////////////////////////////////
QColor StairsInspectorDialog::GetColor() const
{
  return this->colorList[this->colorComboBox->currentIndex()];
}

/////////////////////////////////////////////////
QString StairsInspectorDialog::GetTexture() const
{
  QString texture = QString("");
  if (this->textureComboBox->currentIndex() != -1 &&
      this->textureComboBox->currentIndex() <
      this->textureComboBox->count() - 1)
  {
    texture = this->textureList[this->textureComboBox->currentIndex()];
  }
  return texture;
}

/////////////////////////////////////////////////
void StairsInspectorDialog::SetName(const std::string &_name)
{
  this->stairsNameLabel->setText(tr(_name.c_str()));
}

/////////////////////////////////////////////////
void StairsInspectorDialog::SetStartPosition(const QPointF &_pos)
{
  this->startXSpinBox->setValue(_pos.x());
  this->startYSpinBox->setValue(_pos.y());
}

/////////////////////////////////////////////////
void StairsInspectorDialog::SetWidth(double _width)
{
  this->widthSpinBox->setValue(_width);
}

/////////////////////////////////////////////////
void StairsInspectorDialog::SetDepth(double _depth)
{
  this->depthSpinBox->setValue(_depth);
}


/////////////////////////////////////////////////
void StairsInspectorDialog::SetHeight(double _height)
{
  this->heightSpinBox->setValue(_height);
}

/////////////////////////////////////////////////
void StairsInspectorDialog::SetSteps(int _steps)
{
  this->stepsSpinBox->setValue(_steps);
}

/////////////////////////////////////////////////
void StairsInspectorDialog::SetColor(const QColor _color)
{
  int index = -1;
  for (unsigned int i = 0; i < this->colorList.size(); ++i)
  {
    if (this->colorList[i] == _color)
    {
      index = i;
      break;
    }
  }

  if (index == -1)
  {
    // Add a new color
    this->colorList.push_back(_color);
    QPixmap colorIcon(15, 15);
    colorIcon.fill(this->colorList.back());
    this->colorComboBox->addItem(colorIcon, QString(""));
    index = this->colorComboBox->count()-1;
  }
  GZ_ASSERT(index >= 0, "Color index is broken < 0");
  this->colorComboBox->setCurrentIndex(index);
}

/////////////////////////////////////////////////
void StairsInspectorDialog::SetTexture(QString _texture)
{
  // Find index corresponding to texture (only a few textures allowed so far)
  int index = this->textureComboBox->count()-1;
  for (unsigned int i = 0; i < this->textureList.size(); ++i)
  {
    if (this->textureList[i] == _texture)
    {
      index = i;
      break;
    }
  }
  this->textureComboBox->setCurrentIndex(index);
}

/////////////////////////////////////////////////
void StairsInspectorDialog::OnCancel()
{
  this->close();
}

/////////////////////////////////////////////////
void StairsInspectorDialog::OnApply()
{
  emit Applied();
}

/////////////////////////////////////////////////
void StairsInspectorDialog::OnOK()
{
  emit Applied();
  this->accept();
}
