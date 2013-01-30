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
#ifndef _DATALOGGER_HH_
#define _DATALOGGER_HH_

#include "gazebo/transport/TransportTypes.hh"
#include "gazebo/msgs/msgs.hh"
#include "gazebo/gui/qt.h"

namespace gazebo
{
  namespace gui
  {
    /// \addtogroup gazebo_gui
    /// \{

    /// \class DataLogger DataLogger.hh gui/DataLogger.hh
    /// \brief A widget that provides data logging functionality.
    class DataLogger : public QDialog
    {
      Q_OBJECT

      /// \brief Constructor.
      /// \param[in] _parent Parent widget pointer.
      public: DataLogger(QWidget *_parent = 0);

      /// \brief Destructor.
      public: virtual ~DataLogger();

      /// \brief A signal used to set the time label.
      /// \param[in] _string String representation of time.
      signals: void SetTime(QString _string);

      /// \brief A signal used to set the size label.
      /// \param[in] _string String representation of size.
      signals: void SetSize(QString _string);

      /// \brief A signal used to set the filename.
      /// \param[in] _string The log filename
      signals: void SetFilename(QString _string);

      /// \brief A signal used to set the destination label.
      /// \param[in] _string The log destination directory
      signals: void SetDestination(QString _string);

      /// \brief QT callback for the record button.
      private slots: void OnRecord(bool _toggle);

      /// \brief QT callback for setting the filename.
      private slots: void OnSetFilename(QString _string);

      /// \brief QT callback for setting the destination label.
      private slots: void OnSetDestination(QString _string);

      /// \brief QT callback for setting the log path.
      private slots: void OnBrowse();

      /// \brief QT callback for toggling the settings visibility.
      private slots: void OnToggleSettings(bool _checked);

      /// \brief Callback for log status messages.
      /// \param[in] _msg Log status message.
      private: void OnStatus(ConstLogStatusPtr &_msg);

      /// \brief Node to handle communication.
      private: transport::NodePtr node;

      /// \brief Publisher for log control messages.
      private: transport::PublisherPtr pub;

      /// \brief Subscriber for log status messages.
      private: transport::SubscriberPtr sub;

      /// \brief The button used to start and pause logging.
      private: QToolButton *recordButton;

      /// \brief The button used to show/hide the settings frame.
      private: QPushButton *settingExpandButton;

      /// \brief The button used to stop logging.
      private: QToolButton *stopButton;

      /// \brief Label to display the log time.
      private: QLabel *timeLabel;

      /// \brief Label to display the log destination.
      private: QLabel *destLabel;

      /// \brief Label to display the log file size.
      private: QLabel *sizeLabel;

      /// \brief Label to display status information.
      private: QLabel *statusLabel;

      /// \brief Name of the log file path
      private: QLineEdit *filenameEdit;

      /// \brief Frame that holds settings.
      private: QFrame *settingsFrame;

      private: bool recording;
      private: bool paused;
    };
    /// \}
  }
}
#endif
