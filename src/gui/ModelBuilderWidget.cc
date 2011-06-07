#include <QtGui>

#include "rendering/Rendering.hh"
#include "rendering/UserCamera.hh"

#include "physics/World.hh"
#include "physics/Physics.hh"

#include "transport/Node.hh"
#include "transport/Publisher.hh"

#include "gui/GLWidget.hh"

#include "gui/ModelBuilderWidget.hh"

using namespace gazebo;
using namespace gui;

ModelBuilderWidget::ModelBuilderWidget( QWidget *parent )
  : QWidget(parent)
{
  QVBoxLayout *mainLayout = new QVBoxLayout;

  QFrame *renderFrame = new QFrame;
  renderFrame->setLineWidth(1);
  renderFrame->setFrameShadow(QFrame::Sunken);
  renderFrame->setFrameShape(QFrame::Box);
  renderFrame->show();

  QVBoxLayout *frameLayout = new QVBoxLayout;

  this->glWidget = new GLWidget(renderFrame);
  rendering::ScenePtr scene = rendering::create_scene("model_builder");
  this->glWidget->ViewScene( scene );

  // Add another widget that will hold body information.
  frameLayout->addWidget(this->glWidget);
  renderFrame->setLayout(frameLayout);
  renderFrame->layout()->setContentsMargins(4,4,4,4);

  QToolBar *toolbar = new QToolBar(this);

  this->boxCreateAct = new QAction(QIcon(":/images/box.png"), tr("Box"), this);
  this->boxCreateAct->setStatusTip(tr("Create a box"));
  connect(this->boxCreateAct, SIGNAL(triggered()), this, SLOT(CreateBox()));
  toolbar->addAction(boxCreateAct);

  this->sphereCreateAct = new QAction(QIcon(":/images/sphere.png"), tr("Sphere"), this);
  this->sphereCreateAct->setStatusTip(tr("Create a sphere"));
  connect(this->sphereCreateAct, SIGNAL(triggered()), this, SLOT(CreateSphere()));
  toolbar->addAction(sphereCreateAct);

  this->cylinderCreateAct = new QAction(QIcon(":/images/cylinder.png"), tr("Cylinder"), this);
  this->cylinderCreateAct->setStatusTip(tr("Create a sphere"));
  connect(this->cylinderCreateAct, SIGNAL(triggered()), this, SLOT(CreateCylinder()));
  toolbar->addAction(cylinderCreateAct);

  mainLayout->addWidget(toolbar);
  mainLayout->addWidget(renderFrame);

  this->setLayout(mainLayout);
  this->layout()->setContentsMargins(0,0,0,0);

  // TODO: Use messages so that the gui doesn't depend upon physics 
  physics::init();
  this->world = physics::create_world("model_builder");
  this->world->Load(NULL);
  this->world->Init();
  this->world->SetPaused(true);
  
  msgs::Factory msg;
  common::Message::Init(msg, "new_box");
  std::ostringstream newModelStr;

  newModelStr << "<?xml version='1.0'?>";

  newModelStr << "<model name='my_new_model'>\
    <static>true</static>\
    <origin xyz='0 0 0'/>\
    <link name='body'>\
      <collision name='geom'>\
        <geometry>\
          <box size='1 1 1'/>\
        </geometry>\
        <mass>0.5</mass>\
      </collision>\
      <visual>\
        <geometry>\
          <box size='1 1 1'/>\
        </geometry>\
        <material name='Gazebo/Grey'/>\
        <cast_shadows>true</cast_shadows>\
        <shader>pixel</shader>\
      </visual>\
    </link>\
  </model>";

  msg.set_xml( newModelStr.str() );

  this->node = transport::NodePtr(new transport::Node());
  this->node->Init("model_builder");

  this->factoryPub = this->node->Advertise<msgs::Factory>("~/factory");
  this->factoryPub->Publish(msg);
}

ModelBuilderWidget::~ModelBuilderWidget()
{
  delete glWidget;
}

void ModelBuilderWidget::Init()
{
  this->glWidget->show();
}

void ModelBuilderWidget::CreateBox()
{
  this->glWidget->CreateEntity("box", 
      boost::bind(&ModelBuilderWidget::OnBoxCreate, this, _1, _2) );
}

void ModelBuilderWidget::CreateSphere()
{
  this->glWidget->CreateEntity("sphere",
      boost::bind(&ModelBuilderWidget::OnSphereCreate, this, _1, _2) );
}

void ModelBuilderWidget::CreateCylinder()
{
  this->glWidget->CreateEntity("cylinder",
      boost::bind(&ModelBuilderWidget::OnCylinderCreate, this, _1, _2) );
}

void ModelBuilderWidget::OnBoxCreate(const common::Vector3 &pos,  
                                     const common::Vector3 &scale)
{
  gzdbg << "Box Create[" << pos << "] [" << scale << "]\n";
}

void ModelBuilderWidget::OnSphereCreate(const common::Vector3 &pos,  
                                     const common::Vector3 &scale)
{
  gzdbg << "Sphere Create[" << pos << "] [" << scale << "]\n";
}

void ModelBuilderWidget::OnCylinderCreate(const common::Vector3 &pos,  
                                     const common::Vector3 &scale)
{
  gzdbg << "Cylinder Create[" << pos << "] [" << scale << "]\n";
}
