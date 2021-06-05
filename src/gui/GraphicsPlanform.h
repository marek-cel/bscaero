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
#ifndef GRAPHICSPLANFORM_H
#define GRAPHICSPLANFORM_H

////////////////////////////////////////////////////////////////////////////////

#include <QGraphicsLineItem>
#include <QGraphicsView>

#include <defs.h>

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Wing planform graphics view class.
 */
class GraphicsPlanform : public QGraphicsView
{
    Q_OBJECT

public:

    struct Data
    {
        double y;
        double x_le;
        double x_te;
    };

    /** @brief Constructor. */
    explicit GraphicsPlanform( QWidget *parent = Q_NULLPTR );

    /** @brief Destructor. */
    virtual ~GraphicsPlanform();

    /** */
    void reinit();

    void setData( std::vector< Data > data );

    void setSelected( int selected );

protected:

    /** */
    void resizeEvent( QResizeEvent *event );

private:

    QGraphicsScene *_scene;         ///< graphics scene

    QVector< QGraphicsLineItem* > _le;
    QVector< QGraphicsLineItem* > _te;
    QVector< QGraphicsLineItem* > _xs;

    QBrush _brushLE;                ///<
    QBrush _brushTE;                ///<
    QBrush _brushXS;                ///<
    QBrush _brushXH;                ///<

    QPen _penLE;                    ///<
    QPen _penTE;                    ///<
    QPen _penXS;                    ///<
    QPen _penXH;                    ///<

    std::vector< Data > _data;

    int _selected;

    /** */
    void init();

    /** */
    void reset();

    /** */
    void updateView();
};

////////////////////////////////////////////////////////////////////////////////

#endif // GRAPHICSPLANFORM_H
