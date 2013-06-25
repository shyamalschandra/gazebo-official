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

//#include "gazebo/rendering/Scene.hh"
//#include "gazebo/rendering/UserCamera.hh"

#include "gazebo/gui/Gui.hh"
//#include "gazebo/gui/MouseEventHandler.hh"
#include "gazebo/gui/GuiEvents.hh"
#include "gazebo/gui/model/ModelEditorPalette.hh"

using namespace gazebo;
using namespace gui;

/////////////////////////////////////////////////
ModelEditorPalette::ModelEditorPalette(QWidget *_parent)
    : QWidget(_parent)
{
  QVBoxLayout *mainLayout = new QVBoxLayout;

  this->modelTreeWidget = new QTreeWidget();
  this->modelTreeWidget->setColumnCount(1);
  this->modelTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
  this->modelTreeWidget->header()->hide();
  connect(this->modelTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
      this, SLOT(OnModelSelection(QTreeWidgetItem *, int)));

/*  QFrame *frame = new QFrame;
  QVBoxLayout *frameLayout = new QVBoxLayout;
  frameLayout->addWidget(this->modelTreeWidget, 0);
  frameLayout->setContentsMargins(0, 0, 0, 0);
  frame->setLayout(frameLayout);*/
  mainLayout->addWidget(this->modelTreeWidget);

  // Create a top-level tree item for the path
  this->modelSettingsItem =
    new QTreeWidgetItem(static_cast<QTreeWidgetItem*>(0),
        QStringList(QString("Model Settings")));
    this->modelTreeWidget->addTopLevelItem(this->modelSettingsItem);

  this->modelItem =
    new QTreeWidgetItem(static_cast<QTreeWidgetItem*>(0),
        QStringList(QString("Shapes and Joints")));
    this->modelTreeWidget->addTopLevelItem(this->modelItem);

  QTreeWidgetItem *modelChildItem =
    new QTreeWidgetItem(static_cast<QTreeWidgetItem*>(0));
    this->modelItem->addChild(modelChildItem);

  // Parts and joints buttons
  QWidget *modelWidget = new QWidget;
  QVBoxLayout *modelLayout = new QVBoxLayout;
  QGridLayout *partsLayout = new QGridLayout;
  QLabel *partsLabel = new QLabel(tr("Parts"));

  // cylinder button
  QPushButton *cylinderButton = new QPushButton(tr("Cylinder"), this);
  cylinderButton->setCheckable(true);
  cylinderButton->setChecked(false);
  connect(cylinderButton, SIGNAL(clicked()), this, SLOT(OnCylinder()));

  // Sphere button
  QPushButton *sphereButton = new QPushButton(tr("Sphere"), this);
  sphereButton->setCheckable(true);
  sphereButton->setChecked(false);
  connect(sphereButton, SIGNAL(clicked()), this, SLOT(OnSphere()));

  // Box button
  QPushButton *boxButton = new QPushButton(tr("Box"), this);
  boxButton->setCheckable(true);
  boxButton->setChecked(false);
  connect(boxButton, SIGNAL(clicked()), this, SLOT(OnBox()));

  QButtonGroup *partsButtonGroup = new QButtonGroup;
  partsButtonGroup->addButton(cylinderButton);
  partsButtonGroup->addButton(sphereButton);
  partsButtonGroup->addButton(boxButton);

  partsLayout->addWidget(partsLabel, 0, 0);
  partsLayout->addWidget(cylinderButton, 1, 0);
  partsLayout->addWidget(sphereButton, 1, 1);
  partsLayout->addWidget(boxButton, 1, 2);

  QGridLayout *jointsLayout = new QGridLayout;
  QLabel *jointsLabel = new QLabel(tr("Joints"));

  // Fixed joint button
  QPushButton *fixedJointButton = new QPushButton(tr("Fixed"), this);
  fixedJointButton->setCheckable(true);
  fixedJointButton->setChecked(false);
  connect(fixedJointButton, SIGNAL(clicked()), this, SLOT(OnFixedJoint()));

  // revolute joint button
  QPushButton *revoluteJointButton = new QPushButton(tr("Revolute"), this);
  revoluteJointButton->setCheckable(true);
  revoluteJointButton->setChecked(false);
  connect(revoluteJointButton, SIGNAL(clicked()), this,
      SLOT(OnRevoluteJoint()));

  // slider joint button
  QPushButton *sliderJointButton = new QPushButton(tr("Slider"), this);
  sliderJointButton->setCheckable(true);
  sliderJointButton->setChecked(false);
  connect(sliderJointButton, SIGNAL(clicked()), this, SLOT(OnSliderJoint()));

  // Hinge joint button
  QPushButton *hingeJointButton = new QPushButton(tr("Hinge"), this);
  hingeJointButton->setCheckable(true);
  hingeJointButton->setChecked(false);
  connect(hingeJointButton, SIGNAL(clicked()), this, SLOT(OnHingeJoint()));

  // Screw joint button
  QPushButton *screwJointButton = new QPushButton(tr("Screw"), this);
  screwJointButton->setCheckable(true);
  screwJointButton->setChecked(false);
  connect(screwJointButton, SIGNAL(clicked()), this, SLOT(OnScrewJoint()));

  // Universal joint button
  QPushButton *universalJointButton = new QPushButton(tr("Universal"), this);
  universalJointButton->setCheckable(true);
  universalJointButton->setChecked(false);
  connect(universalJointButton, SIGNAL(clicked()), this,
      SLOT(OnUniversalJoint()));

  QButtonGroup *jointsButtonGroup = new QButtonGroup;
  jointsButtonGroup->addButton(fixedJointButton);
  jointsButtonGroup->addButton(revoluteJointButton);
  jointsButtonGroup->addButton(sliderJointButton);
  jointsButtonGroup->addButton(hingeJointButton);
  jointsButtonGroup->addButton(screwJointButton);
  jointsButtonGroup->addButton(universalJointButton);

  jointsLayout->addWidget(jointsLabel, 0, 0);
  jointsLayout->addWidget(fixedJointButton, 1, 0);
  jointsLayout->addWidget(revoluteJointButton, 1, 1);
  jointsLayout->addWidget(sliderJointButton, 1, 2);
  jointsLayout->addWidget(hingeJointButton, 2, 0);
  jointsLayout->addWidget(screwJointButton, 2, 1);
  jointsLayout->addWidget(universalJointButton, 2, 2);

  modelLayout->addLayout(partsLayout);
  modelLayout->addLayout(jointsLayout);
  modelWidget->setLayout(modelLayout);
  this->modelTreeWidget->setItemWidget(modelChildItem, 0, modelWidget);


  //
  this->pluginItem =
    new QTreeWidgetItem(static_cast<QTreeWidgetItem*>(0),
        QStringList(QString("Plugin")));
    this->modelTreeWidget->addTopLevelItem(this->pluginItem);

  /*// Create the button to raise terrain
  QPushButton *raiseButton = new QPushButton("Raise", this);
  raiseButton->setStatusTip(tr("Left-mouse press to raise terrain."));
  raiseButton->setCheckable(true);
  raiseButton->setChecked(false);
  connect(raiseButton, SIGNAL(toggled(bool)), this, SLOT(OnRaise(bool)));

  // Create the button to lower terrain
  QPushButton *lowerButton = new QPushButton("Lower", this);
  lowerButton->setStatusTip(tr("Left-mouse press to lower terrain."));
  lowerButton->setCheckable(true);
  lowerButton->setChecked(false);
  connect(lowerButton, SIGNAL(toggled(bool)), this, SLOT(OnLower(bool)));

  // Create the button to flatten terrain
  QPushButton *flattenButton = new QPushButton("Flatten", this);
  flattenButton->setStatusTip(tr("Left-mouse press to flatten terrain."));
  flattenButton->setCheckable(true);
  flattenButton->setChecked(false);
  connect(flattenButton, SIGNAL(toggled(bool)), this, SLOT(OnFlatten(bool)));

  // Create the button to roughen terrain
  QPushButton *heightButton = new QPushButton("Pick Height", this);
  heightButton->setStatusTip(
      tr("Left-mouse press to select a terrain height."));
  heightButton->setCheckable(true);
  heightButton->setChecked(false);
  connect(heightButton, SIGNAL(toggled(bool)), this, SLOT(OnPickHeight(bool)));

  QButtonGroup *buttonGroup = new QButtonGroup;
  buttonGroup->addButton(raiseButton);
  buttonGroup->addButton(lowerButton);
  buttonGroup->addButton(flattenButton);
  buttonGroup->addButton(heightButton);

  // Create the layout to hold all the buttons
  QGridLayout *buttonLayout = new QGridLayout;
  buttonLayout->addWidget(raiseButton, 0, 0);
  buttonLayout->addWidget(lowerButton, 0, 1);
  buttonLayout->addWidget(flattenButton, 1, 0);
  buttonLayout->addWidget(heightButton, 1, 1);

  // Add a save button
  QPushButton *saveButton = new QPushButton("Save Image", this);
  saveButton->setStatusTip(tr("Save terrain as a PNG."));
  connect(saveButton, SIGNAL(clicked()), this, SLOT(OnSave()));

  // Create a slider to control the outer size of the brush
  this->outsideRadiusSlider = new QSlider(this);
  this->outsideRadiusSlider->setRange(1, 100000);
  this->outsideRadiusSlider->setTickInterval(1);
  this->outsideRadiusSlider->setOrientation(Qt::Horizontal);
  this->outsideRadiusSlider->setValue(10);
  connect(this->outsideRadiusSlider, SIGNAL(valueChanged(int)),
      this, SLOT(OnOutsideRadiusSlider(int)));

  this->outsideRadiusSpin = new QDoubleSpinBox(this);
  this->outsideRadiusSpin->setRange(0, 1.0);
  this->outsideRadiusSpin->setSingleStep(0.001);
  this->outsideRadiusSpin->setDecimals(3);
  connect(this->outsideRadiusSpin, SIGNAL(valueChanged(double)),
      this, SLOT(OnOutsideRadiusSpin(double)));

  // Create a layout to hold the outer brush size slider and its label
  QHBoxLayout *outsideRadiusSpinLayout = new QHBoxLayout;
  outsideRadiusSpinLayout->addWidget(new QLabel(tr("Outside radius: ")));
  outsideRadiusSpinLayout->addStretch(2);
  outsideRadiusSpinLayout->addWidget(this->outsideRadiusSpin);

  QVBoxLayout *outsideRadiusLayout = new QVBoxLayout;
  outsideRadiusLayout->addLayout(outsideRadiusSpinLayout);
  outsideRadiusLayout->addWidget(this->outsideRadiusSlider);


  // Create a slider to control the inner size of the brush
  this->insideRadiusSlider = new QSlider(this);
  this->insideRadiusSlider->setRange(0, 100000);
  this->insideRadiusSlider->setTickInterval(1);
  this->insideRadiusSlider->setOrientation(Qt::Horizontal);
  this->insideRadiusSlider->setValue(10);
  connect(this->insideRadiusSlider, SIGNAL(valueChanged(int)),
      this, SLOT(OnInsideRadiusSlider(int)));

  this->insideRadiusSpin = new QDoubleSpinBox(this);
  this->insideRadiusSpin->setRange(0, 1.0);
  this->insideRadiusSpin->setSingleStep(0.001);
  this->insideRadiusSpin->setDecimals(3);
  connect(this->insideRadiusSpin, SIGNAL(valueChanged(double)),
      this, SLOT(OnInsideRadiusSpin(double)));

  // Create a layout to hold the inner brush size slider and its label
  QHBoxLayout *insideRadiusSpinLayout = new QHBoxLayout;
  insideRadiusSpinLayout->addWidget(new QLabel(tr("Inside radius: ")));
  insideRadiusSpinLayout->addStretch(2);
  insideRadiusSpinLayout->addWidget(this->insideRadiusSpin);

  QVBoxLayout *insideRadiusLayout = new QVBoxLayout;
  insideRadiusLayout->addLayout(insideRadiusSpinLayout);
  insideRadiusLayout->addWidget(this->insideRadiusSlider);

  // Create a slider to control the weight of the brush
  this->weightSlider = new QSlider(this);
  this->weightSlider->setRange(1, 10000);
  this->weightSlider->setTickInterval(1);
  this->weightSlider->setOrientation(Qt::Horizontal);
  this->weightSlider->setValue(10);
  connect(this->weightSlider, SIGNAL(valueChanged(int)),
      this, SLOT(OnWeightSlider(int)));

  this->weightSpin = new QDoubleSpinBox(this);
  this->weightSpin->setRange(.01, 1.0);
  this->weightSpin->setSingleStep(.1);
  connect(this->weightSpin, SIGNAL(valueChanged(double)),
      this, SLOT(OnWeightSpin(double)));


  // Create a layout to hold the brush weight slider and its label
  QHBoxLayout *weightSpinLayout = new QHBoxLayout;
  weightSpinLayout->addWidget(new QLabel(tr("Weight: ")));
  weightSpinLayout->addStretch(2);
  weightSpinLayout->addWidget(this->weightSpin);

  QVBoxLayout *weightLayout = new QVBoxLayout;
  weightLayout->addLayout(weightSpinLayout);
  weightLayout->addWidget(this->weightSlider);


  // Create a slider to control the weight of the brush
  this->heightSlider = new QSlider(this);
  this->heightSlider->setRange(1, 100);
  this->heightSlider->setTickInterval(1);
  this->heightSlider->setOrientation(Qt::Horizontal);
  this->heightSlider->setValue(10);
  connect(this->heightSlider, SIGNAL(valueChanged(int)),
      this, SLOT(OnHeightSlider(int)));

  this->heightSpin = new QDoubleSpinBox(this);
  connect(this->heightSpin, SIGNAL(valueChanged(double)),
      this, SLOT(OnHeightSpin(double)));

  // Create a layout to hold the brush height slider and its label
  QHBoxLayout *heightSpinLayout = new QHBoxLayout;
  heightSpinLayout->addWidget(new QLabel(tr("Height: ")));
  heightSpinLayout->addStretch(2);
  heightSpinLayout->addWidget(this->heightSpin);

  QVBoxLayout *heightLayout = new QVBoxLayout;
  heightLayout->setContentsMargins(0, 0, 0, 0);
  heightLayout->addLayout(heightSpinLayout);
  heightLayout->addWidget(this->heightSlider);


  // Add all the layouts and widgets to the main layout
  mainLayout->addLayout(buttonLayout);
  mainLayout->addLayout(outsideRadiusLayout);
  mainLayout->addLayout(insideRadiusLayout);
  mainLayout->addLayout(weightLayout);
  mainLayout->addLayout(heightLayout);
  mainLayout->addStretch(1);
  mainLayout->addWidget(saveButton);*/

//  mainLayout->addWidget(frame);
  mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

  this->setObjectName("modelEditorPalette");
  this->setLayout(mainLayout);
  this->layout()->setContentsMargins(0, 0, 0, 0);
}

/////////////////////////////////////////////////
ModelEditorPalette::~ModelEditorPalette()
{
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnModelSelection(QTreeWidgetItem *_item,
                                         int /*_column*/)
{
  if (_item)
  {
   /* std::string path, filename;

    if (_item->parent())
      path = _item->parent()->text(0).toStdString() + "/";

    path = _item->data(0, Qt::UserRole).toString().toStdString();

    if (!path.empty())
    {
      //QApplication::setOverrideCursor(Qt::BusyCursor);
      //filename = common::ModelDatabase::Instance()->GetModelFile(path);
      //gui::Events::createEntity("model", filename);

      //this->fileTreeWidget->clearSelection();
      //QApplication::setOverrideCursor(Qt::ArrowCursor);
    }*/
  }
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnCylinder()
{
  gui::Events::createEntity("cylinder", "");
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnSphere()
{
  gui::Events::createEntity("sphere", "");
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnBox()
{
  gui::Events::createEntity("box", "");
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnFixedJoint()
{
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnRevoluteJoint()
{
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnSliderJoint()
{
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnHingeJoint()
{
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnScrewJoint()
{
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnUniversalJoint()
{
}



/*
/////////////////////////////////////////////////
void ModelEditorPalette::SetState(const std::string &_state)
{
  if (!_state.empty())
  {
    this->state = _state;

    // Add an event filter, which allows the TerrainEditor to capture
    // mouse events.
    MouseEventHandler::Instance()->AddPressFilter("terrain",
        boost::bind(&ModelEditorPalette::OnMousePress, this, _1));

    MouseEventHandler::Instance()->AddMoveFilter("terrain",
        boost::bind(&ModelEditorPalette::OnMouseMove, this, _1));
  }
  else
  {
    this->state.clear();

    // Remove the event filters.
    MouseEventHandler::Instance()->RemovePressFilter("terrain");
    MouseEventHandler::Instance()->RemoveMoveFilter("terrain");
  }
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnSave()
{
  // Get the active camera and scene.
  rendering::UserCameraPtr camera = gui::get_active_camera();
  rendering::ScenePtr scene = camera->GetScene();

  // Get a pointer to the heightmap, if the scen is valid.
  rendering::Heightmap *heightmap = scene ? scene->GetHeightmap() : NULL;
  common::Image img = heightmap->GetImage();

  // Get a filename to save to.
  std::string filename = QFileDialog::getSaveFileName(this,
      tr("Save Heightmap"), QString(),
      tr("PNG Files (*.png)")).toStdString();

  // Return if the user has canceled.
  if (filename.empty())
    return;

  img.SavePNG(filename);
}

/////////////////////////////////////////////////
bool ModelEditorPalette::OnMousePress(const common::MouseEvent &_event)
{
  if (_event.button != common::MouseEvent::LEFT)
    return false;

  bool handled = false;

  // Get the active camera and scene.
  rendering::UserCameraPtr camera = gui::get_active_camera();
  rendering::ScenePtr scene = camera->GetScene();

  // Get a pointer to the heightmap, if the scen is valid.
  rendering::Heightmap *heightmap = scene ? scene->GetHeightmap() : NULL;

  // Only try to modify if the heightmap exists, and the LEFT mouse button
  // was used.
  if (heightmap && !_event.shift)
  {
    handled = this->Apply(_event, camera, heightmap);
    QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
  }
  else
    QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));

  return handled;
}

/////////////////////////////////////////////////
bool ModelEditorPalette::OnMouseMove(const common::MouseEvent &_event)
{
  if (_event.button != common::MouseEvent::LEFT)
    return false;

  bool handled = false;

  // Get the active camera and scene.
  rendering::UserCameraPtr camera = gui::get_active_camera();
  rendering::ScenePtr scene = camera->GetScene();

  // Get a pointer to the heightmap, if the scen is valid.
  rendering::Heightmap *heightmap = scene ? scene->GetHeightmap() : NULL;

  if (heightmap && !_event.shift)
  {
    if (_event.dragging)
      handled = this->Apply(_event, camera, heightmap);
    QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
  }
  else
    QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));

  return handled;
}

/////////////////////////////////////////////////
bool ModelEditorPalette::Apply(const common::MouseEvent &_event,
    rendering::CameraPtr _camera, rendering::Heightmap *_heightmap)
{
  bool handled = false;

  // Get the brush weight and size from the sliders.
  double weight = this->weightSpin->value();
  double outsideRadius = this->outsideRadiusSpin->value() / 10.0;
  double insideRadius = this->insideRadiusSpin->value() / 10.0;

  if (this->state == "lower")
    handled = _heightmap->Lower(_camera, _event.pos, outsideRadius,
        insideRadius, weight);
  else if (this->state == "raise")
    handled = _heightmap->Raise(_camera, _event.pos, outsideRadius,
        insideRadius, weight);
  else if (this->state == "flatten")
    handled = _heightmap->Flatten(_camera, _event.pos, outsideRadius,
        insideRadius, weight);
  else if (this->state == "smooth")
    handled = _heightmap->Smooth(_camera, _event.pos, outsideRadius,
        insideRadius, weight);

  return handled;
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnOutsideRadiusSpin(double _value)
{
  disconnect(this->outsideRadiusSlider, SIGNAL(valueChanged(int)),
      this, SLOT(OnOutsideRadiusSlider(int)));

  this->outsideRadiusSlider->setValue(
      static_cast<int>(rint(_value * this->outsideRadiusSlider->maximum())));

  connect(this->outsideRadiusSlider, SIGNAL(valueChanged(int)),
      this, SLOT(OnOutsideRadiusSlider(int)));
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnOutsideRadiusSlider(int _value)
{
  disconnect(this->outsideRadiusSpin, SIGNAL(valueChanged(double)),
      this, SLOT(OnOutsideRadiusSpin(double)));

  this->outsideRadiusSpin->setValue(_value /
      static_cast<double>(this->outsideRadiusSlider->maximum()));

  connect(this->outsideRadiusSpin, SIGNAL(valueChanged(double)),
      this, SLOT(OnOutsideRadiusSpin(double)));
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnInsideRadiusSpin(double _value)
{
  disconnect(this->insideRadiusSlider, SIGNAL(valueChanged(int)),
      this, SLOT(OnInsideRadiusSlider(int)));

  this->insideRadiusSlider->setValue(
      static_cast<int>(rint(_value * this->insideRadiusSlider->maximum())));

  connect(this->insideRadiusSlider, SIGNAL(valueChanged(int)),
      this, SLOT(OnInsideRadiusSlider(int)));
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnInsideRadiusSlider(int _value)
{
  disconnect(this->insideRadiusSpin, SIGNAL(valueChanged(double)),
      this, SLOT(OnInsideRadiusSpin(double)));


  this->insideRadiusSpin->setValue(_value /
      static_cast<double>(this->insideRadiusSlider->maximum()));

  connect(this->insideRadiusSpin, SIGNAL(valueChanged(double)),
      this, SLOT(OnInsideRadiusSpin(double)));
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnWeightSpin(double _value)
{
  disconnect(this->weightSlider, SIGNAL(valueChanged(int)),
      this, SLOT(OnWeightSlider(int)));

  this->weightSlider->setValue(
      static_cast<int>(rint(_value * this->weightSlider->maximum())));

  connect(this->weightSlider, SIGNAL(valueChanged(int)),
      this, SLOT(OnWeightSlider(int)));
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnWeightSlider(int _value)
{
  disconnect(this->weightSpin, SIGNAL(valueChanged(double)),
             this, SLOT(OnWeightSpin(double)));

  this->weightSpin->setValue(_value /
      static_cast<double>(this->weightSlider->maximum()));

  connect(this->weightSpin, SIGNAL(valueChanged(double)),
      this, SLOT(OnWeightSpin(double)));
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnHeightSpin(double _value)
{
  disconnect(this->heightSlider, SIGNAL(valueChanged(int)),
      this, SLOT(OnHeightSlider(int)));

  this->heightSlider->setValue(static_cast<int>(rint(_value)));

  connect(this->heightSlider, SIGNAL(valueChanged(int)),
      this, SLOT(OnHeightSlider(int)));
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnHeightSlider(int _value)
{
  disconnect(this->heightSpin, SIGNAL(valueChanged(double)),
      this, SLOT(OnHeightSpin(double)));

  this->heightSpin->setValue(_value / 10.0);

  connect(this->heightSpin, SIGNAL(valueChanged(double)),
      this, SLOT(OnHeightSpin(double)));
}*/
