#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <qmainwindow.h>

namespace gazebo
{
  namespace gui
  {
    class RenderWidget;
    class TimePanel;
    class GLWidget;

    class MainWindow : public QMainWindow
    {
      Q_OBJECT

      public: MainWindow();
      public: virtual ~MainWindow();

      public: void Load( const std::string &config );
      public: void Init();

      protected: void closeEvent(QCloseEvent *event);

      private slots: void New();
      private slots: void Open();
      private slots: void Save();
      private slots: void About();
      private slots: void Play();
      private slots: void Pause();
      private slots: void Step();

      private slots: void CreateBox();
      private slots: void CreateSphere();
      private slots: void CreateCylinder();
      private slots: void CreatePointLight();
      private slots: void CreateSpotLight();
      private slots: void CreateDirectionalLight();

      private: void CreateActions();
      private: void CreateMenus();
      private: void CreateToolbars();

      private: QMenu *fileMenu;
      private: QMenu *viewMenu;
      private: QMenu *helpMenu;
      private: QToolBar *playToolbar;
      private: QToolBar *editToolbar;

      private: QAction *newAct;
      private: QAction *openAct;
      private: QAction *saveAct;
      private: QAction *aboutAct;
      private: QAction *quitAct;

      private: QAction *playAct;
      private: QAction *pauseAct;
      private: QAction *stepAct;

      private: QAction *boxCreateAct;
      private: QAction *sphereCreateAct;
      private: QAction *cylinderCreateAct;
      private: QAction *pointLghtCreateAct;
      private: QAction *spotLghtCreateAct;
      private: QAction *dirLghtCreateAct;

      private: TimePanel *timePanel;
      private: RenderWidget *glWidget;
    };
  }
}

#endif
