#pragma GCC system_header
/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_PLOT_RASTERITEM_H
#define QWT_PLOT_RASTERITEM_H

#include "qwt_global.h"
#include "qwt_plot_item.h"
#include "qwt_interval.h"
#include <qglobal.h>
#include <qstring.h>
#include <qimage.h>

/*!
  \brief A class, which displays raster data

  Raster data is a grid of pixel values, that can be represented
  as a QImage. It is used for many types of information like
  spectrograms, cartograms, geographical maps ...

  Often a plot has several types of raster data organized in layers.
  ( f.e a geographical map, with weather statistics ).
  Using setAlpha() raster items can be stacked easily.

  QwtPlotRasterItem is only implemented for images of the following formats:
  QImage::Format_Indexed8, QImage::Format_ARGB32.

  \sa QwtPlotSpectrogram
*/

class QWT_EXPORT QwtPlotRasterItem: public QwtPlotItem
{
public:
    /*!
      - NoCache\n
        renderImage() is called, whenever the item has to be repainted
      - PaintCache\n
        renderImage() is called, whenever the image cache is not valid,
        or the scales, or the size of the canvas has changed. This type
        of cache is only useful for improving the performance of hide/show
        operations. All other situations are already handled by the
        plot canvas cache.

      The default policy is NoCache
     */
    enum CachePolicy
    {
        NoCache,
        PaintCache
    };

    /*!
        Attributes to modify the drawing algorithm.
        \sa setPaintAttribute(), testPaintAttribute()
    */
    enum PaintAttribute
    {
        /*!
          When the image is rendered according to the data pixels
          ( QwtRasterData::pixelHint() ) it can be expanded to paint
          device resolution before it is passed to QPainter. 
          The expansion algorithm rounds the pixel borders in the same 
          way as the axis ticks, what is usually better than the
          scaling algorithm implemented in Qt.
          Disabling this flag might make sense, to reduce the size of a 
          document/file. If this is possible for a document format
          depends on the implementation of the specific QPaintEngine.
         */

        PaintInDeviceResolution = 1
    };

    //! Paint attributes
    typedef QFlags<PaintAttribute> PaintAttributes;

    explicit QwtPlotRasterItem( const QString& title = QString::null );
    explicit QwtPlotRasterItem( const QwtText& title );
    virtual ~QwtPlotRasterItem();

    void setPaintAttribute( PaintAttribute, bool on = true );
    bool testPaintAttribute( PaintAttribute ) const;

    void setAlpha( int alpha );
    int alpha() const;

    void setCachePolicy( CachePolicy );
    CachePolicy cachePolicy() const;

    void invalidateCache();

    virtual void draw( QPainter *p,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &rect ) const;

    virtual QRectF pixelHint( const QRectF & ) const;

    virtual QwtInterval interval(Qt::Axis) const;
    virtual QRectF boundingRect() const;

protected:
    /*!
      \brief Render an image 

      An implementation of render() might iterate over all
      pixels of imageRect. Each pixel has to be translated into 
      the corresponding position in scale coordinates using the maps.
      This position can be used to look up a value in a implementation
      specific way and to map it into a color.

      \param xMap X-Scale Map
      \param yMap Y-Scale Map
      \param area Requested area for the image in scale coordinates
      \param imageSize Requested size of the image
     */
    virtual QImage renderImage( const QwtScaleMap &xMap,
        const QwtScaleMap &yMap, const QRectF &area,
        const QSize &imageSize ) const = 0;

    virtual QwtScaleMap imageMap( Qt::Orientation,
        const QwtScaleMap &map, const QRectF &area,
        const QSize &imageSize, double pixelSize) const;

private:
    QwtPlotRasterItem( const QwtPlotRasterItem & );
    QwtPlotRasterItem &operator=( const QwtPlotRasterItem & );

    void init();

    QImage compose( const QwtScaleMap &, const QwtScaleMap &,
        const QRectF &imageArea, const QRectF &paintRect,
        const QSize &imageSize, bool doCache) const;


    class PrivateData;
    PrivateData *d_data;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( QwtPlotRasterItem::PaintAttributes )

#endif
