
#include "gazebo/gui/OculusWindow.hh"
#include "gazebo/rendering/OculusCamera.hh"
#include "gazebo/rendering/RenderEngine.hh"
#include "gazebo/rendering/RenderingIface.hh"
#include "gazebo/rendering/Scene.hh"
#include "gazebo/rendering/Visual.hh"
#include "gazebo/rendering/WindowManager.hh"

using namespace gazebo;
using namespace gui;

/////////////////////////////////////////////////
OculusWindow::OculusWindow(int _x, int _y, const std::string &_visual,
    QWidget *_parent)
  : QWidget(_parent)
{
  this->xPos = _x;
  this->yPos = _y;
  this->visualName = _visual;

  setAttribute(Qt::WA_NativeWindow, true);
  setAttribute(Qt::WA_OpaquePaintEvent, true);
  setAttribute(Qt::WA_PaintOnScreen, true);

  this->windowId = -1;

  this->setObjectName("oculusWindow");

  this->setWindowIcon(QIcon(":/images/gazebo.svg"));
  this->setWindowTitle(tr("Gazebo: Oculus"));

  this->renderFrame = new QFrame;
  this->renderFrame->setFrameShape(QFrame::NoFrame);
  this->renderFrame->setSizePolicy(QSizePolicy::Expanding,
                                   QSizePolicy::Expanding);
  this->renderFrame->setContentsMargins(0, 0, 0, 0);
  this->renderFrame->show();

  QVBoxLayout *renderLayout = new QVBoxLayout;
  renderLayout->addWidget(this->renderFrame);
  renderLayout->setContentsMargins(0, 0, 0, 0);

  this->setLayout(renderLayout);
  this->isFullScreen = false;
}

/////////////////////////////////////////////////
OculusWindow::~OculusWindow()
{
  this->attachCameraThread->join();
  delete this->attachCameraThread;
  this->oculusCamera.reset();
}

/////////////////////////////////////////////////
void OculusWindow::keyPressEvent(QKeyEvent *_event)
{
  // Toggle full screen
  if (_event->key() == Qt::Key_F11)
  {
    if (this->isFullScreen)
      this->showFullScreen();
    else
      this->showNormal();

    this->isFullScreen = !this->isFullScreen;
  }
}

/////////////////////////////////////////////////
void OculusWindow::resizeEvent(QResizeEvent *_e)
{
  if (!this->scene)
    return;

  if (this->windowId >= 0)
  {
    rendering::RenderEngine::Instance()->GetWindowManager()->Resize(
        this->windowId, _e->size().width(), _e->size().height());
    this->oculusCamera->Resize(_e->size().width(), _e->size().height());
  }
}

/////////////////////////////////////////////////
void OculusWindow::AttachCameraToVisual()
{
  if (!this->scene)
  {
    gzerr << "Scene is NULL!" << std::endl;
    return;
  }
  while (!this->scene->GetVisual(this->visualName))
  {
    common::Time::MSleep(100);
  }

  this->oculusCamera->AttachToVisual(this->visualName, true);

  math::Vector3 camPos(0.1, 0, 0);
  math::Vector3 lookAt(0, 0, 0);
  math::Vector3 delta = lookAt - camPos;

  double yaw = atan2(delta.y, delta.x);

  double pitch = atan2(-delta.z, sqrt(delta.x*delta.x + delta.y*delta.y));

  this->oculusCamera->SetWorldPose(math::Pose(
        camPos, math::Vector3(0, pitch, yaw)));
}

/////////////////////////////////////////////////
void OculusWindow::showEvent(QShowEvent *_event)
{
  std::cout << "showEvent" << std::endl;
  this->scene = rendering::get_scene();

  if (!this->oculusCamera)
  {
    this->oculusCamera = this->scene->CreateOculusCamera("gzoculus_camera");

    this->attachCameraThread = new boost::thread(
        boost::bind(&OculusWindow::AttachCameraToVisual, this));
  }
  //this->oculusCamera->AttachToVisual(this->visualName, true);

  if (this->windowId == -1)
  {
    this->windowId = rendering::RenderEngine::Instance()->GetWindowManager()->
      CreateWindow(this->GetOgreHandle(), this->width(), this->height());
    if (this->oculusCamera)
      rendering::RenderEngine::Instance()->GetWindowManager()->SetCamera(
          this->windowId, this->oculusCamera);
  }

  QWidget::showEvent(_event);



  this->setFocus();

  QSize winSize;
  winSize.setWidth(1280);
  winSize.setHeight(800);
  this->resize(winSize);

  // Put the window on the ocuclus screen
  this->setGeometry(this->xPos, this->yPos, 1280, 800);

  // Make the window full screen
  this->isFullScreen = true;
  this->showFullScreen();
}

//////////////////////////////////////////////////
std::string OculusWindow::GetOgreHandle() const
{
  std::string ogreHandle;

#if defined(WIN32) || defined(__APPLE__)
  ogreHandle = boost::lexical_cast<std::string>(this->winId());
#else
  QX11Info info = x11Info();
  QWidget *q_parent = dynamic_cast<QWidget*>(this->renderFrame);
  ogreHandle = boost::lexical_cast<std::string>(
      reinterpret_cast<uint64_t>(info.display()));
  ogreHandle += ":";
  ogreHandle += boost::lexical_cast<std::string>(
      static_cast<uint32_t>(info.screen()));
  ogreHandle += ":";
  assert(q_parent);
  ogreHandle += boost::lexical_cast<std::string>(
      static_cast<uint64_t>(q_parent->winId()));
#endif

  return ogreHandle;
}
