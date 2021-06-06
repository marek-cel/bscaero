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

#include <gui/DialogResults.h>
#include <ui_DialogResults.h>

#include <qwt_plot_curve.h>

////////////////////////////////////////////////////////////////////////////////

DialogResults::DialogResults(QWidget *parent) :
    QDialog ( parent ),
    _ui ( new Ui::DialogResults ),

    _wing ( Q_NULLPTR ),

    _penPlot ( QPen( Qt::black, 2 ) )
{
    _ui->setupUi( this );

#   ifdef WIN32
    _ui->textResults->setFontFamily( "Courier" );
#   else
    _ui->textResults->setFontFamily( "Monospace" );
#   endif
}

////////////////////////////////////////////////////////////////////////////////

DialogResults::~DialogResults()
{
    if ( _ui ) delete _ui;
    _ui = Q_NULLPTR;
}

////////////////////////////////////////////////////////////////////////////////

void DialogResults::setWing( Wing *wing )
{
    _wing = wing;
}

////////////////////////////////////////////////////////////////////////////////

void DialogResults::show()
{
    ////////////////
    QDialog::show();
    ////////////////

    _ui->sliderAoA->setValue( 0 );
    _ui->sliderAoA->setMaximum( _wing->getAoA_Iterations() - 1 );

    _ui->listWidget->setCurrentRow( 0 );
    _ui->tabWidget->setCurrentIndex( 0 );
}

////////////////////////////////////////////////////////////////////////////////

void DialogResults::setAoA( bool enabled )
{
    _ui->labelAoA     ->setEnabled( enabled );
    _ui->sliderAoA    ->setEnabled( enabled );
    _ui->spinBoxAoA   ->setEnabled( enabled );
    _ui->labelUnitAoA ->setEnabled( enabled );
}

////////////////////////////////////////////////////////////////////////////////

void DialogResults::updateAll()
{
    _ui->plotResults->detachItems( QwtPlotItem::Rtti_PlotCurve  , true );
    _ui->plotResults->detachItems( QwtPlotItem::Rtti_PlotMarker , true );

    _ui->textResults->setText( "" );

    setAoA( false );

    if ( _wing )
    {
        if ( _wing->isResultsUpToDate() )
        {
            switch ( _ui->listWidget->currentRow() )
            {
            case 0:
                updatePlotResults_LiftCoef();
                updateTextResults_LiftCoef();
                break;

            case 1:
                updatePlotResults_DragCoef();
                updateTextResults_DragCoef();
                break;

            case 2:
                setAoA( true );
                updatePlotResults_CircDist();
                updateTextResults_CircDist();
                break;

            case 3:
                setAoA( true );
                updatePlotResults_LiftDist();
                updateTextResults_LiftDist();
                break;

            case 4:
                setAoA( true );
                updatePlotResults_VelDist();
                updateTextResults_VelDist();
                break;

            case 5:
                setAoA( true );
                updatePlotResults_DragDist();
                updateTextResults_DragDist();
                break;
            }
        }
    }

    _ui->plotResults->replot();
}

////////////////////////////////////////////////////////////////////////////////

void DialogResults::updatePlotResults_LiftCoef()
{
    QVector< double > vx1;
    QVector< double > vy1;

    for ( int i = 0; i < _wing->getAoA_Iterations(); i++ )
    {
        vx1.push_back( _wing->getAoADeg   ( i ) );
        vy1.push_back( _wing->getLiftCoef ( i ) );
    }


    if ( vx1.length() > 0 && vy1.length() > 0 && vx1.length() == vy1.length() )
    {
        QwtPlotCurve *curve = new QwtPlotCurve( "" );

        curve->setSamples( vx1, vy1 );
        curve->setPen( _penPlot );

        curve->attach( _ui->plotResults );
    }
}

////////////////////////////////////////////////////////////////////////////////

void DialogResults::updatePlotResults_DragCoef()
{
    QVector< double > vx1;
    QVector< double > vy1;

    for ( int i = 0; i < _wing->getAoA_Iterations(); i++ )
    {
        vx1.push_back( _wing->getLiftCoef( i ) );
        vy1.push_back( _wing->getDragCoef( i ) );
    }


    if ( vx1.length() > 0 && vy1.length() > 0 && vx1.length() == vy1.length() )
    {
        QwtPlotCurve *curve = new QwtPlotCurve( "" );

        curve->setSamples( vx1, vy1 );
        curve->setPen( _penPlot );

        curve->attach( _ui->plotResults );
    }
}

////////////////////////////////////////////////////////////////////////////////

void DialogResults::updatePlotResults_CircDist()
{
    QVector< double > vx1;
    QVector< double > vy1;

    int aoa = _ui->sliderAoA->value();

    for ( int i = 0; i < _wing->getSectionsFullspan(); i++ )
    {
        vx1.push_back( _wing->getWingSpanCoef( i ) );
        vy1.push_back( _wing->getGamma( aoa, i ) );
    }


    if ( vx1.length() > 0 && vy1.length() > 0 && vx1.length() == vy1.length() )
    {
        QwtPlotCurve *curve = new QwtPlotCurve( "" );

        curve->setSamples( vx1, vy1 );
        curve->setPen( _penPlot );

        curve->attach( _ui->plotResults );
    }
}

////////////////////////////////////////////////////////////////////////////////

void DialogResults::updatePlotResults_LiftDist()
{
    QVector< double > vx1;
    QVector< double > vy1;

    int aoa = _ui->sliderAoA->value();

    for ( int i = 0; i < _wing->getSectionsFullspan(); i++ )
    {
        vx1.push_back( _wing->getWingSpanCoef( i ) );
        vy1.push_back( _wing->getLiftCoefDist( aoa, i ) );
    }


    if ( vx1.length() > 0 && vy1.length() > 0 && vx1.length() == vy1.length() )
    {
        QwtPlotCurve *curve = new QwtPlotCurve( "" );

        curve->setSamples( vx1, vy1 );
        curve->setPen( _penPlot );

        curve->attach( _ui->plotResults );
    }
}

////////////////////////////////////////////////////////////////////////////////

void DialogResults::updatePlotResults_VelDist()
{
    QVector< double > vx1;
    QVector< double > vy1;

    int aoa = _ui->sliderAoA->value();

    for ( int i = 1; i < _wing->getSectionsFullspan() - 1; i++ )
    {
        vx1.push_back( _wing->getWingSpanCoef( i ) );
        vy1.push_back( _wing->getVelocityDist( aoa, i ) );
    }


    if ( vx1.length() > 0 && vy1.length() > 0 && vx1.length() == vy1.length() )
    {
        QwtPlotCurve *curve = new QwtPlotCurve( "" );

        curve->setSamples( vx1, vy1 );
        curve->setPen( _penPlot );

        curve->attach( _ui->plotResults );
    }
}

////////////////////////////////////////////////////////////////////////////////

void DialogResults::updatePlotResults_DragDist()
{
    QVector< double > vx1;
    QVector< double > vy1;

    int aoa = _ui->sliderAoA->value();

    for ( int i = 1; i < _wing->getSectionsFullspan() - 1; i++ )
    {
        vx1.push_back( _wing->getWingSpanCoef( i ) );
        vy1.push_back( _wing->getDragCoefDist( aoa, i ) );
    }


    if ( vx1.length() > 0 && vy1.length() > 0 && vx1.length() == vy1.length() )
    {
        QwtPlotCurve *curve = new QwtPlotCurve( "" );

        curve->setSamples( vx1, vy1 );
        curve->setPen( _penPlot );

        curve->attach( _ui->plotResults );
    }
}

////////////////////////////////////////////////////////////////////////////////

void DialogResults::updateTextResults_LiftCoef()
{
    QString text;

    text += "AoA [deg]     C_L [-]";
    text += "\n";

    for ( int i = 0; i < _wing->getAoA_Iterations(); i++ )
    {
        text += QString( "%1" ).arg( _wing->getAoADeg   ( i ), 6, 'f', 2, QChar(' ') );
        text += "        ";
        text += QString( "%1" ).arg( _wing->getLiftCoef ( i ), 9, 'f', 6, QChar(' ') );
        text += "\n";
    }

    _ui->textResults->setText( text );
}

////////////////////////////////////////////////////////////////////////////////

void DialogResults::updateTextResults_DragCoef()
{
    QString text;

    text += "C_L [-]       C_D [-]";
    text += "\n";

    for ( int i = 0; i < _wing->getAoA_Iterations(); i++ )
    {
        text += QString( "%1" ).arg( _wing->getLiftCoef( i ), 9, 'f', 6, QChar(' ') );
        text += "        ";
        text += QString( "%1" ).arg( _wing->getDragCoef( i ), 9, 'f', 6, QChar(' ') );
        text += "\n";
    }

    _ui->textResults->setText( text );
}

////////////////////////////////////////////////////////////////////////////////

void DialogResults::updateTextResults_CircDist()
{
    QString text;

    text += "             ";

    for ( int j = 0; j < _wing->getAoA_Iterations(); j++ )
    {
        text += QString( "%1" ).arg( _wing->getAoADeg( j ), 6, 'f', 2, QChar(' ') );
        text += "           ";
    }

    text += "\n";

    for ( int i = 0; i < _wing->getSectionsFullspan(); i++ )
    {
        text += QString( "%1" ).arg( _wing->getWingSpanCoef( i ), 6, 'f', 2, QChar(' ') );
        text += "        ";

        for ( int j = 0; j < _wing->getAoA_Iterations(); j++ )
        {
            text += QString( "%1" ).arg( _wing->getGamma( j, i ), 9, 'f', 6, QChar(' ') );
            text += "        ";
        }

        text += "\n";
    }

    _ui->textResults->setText( text );
}

////////////////////////////////////////////////////////////////////////////////

void DialogResults::updateTextResults_LiftDist()
{
    QString text;

    text += "             ";

    for ( int j = 0; j < _wing->getAoA_Iterations(); j++ )
    {
        text += QString( "%1" ).arg( _wing->getAoADeg( j ), 6, 'f', 2, QChar(' ') );
        text += "           ";
    }

    text += "\n";

    for ( int i = 0; i < _wing->getSectionsFullspan(); i++ )
    {
        text += QString( "%1" ).arg( _wing->getWingSpanCoef( i ), 6, 'f', 2, QChar(' ') );
        text += "        ";

        for ( int j = 0; j < _wing->getAoA_Iterations(); j++ )
        {
            text += QString( "%1" ).arg( _wing->getLiftCoefDist( j, i ), 9, 'f', 6, QChar(' ') );
            text += "        ";
        }

        text += "\n";
    }

    _ui->textResults->setText( text );
}

////////////////////////////////////////////////////////////////////////////////

void DialogResults::updateTextResults_VelDist()
{
    QString text;

    text += "             ";

    for ( int j = 0; j < _wing->getAoA_Iterations(); j++ )
    {
        text += QString( "%1" ).arg( _wing->getAoADeg( j ), 6, 'f', 2, QChar(' ') );
        text += "           ";
    }

    text += "\n";

    for ( int i = 1; i < _wing->getSectionsFullspan() - 1; i++ )
    {
        text += QString( "%1" ).arg( _wing->getWingSpanCoef( i ), 6, 'f', 2, QChar(' ') );
        text += "        ";

        for ( int j = 0; j < _wing->getAoA_Iterations(); j++ )
        {
            text += QString( "%1" ).arg( _wing->getVelocityDist( j, i ), 9, 'f', 6, QChar(' ') );
            text += "        ";
        }

        text += "\n";
    }

    _ui->textResults->setText( text );
}

////////////////////////////////////////////////////////////////////////////////

void DialogResults::updateTextResults_DragDist()
{
    QString text;

    text += "             ";

    for ( int j = 0; j < _wing->getAoA_Iterations(); j++ )
    {
        text += QString( "%1" ).arg( _wing->getAoADeg( j ), 6, 'f', 2, QChar(' ') );
        text += "           ";
    }

    text += "\n";

    for ( int i = 1; i < _wing->getSectionsFullspan() - 1; i++ )
    {
        text += QString( "%1" ).arg( _wing->getWingSpanCoef( i ), 6, 'f', 2, QChar(' ') );
        text += "        ";

        for ( int j = 0; j < _wing->getAoA_Iterations(); j++ )
        {
            text += QString( "%1" ).arg( _wing->getDragCoefDist( j, i ), 9, 'f', 6, QChar(' ') );
            text += "        ";
        }

        text += "\n";
    }

    _ui->textResults->setText( text );
}

////////////////////////////////////////////////////////////////////////////////

void DialogResults::on_listWidget_currentRowChanged( int currentRow )
{
    QString title = tr("Results");

    if ( currentRow != -1 )
    {
        title += " - " + _ui->listWidget->currentItem()->text();
    }

    setWindowTitle( title );

    updateAll();
}

////////////////////////////////////////////////////////////////////////////////

void DialogResults::on_sliderAoA_valueChanged( int value )
{
    double aoa = _wing->getAoADeg( value );

    _ui->spinBoxAoA->setValue( aoa );

    updateAll();
}
