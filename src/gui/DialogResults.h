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
#ifndef DIALOGRESULTS_H
#define DIALOGRESULTS_H

////////////////////////////////////////////////////////////////////////////////

#include <QDialog>
#include <QPen>

#include <Wing.h>

////////////////////////////////////////////////////////////////////////////////

namespace Ui
{
    class DialogResults;
}

////////////////////////////////////////////////////////////////////////////////

class DialogResults : public QDialog
{
    Q_OBJECT

public:

    explicit DialogResults( QWidget *parent = Q_NULLPTR );

    ~DialogResults();

    void setWing( Wing *wing );

public slots:

    void show();

private:

    Ui::DialogResults *_ui;

    Wing *_wing;

    QPen _penPlot;

    void setAoA( bool enabled );

    void updateAll();

    void updatePlotResults_LiftCoef();
    void updatePlotResults_DragCoef();
    void updatePlotResults_CircDist();
    void updatePlotResults_LiftDist();
    void updatePlotResults_VelDist();
    void updatePlotResults_DragDist();

    void updateTextResults_LiftCoef();
    void updateTextResults_DragCoef();
    void updateTextResults_CircDist();
    void updateTextResults_LiftDist();
    void updateTextResults_VelDist();
    void updateTextResults_DragDist();

private slots:

    void on_listWidget_currentRowChanged( int currentRow );

    void on_sliderAoA_valueChanged( int value );
};

////////////////////////////////////////////////////////////////////////////////

#endif // DIALOGRESULTS_H
