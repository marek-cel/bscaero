/****************************************************************************//*
 * Copyright (C) 2021 Marek M. Cel
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 ******************************************************************************/

#include <gui/GraphicsPlanform.h>

#ifdef WIN32
#   include <float.h>
#endif

#include <cfloat>
#include <cmath>
#include <cstdio>

////////////////////////////////////////////////////////////////////////////////

GraphicsPlanform::GraphicsPlanform( QWidget *parent ) :
    QGraphicsView ( parent ),

    _scene ( Q_NULLPTR ),

    _brushLE ( QColor(   0 ,  0 ,  0 ), Qt::SolidPattern ),
    _brushTE ( QColor(   0 ,  0 ,  0 ), Qt::SolidPattern ),
    _brushXS ( QColor(  50 , 50 , 50 ), Qt::SolidPattern ),
    _brushXH ( QColor( 255 ,  0 ,  0 ), Qt::SolidPattern ),

    _penLE ( _brushLE, 2 ),
    _penTE ( _brushTE, 2 ),
    _penXS ( _brushXS, 1 ),
    _penXH ( _brushXH, 2 ),

    _selected ( -1 )
{
    _scene = new QGraphicsScene( this );
    setScene( _scene );

    _scene->clear();

    init();
}

////////////////////////////////////////////////////////////////////////////////

GraphicsPlanform::~GraphicsPlanform()
{
    if ( _scene )
    {
        _scene->clear();
        delete _scene;
        _scene = Q_NULLPTR;
    }

    reset();
}

////////////////////////////////////////////////////////////////////////////////

void GraphicsPlanform::reinit()
{
    if ( _scene )
    {
        _scene->clear();

        init();
    }
}

////////////////////////////////////////////////////////////////////////////////

void GraphicsPlanform::setData( std::vector< Data > data )
{
    _data = data;
    reinit();
}

////////////////////////////////////////////////////////////////////////////////

void GraphicsPlanform::setSelected( int selected )
{
    _selected = selected;
    reinit();
}

////////////////////////////////////////////////////////////////////////////////

void GraphicsPlanform::resizeEvent( QResizeEvent *event )
{
    ////////////////////////////////////
    QGraphicsView::resizeEvent( event );
    ////////////////////////////////////

    reinit();
}

////////////////////////////////////////////////////////////////////////////////

void GraphicsPlanform::init()
{
    updateView();
}

////////////////////////////////////////////////////////////////////////////////

void GraphicsPlanform::reset()
{
    for ( int i = 0; i < _le.size(); i++ ) { _le[ i ] = Q_NULLPTR; }
    for ( int i = 0; i < _te.size(); i++ ) { _te[ i ] = Q_NULLPTR; }
    for ( int i = 0; i < _xs.size(); i++ ) { _xs[ i ] = Q_NULLPTR; }

    _le.clear();
    _te.clear();
    _xs.clear();
}

////////////////////////////////////////////////////////////////////////////////

void GraphicsPlanform::updateView()
{
    _scene->clear();

    double x_max =  0.0;
    double y_min =  DBL_MAX;
    double y_max = -DBL_MAX;

    for ( Data d : _data )
    {
        double x1 = d.y;
        double y1 = d.x_le;
        double y2 = d.x_te;

        x_max = std::max( x_max, x1 );
        y_min = std::min( y_min, y1 );
        y_min = std::min( y_min, y2 );
        y_max = std::max( y_max, y1 );
        y_max = std::max( y_max, y2 );
    }

    double scaleX = static_cast< double >( width()  ) / ( 2.0 * x_max );
    double scaleY = static_cast< double >( height() ) / ( y_max - y_min );

    double scale_pen = 0.1 * scaleX;


    double scale = 100.0;//std::min( scaleX, scaleY );

    for ( size_t i = 0; i < _data.size(); i++ )
    {
        double x1 = scale * _data[ i ].y;
        double y1 = scale * _data[ i ].x_le;
        double x2 = scale * _data[ i ].y;
        double y2 = scale * _data[ i ].x_te;

        QPen penXS( _brushXS, 0.2 * scale_pen );
        QPen penXH( _brushXH, 0.5 * scale_pen );

        QGraphicsLineItem *xs_l = _scene->addLine( -x1, y1, -x2, y2, ( (int)i == _selected ) ? penXH : penXS );
        QGraphicsLineItem *xs_r = _scene->addLine(  x1, y1,  x2, y2, ( (int)i == _selected ) ? penXH : penXS );

        _xs.push_back( xs_l );
        _xs.push_back( xs_r );

        if ( i < _data.size() - 1 )
        {
            double x1 = scale * _data[ i     ].y;
            double x2 = scale * _data[ i + 1 ].y;

            double y1_le = scale * _data[ i     ].x_le;
            double y2_le = scale * _data[ i + 1 ].x_le;

            double y1_te = scale * _data[ i     ].x_te;
            double y2_te = scale * _data[ i + 1 ].x_te;

            QPen penLE( _brushLE, 0.5 * scale_pen );
            QPen penTE( _brushTE, 0.5 * scale_pen );

            QGraphicsLineItem *le_l = _scene->addLine( -x1, y1_le, -x2, y2_le, penLE );
            QGraphicsLineItem *le_r = _scene->addLine(  x1, y1_le,  x2, y2_le, penLE );

            QGraphicsLineItem *te_l = _scene->addLine( -x1, y1_te, -x2, y2_te, penTE );
            QGraphicsLineItem *te_r = _scene->addLine(  x1, y1_te,  x2, y2_te, penTE );

            _le.push_back( le_l );
            _le.push_back( le_r );
            _te.push_back( te_l );
            _te.push_back( te_r );
        }
    }

    _scene->update();

    //centerOn( 0, 0 );

    //setSceneRect( -scale * ( x_max ) - 10,
    //               scale * ( y_min ) - 10,
    //               scale * ( 2.0 * x_max )   + 20,
    //               scale * ( y_max + y_min ) + 20
    //            );

    fitInView( _scene->sceneRect(), Qt::KeepAspectRatio );
}
