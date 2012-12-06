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

#include "EditorView.hh"
#include "RectItem.hh"
#include "GridLines.hh"
#include "PolylineItem.hh"

using namespace gazebo;
using namespace gui;

/////////////////////////////////////////////////
EditorView::EditorView(QWidget *_parent)
  : QGraphicsView(_parent)
{
  this->setObjectName("editorView");

  this->drawMode = None;
  this->drawInProgress = false;
}

/////////////////////////////////////////////////
EditorView::~EditorView()
{

}

/////////////////////////////////////////////////
void EditorView::mousePressEvent(QMouseEvent *_event)
{
  QGraphicsView::mousePressEvent(_event);
}

/////////////////////////////////////////////////
void EditorView::mouseReleaseEvent(QMouseEvent *_event)
{
  /// TODO for testing, delete me later
  if (_event->button() == Qt::RightButton)
  {
    if (this->drawMode == Wall)
    {
      qDebug() << "set draw mode to none";
      this->drawMode = None;
    }
    else {
      qDebug() << "set draw mode to wall";
      this->drawMode = Wall;
    }
    return;
  }

  switch (drawMode)
  {
    case None:
      break;
    case Wall:
      if (!drawInProgress)
      {
        this->DrawLine(_event->pos());
      }
      else
      {
        lineList.back()->AddPoint(this->mapToScene(_event->pos()));
      }
      break;
    default:
      break;
  }

  if (drawMode != None && !drawInProgress)
    drawInProgress = true;

  QGraphicsView::mouseReleaseEvent(_event);
}

/////////////////////////////////////////////////
void EditorView::mouseMoveEvent(QMouseEvent *_event)
{
  QPointF trans = _event->pos() - lastMousePos;
  switch (drawMode)
  {
    case None:
      break;
    case Wall:
      if (drawInProgress)
      {
        if (!lineList.empty())
        {
          lineList.back()->TranslateVertex(lineList.back()->GetCount() - 1,
              trans);
        }
      }
      break;
    default:
      break;
  }
  lastMousePos = _event->pos();
  QGraphicsView::mouseMoveEvent(_event);
}

/////////////////////////////////////////////////
void EditorView::mouseDoubleClickEvent(QMouseEvent *_event)
{
  drawMode =  None;
  drawInProgress = false;

  QGraphicsView::mouseDoubleClickEvent(_event);
}

/////////////////////////////////////////////////
void EditorView::DrawLine(QPoint _pos)
{
  QPointF pointStart = mapToScene(_pos);
  QPointF pointEnd = pointStart;

  qDebug() << " point start " << pointStart;

  PolylineItem* line = new PolylineItem(pointStart, pointEnd);
  scene()->addItem(line);
  lineList.push_back(line);

  lastLineCornerPos = _pos;
  lastMousePos = _pos;
}
