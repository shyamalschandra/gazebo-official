#pragma GCC system_header
/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_PAINTER_H
#define QWT_PAINTER_H

#include "qwt_global.h"

#include <qpoint.h>
#include <qrect.h>
#include <qpen.h>
#include <qline.h>

class QPainter;
class QBrush;
class QColor;
class QWidget;
class QPolygonF;
class QRectF;
class QImage;
class QPixmap;
class QwtScaleMap;
class QwtColorMap;
class QwtInterval;

class QPalette;
class QTextDocument;
class QPainterPath;

/*!
  \brief A collection of QPainter workarounds
*/
class QWT_EXPORT QwtPainter
{
public:
    static void setPolylineSplitting( bool );
    static bool polylineSplitting();

    static void setRoundingAlignment( bool );
    static bool roundingAlignment();
    static bool roundingAlignment(QPainter *);

    static void drawText( QPainter *, double x, double y, const QString & );
    static void drawText( QPainter *, const QPointF &, const QString & );
    static void drawText( QPainter *, double x, double y, double w, double h,
        int flags, const QString & );
    static void drawText( QPainter *, const QRectF &, 
        int flags, const QString & );

#ifndef QT_NO_RICHTEXT
    static void drawSimpleRichText( QPainter *, const QRectF &,
        int flags, const QTextDocument & );
#endif

    static void drawRect( QPainter *, double x, double y, double w, double h );
    static void drawRect( QPainter *, const QRectF &rect );
    static void fillRect( QPainter *, const QRectF &, const QBrush & );

    static void drawEllipse( QPainter *, const QRectF & );
    static void drawPie( QPainter *, const QRectF & r, int a, int alen );

    static void drawLine( QPainter *, double x1, double y1, double x2, double y2 );
    static void drawLine( QPainter *, const QPointF &p1, const QPointF &p2 );
    static void drawLine( QPainter *, const QLineF & );

    static void drawPolygon( QPainter *, const QPolygonF &pa );
    static void drawPolyline( QPainter *, const QPolygonF &pa );
    static void drawPolyline( QPainter *, const QPointF *, int pointCount );

    static void drawPoint( QPainter *, double x, double y );
    static void drawPoint( QPainter *, const QPointF & );

    static void drawPath( QPainter *, const QPainterPath & );
    static void drawImage( QPainter *, const QRectF &, const QImage & );
    static void drawPixmap( QPainter *, const QRectF &, const QPixmap & );

    static void drawRoundedFrame( QPainter *, 
        const QRectF &, double xRadius, double yRadius,
        const QPalette &, int lineWidth, int frameStyle );

    static void drawFocusRect( QPainter *, QWidget * );
    static void drawFocusRect( QPainter *, QWidget *, const QRect & );

    static void drawColorBar( QPainter *painter,
        const QwtColorMap &, const QwtInterval &,
        const QwtScaleMap &, Qt::Orientation, const QRectF & );

    static bool isAligning( QPainter *painter );

private:
    static bool d_polylineSplitting;
    static bool d_roundingAlignment;
};

//!  Wrapper for QPainter::drawPoint()
inline void QwtPainter::drawPoint( QPainter *painter, double x, double y )
{
    QwtPainter::drawPoint( painter, QPointF( x, y ) );
}

//!  Wrapper for QPainter::drawLine()
inline void QwtPainter::drawLine( QPainter *painter,
    double x1, double y1, double x2, double y2 )
{
    QwtPainter::drawLine( painter, QPointF( x1, y1 ), QPointF( x2, y2 ) );
}

//!  Wrapper for QPainter::drawLine()
inline void QwtPainter::drawLine( QPainter *painter, const QLineF &line )
{
    QwtPainter::drawLine( painter, line.p1(), line.p2() );
}

/*!
  Returns whether line splitting for the raster paint engine is enabled.
  \sa setPolylineSplitting()
*/
inline bool QwtPainter::polylineSplitting()
{
    return d_polylineSplitting;
}

/*!
  Returns whether coordinates should be rounded, before they are painted
  to a paint engine that floors to integer values.  For other paint engines
  this ( PDF, SVG ), this flag has no effect.

  \sa setRoundingAlignment(), isAligning()
*/
inline bool QwtPainter::roundingAlignment()
{
    return d_roundingAlignment;
}

/*!
  \return roundingAlignment() && isAligning(painter);
  \param painter Painter
*/
inline bool QwtPainter::roundingAlignment(QPainter *painter)
{
    return d_roundingAlignment && isAligning(painter);
}
#endif
