#pragma GCC system_header
/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_PLOT_MARKER_H
#define QWT_PLOT_MARKER_H

#include <qpen.h>
#include <qfont.h>
#include <qstring.h>
#include <qbrush.h>
#include "qwt_global.h"
#include "qwt_plot_item.h"

class QRectF;
class QwtText;
class QwtSymbol;

/*!
  \brief A class for drawing markers

  A marker can be a horizontal line, a vertical line,
  a symbol, a label or any combination of them, which can
  be drawn around a center point inside a bounding rectangle.

  The setSymbol() member assigns a symbol to the marker.
  The symbol is drawn at the specified point.

  With setLabel(), a label can be assigned to the marker.
  The setLabelAlignment() member specifies where the label is
  drawn. All the Align*-constants in Qt::AlignmentFlags (see Qt documentation)
  are valid. The interpretation of the alignment depends on the marker's
  line style. The alignment refers to the center point of
  the marker, which means, for example, that the label would be printed
  left above the center point if the alignment was set to 
  Qt::AlignLeft | Qt::AlignTop.
*/

class QWT_EXPORT QwtPlotMarker: public QwtPlotItem
{
public:

    /*!
        Line styles.
        \sa setLineStyle(), lineStyle()
    */
    enum LineStyle
    {
        //! No line
        NoLine,

        //! A horizontal line
        HLine,

        //! A vertical line
        VLine,

        //! A crosshair
        Cross
    };

    explicit QwtPlotMarker();
    virtual ~QwtPlotMarker();

    virtual int rtti() const;

    double xValue() const;
    double yValue() const;
    QPointF value() const;

    void setXValue( double );
    void setYValue( double );
    void setValue( double, double );
    void setValue( const QPointF & );

    void setLineStyle( LineStyle st );
    LineStyle lineStyle() const;

    void setLinePen( const QPen &p );
    const QPen &linePen() const;

    void setSymbol( const QwtSymbol * );
    const QwtSymbol *symbol() const;

    void setLabel( const QwtText& );
    QwtText label() const;

    void setLabelAlignment( Qt::Alignment );
    Qt::Alignment labelAlignment() const;

    void setLabelOrientation( Qt::Orientation );
    Qt::Orientation labelOrientation() const;

    void setSpacing( int );
    int spacing() const;

    virtual void draw( QPainter *p,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF & ) const;

    virtual QRectF boundingRect() const;

    virtual void updateLegend( QwtLegend * ) const;
    virtual void drawLegendIdentifier( QPainter *, const QRectF & ) const;

protected:
    virtual void drawLines( QPainter *, 
        const QRectF &, const QPointF & ) const;

    virtual void drawLabel( QPainter *, 
        const QRectF &, const QPointF & ) const;

private:

    class PrivateData;
    PrivateData *d_data;
};

#endif
