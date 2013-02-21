#pragma GCC system_header
/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_PLOT_CURVE_H
#define QWT_PLOT_CURVE_H

#include "qwt_global.h"
#include "qwt_plot_seriesitem.h"
#include "qwt_series_data.h"
#include "qwt_text.h"
#include <qpen.h>
#include <qstring.h>

class QPainter;
class QPolygonF;
class QwtScaleMap;
class QwtSymbol;
class QwtCurveFitter;

/*!
  \brief A plot item, that represents a series of points

  A curve is the representation of a series of points in the x-y plane.
  It supports different display styles, interpolation ( f.e. spline )
  and symbols.

  \par Usage
  <dl><dt>a) Assign curve properties</dt>
  <dd>When a curve is created, it is configured to draw black solid lines
  with in QwtPlotCurve::Lines style and no symbols. 
  You can change this by calling
  setPen(), setStyle() and setSymbol().</dd>
  <dt>b) Connect/Assign data.</dt>
  <dd>QwtPlotCurve gets its points using a QwtSeriesData object offering
  a bridge to the real storage of the points ( like QAbstractItemModel ).
  There are several convenience classes derived from QwtSeriesData, that also store
  the points inside ( like QStandardItemModel ). QwtPlotCurve also offers
  a couple of variations of setSamples(), that build QwtSeriesData objects from
  arrays internally.</dd>
  <dt>c) Attach the curve to a plot</dt>
  <dd>See QwtPlotItem::attach()
  </dd></dl>

  \par Example:
  see examples/bode

  \sa QwtPointSeriesData, QwtSymbol, QwtScaleMap
*/
class QWT_EXPORT QwtPlotCurve: public QwtPlotSeriesItem<QPointF>
{
public:
    /*!
        Curve styles.
        \sa setStyle(), style()
    */
    enum CurveStyle
    {
        /*!
           Don't draw a curve. Note: This doesn't affect the symbols.
        */
        NoCurve = -1,

        /*!
           Connect the points with straight lines. The lines might
           be interpolated depending on the 'Fitted' attribute. Curve
           fitting can be configured using setCurveFitter().
        */
        Lines,

        /*!
           Draw vertical or horizontal sticks ( depending on the 
           orientation() ) from a baseline which is defined by setBaseline().
        */
        Sticks,

        /*!
           Connect the points with a step function. The step function
           is drawn from the left to the right or vice versa,
           depending on the QwtPlotCurve::Inverted attribute.
        */
        Steps,

        /*!
           Draw dots at the locations of the data points. Note:
           This is different from a dotted line (see setPen()), and faster
           as a curve in QwtPlotCurve::NoStyle style and a symbol 
           painting a point.
        */
        Dots,

        /*!
           Styles >= QwtPlotCurve::UserCurve are reserved for derived
           classes of QwtPlotCurve that overload drawCurve() with
           additional application specific curve types.
        */
        UserCurve = 100
    };

    /*!
      Attribute for drawing the curve
      \sa setCurveAttribute(), testCurveAttribute(), curveFitter()
    */
    enum CurveAttribute
    {
        /*!
           For QwtPlotCurve::Steps only. 
           Draws a step function from the right to the left.
         */
        Inverted = 0x01,

        /*!
          Only in combination with QwtPlotCurve::Lines
          A QwtCurveFitter tries to
          interpolate/smooth the curve, before it is painted.

          \note Curve fitting requires temporary memory
          for calculating coefficients and additional points.
          If painting in QwtPlotCurve::Fitted mode is slow it might be better
          to fit the points, before they are passed to QwtPlotCurve.
         */
        Fitted = 0x02
    };

    //! Curve attributes
    typedef QFlags<CurveAttribute> CurveAttributes;

    /*!
        Attributes how to represent the curve on the legend

        \sa setLegendAttribute(), testLegendAttribute(),
            drawLegendIdentifier()
     */

    enum LegendAttribute
    {
        /*!
          QwtPlotCurve tries to find a color representing the curve 
          and paints a rectangle with it.
         */
        LegendNoAttribute = 0x00,

        /*!
          If the style() is not QwtPlotCurve::NoCurve a line 
          is painted with the curve pen().
         */
        LegendShowLine = 0x01,

        /*!
          If the curve has a valid symbol it is painted.
         */
        LegendShowSymbol = 0x02,

        /*!
          If the curve has a brush a rectangle filled with the
          curve brush() is painted.
         */
        LegendShowBrush = 0x04
    };

    //! Legend attributes
    typedef QFlags<LegendAttribute> LegendAttributes;

    /*!
        Attributes to modify the drawing algorithm.
        The default setting enables ClipPolygons

        \sa setPaintAttribute(), testPaintAttribute()
    */
    enum PaintAttribute
    {
        /*!
          Clip polygons before painting them. In situations, where points
          are far outside the visible area (f.e when zooming deep) this
          might be a substantial improvement for the painting performance
         */
        ClipPolygons = 0x01,

        /*!
          Paint the symbol to a QPixmap and paint the pixmap
          instead rendering the symbol for each point. The flag has
          no effect, when the curve is not painted to the canvas
          ( f.e when exporting the plot to a PDF document ).
         */
        CacheSymbols = 0x02
    };

    //! Paint attributes
    typedef QFlags<PaintAttribute> PaintAttributes;

    explicit QwtPlotCurve( const QString &title = QString::null );
    explicit QwtPlotCurve( const QwtText &title );

    virtual ~QwtPlotCurve();

    virtual int rtti() const;

    void setPaintAttribute( PaintAttribute, bool on = true );
    bool testPaintAttribute( PaintAttribute ) const;

    void setLegendAttribute( LegendAttribute, bool on = true );
    bool testLegendAttribute( LegendAttribute ) const;

#ifndef QWT_NO_COMPAT
    void setRawSamples( const double *xData, const double *yData, int size );
    void setSamples( const double *xData, const double *yData, int size );
    void setSamples( const QVector<double> &xData, const QVector<double> &yData );
#endif
    void setSamples( const QVector<QPointF> & );

    int closestPoint( const QPoint &pos, double *dist = NULL ) const;

    double minXValue() const;
    double maxXValue() const;
    double minYValue() const;
    double maxYValue() const;

    void setCurveAttribute( CurveAttribute, bool on = true );
    bool testCurveAttribute( CurveAttribute ) const;

    void setPen( const QPen & );
    const QPen &pen() const;

    void setBrush( const QBrush & );
    const QBrush &brush() const;

    void setBaseline( double ref );
    double baseline() const;

    void setStyle( CurveStyle style );
    CurveStyle style() const;

    void setSymbol( const QwtSymbol *s );
    const QwtSymbol *symbol() const;

    void setCurveFitter( QwtCurveFitter * );
    QwtCurveFitter *curveFitter() const;

    virtual void drawSeries( QPainter *,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &canvasRect, int from, int to ) const;

    virtual void updateLegend( QwtLegend * ) const;
    virtual void drawLegendIdentifier( QPainter *, const QRectF & ) const;

protected:

    void init();

    virtual void drawCurve( QPainter *, int style,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &canvasRect, int from, int to ) const;

    virtual void drawSymbols( QPainter *, const QwtSymbol &,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &canvasRect, int from, int to ) const;

    void drawLines( QPainter *,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &canvasRect, int from, int to ) const;

    void drawSticks( QPainter *,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &canvasRect, int from, int to ) const;

    void drawDots( QPainter *,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &canvasRect, int from, int to ) const;

    void drawSteps( QPainter *,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &canvasRect, int from, int to ) const;

    virtual void fillCurve( QPainter *,
        const QwtScaleMap &, const QwtScaleMap &, 
        const QRectF &canvasRect, QPolygonF & ) const;

    void closePolyline( QPainter *,
        const QwtScaleMap &, const QwtScaleMap &, QPolygonF & ) const;

private:
    class PrivateData;
    PrivateData *d_data;
};

//! boundingRect().left()
inline double QwtPlotCurve::minXValue() const
{
    return boundingRect().left();
}

//! boundingRect().right()
inline double QwtPlotCurve::maxXValue() const
{
    return boundingRect().right();
}

//! boundingRect().top()
inline double QwtPlotCurve::minYValue() const
{
    return boundingRect().top();
}

//! boundingRect().bottom()
inline double QwtPlotCurve::maxYValue() const
{
    return boundingRect().bottom();
}

Q_DECLARE_OPERATORS_FOR_FLAGS( QwtPlotCurve::PaintAttributes )
Q_DECLARE_OPERATORS_FOR_FLAGS( QwtPlotCurve::LegendAttributes )
Q_DECLARE_OPERATORS_FOR_FLAGS( QwtPlotCurve::CurveAttributes )

#endif
