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

#include "gazebo/gui/qwt/qwt_plot.h"
#include "gazebo/gui/qwt/qwt_plot_panner.h"
#include "gazebo/gui/qwt/qwt_plot_magnifier.h"
#include "gazebo/gui/qwt/qwt_plot_layout.h"
#include "gazebo/gui/qwt/qwt_plot_grid.h"
#include "gazebo/gui/qwt/qwt_plot_canvas.h"
#include "gazebo/gui/qwt/qwt_plot_curve.h"
#include "gazebo/gui/qwt/qwt_curve_fitter.h"
#include "gazebo/gui/qwt/qwt_symbol.h"
#include "gazebo/gui/qwt/qwt_plot_directpainter.h"

#include "gazebo/math/Helpers.hh"
#include "gazebo/gui/IncrementalPlot.hh"

using namespace gazebo;
using namespace gui;


class CurveData: public QwtArraySeriesData<QPointF>
{
  public: CurveData()
          {}

  public: virtual QRectF boundingRect() const
          {
            if (this->d_boundingRect.width() < 0.0)
              this->d_boundingRect = qwtBoundingRect(*this);

            return this->d_boundingRect;
          }

  public: inline void Add(const QPointF &_point)
          {
            this->d_samples += _point;

            /*this->history.push_back(_point.y());
            if (this->history.size() > 1000)
              this->history.pop_front();
              */
          }

  /*public: inline double GetMaxY()
          {
            std::list<double>::iterator maxIter =
              std::max_element(this->history.begin(),
                  this->history.end());

            return *maxIter;
          }
          */

  public: void Clear()
          {
            this->d_samples.clear();
            this->d_samples.squeeze();
            this->d_boundingRect = QRectF(0.0, 0.0, -1.0, -1.0);
          }

  // private: std::list<double> history;
};

/////////////////////////////////////////////////
IncrementalPlot::IncrementalPlot(QWidget *_parent)
  : QwtPlot(_parent)
{
  this->directPainter = new QwtPlotDirectPainter(this);

  // panning with the left mouse button
  (void) new QwtPlotPanner(this->canvas());

  // zoom in/out with the wheel
  (void) new QwtPlotMagnifier(this->canvas());

#if defined(Q_WS_X11)
  this->canvas()->setAttribute(Qt::WA_PaintOutsidePaintEvent, true);
  this->canvas()->setAttribute(Qt::WA_PaintOnScreen, true );
#endif

  this->setAutoReplot(true);

  this->setFrameStyle(QFrame::NoFrame);
  this->setLineWidth(0);
  this->setCanvasLineWidth(2);

  this->plotLayout()->setAlignCanvasToScales(true);

  QwtPlotGrid *grid = new QwtPlotGrid;
  grid->setMajPen(QPen(Qt::gray, 0, Qt::DotLine));
  grid->attach(this);

  this->setAxisScale(QwtPlot::xBottom, 0, 1.0);
  this->setAxisScale(QwtPlot::yLeft, 0, 0.002);

  QwtText xtitle("Real Time (s)");
  xtitle.setFont(QFont(fontInfo().family(), 10, QFont::Bold));
  this->setAxisTitle(QwtPlot::xBottom, xtitle);

  QwtText ytitle("Duration (ms)");
  ytitle.setFont(QFont(fontInfo().family(), 10, QFont::Bold));
  this->setAxisTitle(QwtPlot::yLeft, ytitle);

  this->replot();
}

/////////////////////////////////////////////////
IncrementalPlot::~IncrementalPlot()
{
  for (CurveMap::iterator iter = this->curves.begin();
       iter != this->curves.end(); ++iter)
  {
    delete iter->second;
  }

  this->curves.clear();
}

/////////////////////////////////////////////////
void IncrementalPlot::Add(const QString &_label, const QPointF &_pt)
{
  QwtPlotCurve *curve = NULL;

  CurveMap::iterator iter = this->curves.find(_label);
  if (iter == this->curves.end())
    curve = this->AddCurve(_label);
  else
    curve = iter->second;

  CurveData *curveData = static_cast<CurveData *>(curve->data());

  curveData->Add(_pt);

  const bool doClip = !this->canvas()->testAttribute(Qt::WA_PaintOnScreen);

  if (doClip)
  {
    // Depending on the platform setting a clip might be an important
    // performance issue. F.e. for Qt Embedded this reduces the
    // part of the backing store that has to be copied out - maybe
    // to an unaccelerated frame buffer device.
    const QwtScaleMap xMap = this->canvasMap(curve->xAxis());
    const QwtScaleMap yMap = this->canvasMap(curve->yAxis());

    QRegion clipRegion;

    const QSize symbolSize = curve->symbol()->size();
    QRect r(0, 0, symbolSize.width() + 2, symbolSize.height() + 2);

    const QPointF center = QwtScaleMap::transform(xMap, yMap, _pt);
    r.moveCenter(center.toPoint());
    clipRegion += r;

    this->directPainter->setClipRegion(clipRegion);
  }

  this->setAxisScale(this->xBottom,
      std::max(0.0, static_cast<double>(_pt.x() - 5.0)),
      std::max(1.0, static_cast<double>(_pt.x())));

  this->setAxisScale(this->yLeft, 0.0, curve->maxYValue());

  this->directPainter->drawSeries(curve,
      curveData->size() - 1,
      curveData->size() - 1);
}

/////////////////////////////////////////////////
QwtPlotCurve *IncrementalPlot::AddCurve(const QString &_label)
{
  QwtPlotCurve *curve = new QwtPlotCurve(_label);

  curve->setStyle(QwtPlotCurve::Lines);
  curve->setData(new CurveData());

  curve->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,
        Qt::NoBrush, QPen(Qt::red), QSize(2, 2)));

  QPen pen(QColor(255, 0, 0));
  pen.setWidth(1.0);
  curve->setStyle(QwtPlotCurve::Lines);
  curve->setPen(pen);

  curve->attach(this);

  // Delete an old curve if it exists.
  if (this->curves.find(_label) != this->curves.end())
  {
    CurveData *curveData = static_cast<CurveData*>(
        this->curves[_label]->data());
    curveData->Clear();
    delete this->curves[_label];
  }

  this->curves[_label] = curve;

  if (this->curves.size() == 2)
  {
    this->enableAxis(QwtPlot::yRight);

    QwtText ytitle("Real Time Factor (%)");
    ytitle.setFont(QFont(fontInfo().family(), 10, QFont::Bold));
    this->setAxisTitle(QwtPlot::yRight, ytitle);
    curve->setYAxis(QwtPlot::yRight);
  }


  return curve;
}

/////////////////////////////////////////////////
void IncrementalPlot::Clear(const QString &_label)
{
  CurveMap::iterator iter = this->curves.find(_label);

  if (iter == this->curves.end())
    return;

  CurveData *curveData = static_cast<CurveData *>(iter->second->data());
  curveData->Clear();;

  delete iter->second;
  this->curves.erase(iter);

  this->replot();
}

/////////////////////////////////////////////////
void IncrementalPlot::Clear()
{
  CurveData *curveData = NULL;

  for (CurveMap::iterator iter = this->curves.begin();
       iter != this->curves.end(); ++iter)
  {
    curveData = static_cast<CurveData *>(iter->second->data());
    curveData->Clear();
    delete iter->second;
  }

  this->curves.clear();

  this->replot();
}

/////////////////////////////////////////////////
QSize IncrementalPlot::sizeHint() const
{
  return QSize(540, 400);
}
