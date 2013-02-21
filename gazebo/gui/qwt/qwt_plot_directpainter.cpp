#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wswitch-default"
/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_plot_directpainter.h"
#include "qwt_scale_map.h"
#include "qwt_plot.h"
#include "qwt_plot_canvas.h"
#include "qwt_plot_seriesitem.h"
#include <qpainter.h>
#include <qevent.h>
#include <qapplication.h>
#include <qpixmap.h>

static inline void renderItem( 
    QPainter *painter, const QRect &canvasRect,
    QwtPlotAbstractSeriesItem *seriesItem, int from, int to )
{
    // A minor performance improvement is possible
    // with caching the maps. TODO ...

    QwtPlot *plot = seriesItem->plot();
    const QwtScaleMap xMap = plot->canvasMap( seriesItem->xAxis() );
    const QwtScaleMap yMap = plot->canvasMap( seriesItem->yAxis() );

    painter->setRenderHint( QPainter::Antialiasing,
        seriesItem->testRenderHint( QwtPlotItem::RenderAntialiased ) );
    seriesItem->drawSeries( painter, xMap, yMap, canvasRect, from, to );
}

class QwtPlotDirectPainter::PrivateData
{
public:
    PrivateData():
        attributes( 0 ),
        hasClipping(false),
        seriesItem( NULL )
    {
    }

    QwtPlotDirectPainter::Attributes attributes;

    bool hasClipping;
    QRegion clipRegion;

    QPainter painter;

    QwtPlotAbstractSeriesItem *seriesItem;
    int from;
    int to;
};

//! Constructor
QwtPlotDirectPainter::QwtPlotDirectPainter( QObject *parent ):
    QObject( parent )
{
    d_data = new PrivateData;
}

//! Destructor
QwtPlotDirectPainter::~QwtPlotDirectPainter()
{
    delete d_data;
}

/*!
  Change an attribute

  \param attribute Attribute to change
  \param on On/Off

  \sa Attribute, testAttribute()
*/
void QwtPlotDirectPainter::setAttribute( Attribute attribute, bool on )
{
    if ( bool( d_data->attributes & attribute ) != on )
    {
        if ( on )
            d_data->attributes |= attribute;
        else
            d_data->attributes &= ~attribute;

        if ( ( attribute == AtomicPainter ) && on )
            reset();
    }
}

/*!
  Check if a attribute is set.

  \param attribute Attribute to be tested
  \sa Attribute, setAttribute()
*/
bool QwtPlotDirectPainter::testAttribute( Attribute attribute ) const
{
    return d_data->attributes & attribute;
}

/*!
  En/Disables clipping 

  \param enable Enables clipping is true, disable it otherwise
  \sa hasClipping(), clipRegion(), setClipRegion()
*/
void QwtPlotDirectPainter::setClipping( bool enable )
{
    d_data->hasClipping = enable;
}

/*!
  \return true, when clipping is enabled
  \sa setClipping(), clipRegion(), setClipRegion()
*/
bool QwtPlotDirectPainter::hasClipping() const
{
    return d_data->hasClipping;
}

/*!
   \brief Assign a clip region and enable clipping

   Depending on the environment setting a proper clip region might improve 
   the performance heavily. F.e. on Qt embedded only the clipped part of
   the backing store will be copied to a ( maybe unaccelerated ) frame buffer
   device.
   
   \param region Clip region
   \sa clipRegion(), hasClipping(), setClipping()
*/
void QwtPlotDirectPainter::setClipRegion( const QRegion &region )
{
    d_data->clipRegion = region;
    d_data->hasClipping = true;
}

/*!
   \return Currently set clip region.
   \sa setClipRegion(), setClipping(), hasClipping()
*/
QRegion QwtPlotDirectPainter::clipRegion() const
{
    return d_data->clipRegion;
}

/*!
  \brief Draw a set of points of a seriesItem.

  When observing an measurement while it is running, new points have to be
  added to an existing seriesItem. drawSeries() can be used to display them avoiding
  a complete redraw of the canvas.

  Setting plot()->canvas()->setAttribute(Qt::WA_PaintOutsidePaintEvent, true);
  will result in faster painting, if the paint engine of the canvas widget
  supports this feature.

  \param seriesItem Item to be painted
  \param from Index of the first point to be painted
  \param to Index of the last point to be painted. If to < 0 the
         series will be painted to its last point.
*/
void QwtPlotDirectPainter::drawSeries(
    QwtPlotAbstractSeriesItem *seriesItem, int from, int to )
{
    if ( seriesItem == NULL || seriesItem->plot() == NULL )
        return;

    QwtPlotCanvas *canvas = seriesItem->plot()->canvas();
    const QRect canvasRect = canvas->contentsRect();

    const bool hasBackingStore = 
        canvas->testPaintAttribute( QwtPlotCanvas::BackingStore ) 
        && canvas->backingStore() && !canvas->backingStore()->isNull();

    if ( hasBackingStore )
    {
        QPainter painter( const_cast<QPixmap *>( canvas->backingStore() ) );

        if ( d_data->hasClipping )
            painter.setClipRegion( d_data->clipRegion );

        renderItem( &painter, canvasRect, seriesItem, from, to );

        if ( testAttribute( QwtPlotDirectPainter::FullRepaint ) )
        {
            canvas->repaint();
            return;
        }
    }

    bool immediatePaint = true;
    if ( !canvas->testAttribute( Qt::WA_WState_InPaintEvent ) &&
        !canvas->testAttribute( Qt::WA_PaintOutsidePaintEvent ) )
    {
        immediatePaint = false;
    }

    if ( immediatePaint )
    {
        if ( !d_data->painter.isActive() )
        {
            reset();

            d_data->painter.begin( canvas );
            canvas->installEventFilter( this );
        }

        if ( d_data->hasClipping )
        {
            d_data->painter.setClipRegion( 
                QRegion( canvasRect ) & d_data->clipRegion );
        }
        else
        {
            if ( !d_data->painter.hasClipping() )
                d_data->painter.setClipRect( canvasRect );
        }

        renderItem( &d_data->painter, canvasRect, seriesItem, from, to );

        if ( d_data->attributes & QwtPlotDirectPainter::AtomicPainter )
        {
            reset();
        }
        else
        {
            if ( d_data->hasClipping )
                d_data->painter.setClipping( false );
        }
    }
    else
    {
        reset();

        d_data->seriesItem = seriesItem;
        d_data->from = from;
        d_data->to = to;

        QRegion clipRegion = canvasRect;
        if ( d_data->hasClipping )
            clipRegion &= d_data->clipRegion;

        canvas->installEventFilter( this );
        canvas->repaint(clipRegion);
        canvas->removeEventFilter( this );

        d_data->seriesItem = NULL;
    }
}

//! Close the internal QPainter
void QwtPlotDirectPainter::reset()
{
    if ( d_data->painter.isActive() )
    {
        QWidget *w = ( QWidget * )d_data->painter.device();
        if ( w )
            w->removeEventFilter( this );

        d_data->painter.end();
    }
}

//! Event filter
bool QwtPlotDirectPainter::eventFilter( QObject *, QEvent *event )
{
    if ( event->type() == QEvent::Paint )
    {
        reset();

        if ( d_data->seriesItem )
        {
            const QPaintEvent *pe = static_cast< QPaintEvent *>( event );

            QwtPlotCanvas *canvas = d_data->seriesItem->plot()->canvas();

            QPainter painter( canvas );
            painter.setClipRegion( pe->region() );

            bool copyCache = testAttribute( CopyBackingStore )
                && canvas->testPaintAttribute( QwtPlotCanvas::BackingStore );

            if ( copyCache )
            {
                // is something valid in the cache ?
                copyCache = ( canvas->backingStore() != NULL )
                    && !canvas->backingStore()->isNull();
            }

            if ( copyCache )
            {
                painter.drawPixmap( 
                    canvas->contentsRect().topLeft(), 
                    *canvas->backingStore() );
            }
            else
            {
                renderItem( &painter, canvas->contentsRect(),
                    d_data->seriesItem, d_data->from, d_data->to );
            }

            return true; // don't call QwtPlotCanvas::paintEvent()
        }
    }

    return false;
}
