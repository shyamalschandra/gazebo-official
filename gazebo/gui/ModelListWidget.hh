/*
 * Copyright 2011 Nate Koenig
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
#ifndef MODEL_LIST_WIDGET_HH
#define MODEL_LIST_WIDGET_HH

#include <string>
#include <list>
#include <vector>

#include "gui/qt.h"
#include "sdf/sdf.hh"
#include "msgs/msgs.hh"
#include "transport/TransportTypes.hh"
#include "rendering/RenderTypes.hh"

class QTreeWidget;
class QTreeWidgetItem;
class QPushButton;
class QtTreePropertyBrowser;
class QtVariantPropertyManager;
class QtProperty;
class QtTreePropertyItem;
class QtBrowserItem;
class QtVariantEditorFactory;

namespace boost
{
  class recursive_mutex;
  class mutex;
}

namespace gazebo
{
  namespace gui
  {
    class ModelEditWidget;

    class ModelListWidget : public QWidget
    {
      Q_OBJECT
      public: ModelListWidget(QWidget *_parent = 0);
      public: virtual ~ModelListWidget();

      private slots: void OnModelSelection(QTreeWidgetItem *item, int column);
      private slots: void Update();
      private slots: void OnPropertyChanged(QtProperty *_item);
      private slots: void OnCustomContextMenu(const QPoint &_pt);
      private slots: void OnCurrentPropertyChanged(QtBrowserItem *_item);
      private: void OnSetSelectedEntity(const std::string &_name);
      private: void OnResponse(ConstResponsePtr &_msg);

      private: void OnModelUpdate(const msgs::Model &_msg);
      private: void OnRequest(ConstRequestPtr &_msg);

      private: void OnPose(ConstPosePtr &_msg);

      private: void OnRemoveScene(const std::string &_name);
      private: void OnCreateScene(const std::string &_name);

      private: void AddModelToList(const msgs::Model &_msg);

      private: void FillMsgField(QtProperty *_item,
                   google::protobuf::Message *_message,
                   const google::protobuf::Reflection *_reflection,
                   const google::protobuf::FieldDescriptor *_field);

      private: void FillMsg(QtProperty *_item,
                   google::protobuf::Message *_message,
                   const google::protobuf::Descriptor *_descriptor,
                   QtProperty *_changedItem);

      private: void FillGeometryMsg(QtProperty *_item,
                   google::protobuf::Message *_message,
                   const google::protobuf::Descriptor *_descriptor,
                   QtProperty *_changedItem);

      private: void FillPoseMsg(QtProperty *_item,
                   google::protobuf::Message *_message,
                   const google::protobuf::Descriptor *_descriptor);

      private: QtProperty *PopChildItem(QList<QtProperty*> &_list,
                                        const std::string &_name);

      private: QtProperty *GetParentItemValue(const std::string &_name);
      private: QtProperty *GetParentItemValue(QtProperty *_item,
                                           const std::string &_name);

      private: QtProperty *GetParentItem(const std::string &_name);
      private: QtProperty *GetParentItem(QtProperty *_item,
                                           const std::string &_name);

      private: QtProperty *GetChildItemValue(const std::string &_name);
      private: QtProperty *GetChildItemValue(QtProperty *_item,
                                             const std::string &_name);

      private: QtProperty *GetChildItem(const std::string &_name);
      private: QtProperty *GetChildItem(QtProperty *_item,
                                        const std::string &_name);

      private: bool HasChildItem(QtProperty *_parent, QtProperty *_child);

      private: void RemoveEntity(const std::string &_name);

      private: QTreeWidgetItem *GetModelListItem(const std::string &_name);

      private: void FillPropertyTree(const msgs::Model &_msg,
                                     QtProperty *_parentItem);

      private: void FillPropertyTree(const msgs::Link &_msg,
                                     QtProperty *_parent);

      private: void FillPropertyTree(const msgs::Collision &_msg,
                                     QtProperty *_parent);

      private: void FillPropertyTree(const msgs::Joint &_msg,
                                     QtProperty *_parent);

      private: void FillVector3dProperty(const msgs::Vector3d &_msg,
                                         QtProperty *_parent);

      private: void FillPoseProperty(const msgs::Pose &_msg,
                                     QtProperty *_parent);

      private: void FillPropertyTree(const msgs::Surface &_msg,
                                       QtProperty *_parent);

      private: void FillPropertyTree(const msgs::Visual &_msg,
                                       QtProperty *_parent);

      private: void FillPropertyTree(const msgs::Geometry &_msg,
                                       QtProperty *_parent);

      private: void FillPropertyTree(const msgs::Scene &_msg,
                                     QtProperty *_parent);

      private: void ProcessPoseMsgs();
      private: void ProcessModelMsgs();

      public: void InitTransport(const std::string &_name ="");
      private: void ResetTree();
      private: void ResetScene();

      private: QTreeWidget *modelTreeWidget;
      private: QtTreePropertyBrowser *propTreeBrowser;

      private: transport::NodePtr node;
      private: transport::PublisherPtr requestPub, modelPub;
      private: transport::SubscriberPtr responseSub;
      private: transport::SubscriberPtr requestSub;
      private: transport::SubscriberPtr poseSub;

      private: QTreeWidgetItem *sceneItem;
      private: QTreeWidgetItem *physicsItem;
      private: QTreeWidgetItem *modelsItem;
      private: QTreeWidgetItem *lightsItem;

      private: QtVariantPropertyManager *variantManager;
      private: QtVariantEditorFactory *variantFactory;
      private: boost::mutex *propMutex, *receiveMutex;
      private: sdf::ElementPtr sdfElement;
      private: std::string selectedModelName;
      private: bool fillingPropertyTree;
      private: QtProperty *selectedProperty;

      private: msgs::Request *requestMsg;

      private: std::vector<event::ConnectionPtr> connections;

      typedef std::list<boost::shared_ptr<msgs::Pose const> > PoseMsgs_L;
      private: PoseMsgs_L poseMsgs;

      typedef std::list<msgs::Model> ModelMsgs_L;
      private: ModelMsgs_L modelMsgs;

      private: msgs::Model modelMsg;
      private: msgs::Link linkMsg;
      private: msgs::Scene sceneMsg;
      private: msgs::Joint jointMsg;

      private: bool fillPropertyTree;
      private: std::string fillType;
    };

    class ModelListSheetDelegate: public QItemDelegate
    {
      Q_OBJECT
      public: ModelListSheetDelegate(QTreeView *view, QWidget *parent);

      public: virtual void paint(QPainter *painter,
                  const QStyleOptionViewItem &option,
                  const QModelIndex &index) const;

      public: virtual QSize sizeHint(const QStyleOptionViewItem &opt,
                  const QModelIndex &index) const;

      private: QTreeView *m_view;
    };
  }
}
#endif
