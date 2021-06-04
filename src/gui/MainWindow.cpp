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

#include <gui/MainWindow.h>
#include <ui_MainWindow.h>

#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>

#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_scale_engine.h>
#include <qwt_symbol.h>

#include <defs.h>

#include <gui/DialogSection.h>

////////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow( QWidget *parent ) :
    QMainWindow ( parent ),
    _ui ( new Ui::MainWindow ),

    _wing ( Q_NULLPTR ),

    _params_saved ( false ),
    _file_changed ( false )
{
    _ui->setupUi( this );

    _wing = new Wing();

    settingsRead();

    //_ui->plotPlanform->enableAxis( 0, false );
    //_ui->plotPlanform->enableAxis( 2, false );
}

////////////////////////////////////////////////////////////////////////////////

MainWindow::~MainWindow()
{
    settingsSave();

    if ( _ui ) delete _ui;
    _ui = Q_NULLPTR;

    if ( _wing ) delete _wing;
    _wing = Q_NULLPTR;
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::closeEvent( QCloseEvent *event )
{
    /////////////////////////////////
    QMainWindow::closeEvent( event );
    /////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::settingsRead()
{
    QSettings settings( BSC_AERO_ORG_NAME, BSC_AERO_APP_NAME );

    settings.beginGroup( "main_window" );

    restoreState( settings.value( "state" ).toByteArray() );
    restoreGeometry( settings.value( "geometry" ).toByteArray() );

    settings.endGroup();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::settingsSave()
{
    QSettings settings( BSC_AERO_ORG_NAME, BSC_AERO_APP_NAME );

    settings.beginGroup( "main_window" );

    settings.setValue( "state", saveState() );
    settings.setValue( "geometry", saveGeometry() );

    settings.endGroup();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::updateAll()
{
    updateGraphicsViewPlanform();
    //updatePlotPlanform();
    updateTableWidgetSectionsData();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::updateGraphicsViewPlanform()
{
    std::vector< GraphicsPlanform::Data > vd;

    for ( int i = 0; i < _wing->getSections(); i++ )
    {
        GraphicsPlanform::Data data;

        data.y    = _wing->getSectionY   ( i );
        data.x_le = _wing->getSectionLEX ( i );
        data.x_te = _wing->getSectionTEX ( i );

        vd.push_back( data );
    }

    _ui->graphicsViewPlanform->setData( vd );
}

////////////////////////////////////////////////////////////////////////////////

//void MainWindow::updatePlotPlanform()
//{
//    // clearing current plot
//    _ui->plotPlanform->detachItems( QwtPlotItem::Rtti_PlotCurve  , true );
//    _ui->plotPlanform->detachItems( QwtPlotItem::Rtti_PlotMarker , true );
//
//    // creating arrays of size eq. to number of cross sections
//    QVector< double > span;
//    QVector< double > le_x;
//    QVector< double > te_x;
//
//    QwtPlotCurve* le_curve = new QwtPlotCurve( "Leading Edge"  );
//    QwtPlotCurve* te_curve = new QwtPlotCurve( "Trailing Edge" );
//
//    // comping cross sections data into arrays
//    for ( int i = 0; i < _wing->getSections(); i++ )
//    {
//        span.push_back(  _wing->getSectionY   ( i ) );
//        le_x.push_back( -_wing->getSectionLEX ( i ) );
//        te_x.push_back( -_wing->getSectionTEX ( i ) );
//    }
//
//    // setting curves data
//    le_curve->setSamples( span, le_x );
//    te_curve->setSamples( span, te_x );
//
//    le_curve->setPen( QPen( Qt::blue , 2 ) );
//    te_curve->setPen( QPen( Qt::red  , 2 ) );
//
//    // inserting curves into plot
//    le_curve->attach( _ui->plotPlanform );
//    te_curve->attach( _ui->plotPlanform );
//
//    // reploting plot
//    _ui->plotPlanform->replot();
//}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::updateTableWidgetSectionsData()
{
    // removing all rows
    _ui->tableWidgetSectionsData->clearContents();

    for ( int i = _ui->tableWidgetSectionsData->rowCount() - 1; i >= 0  ; i-- )
    {
        _ui->tableWidgetSectionsData->removeRow( i );
    }

    // adding new rows
    for ( int i = 0; i < _wing->getSections(); i++ )
    {
        _ui->tableWidgetSectionsData->insertRow( i );
        _ui->tableWidgetSectionsData->setItem( i, 0, new QTableWidgetItem( QString::number( _wing->getSectionY     (i),'f' ) ) );
        _ui->tableWidgetSectionsData->setItem( i, 1, new QTableWidgetItem( QString::number( _wing->getSectionLEX   (i),'f' ) ) );
        _ui->tableWidgetSectionsData->setItem( i, 2, new QTableWidgetItem( QString::number( _wing->getSectionTEX   (i),'f' ) ) );
        _ui->tableWidgetSectionsData->setItem( i, 3, new QTableWidgetItem( QString::number( _wing->getSectionSlope (i),'f' ) ) );
        _ui->tableWidgetSectionsData->setItem( i, 4, new QTableWidgetItem( QString::number( _wing->getSectionAngle (i),'f' ) ) );
    }
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_actionFileNew_triggered()
{

}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_actionFileOpen_triggered()
{

}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_actionFileSave_triggered()
{

}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_actionFileSaveAs_triggered()
{

}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_actionExit_triggered()
{
    close();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_actionAbout_triggered()
{
    QFile aboutHtmlFile( ":/gui/html/about.html" );

    QString aboutWinTitle;
    QString aboutInfoText;

    aboutWinTitle = tr( "About" );

    if ( aboutHtmlFile.open( QIODevice::ReadOnly ) )
    {
        aboutInfoText = aboutHtmlFile.readAll();
        aboutHtmlFile.close();
    }

    QMessageBox::about( this, aboutWinTitle, aboutInfoText );
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_tableWidgetSectionsData_currentCellChanged( int row, int, int, int )
{
    _ui->graphicsViewPlanform->setSelected( row );

    if ( row >= 0 && row < _ui->tableWidgetSectionsData->rowCount() )
    {
        _ui->pushButtonSectionEdit   ->setEnabled( true );
        _ui->pushButtonSectionRemove ->setEnabled( true );
    }
    else
    {
        _ui->pushButtonSectionEdit   ->setEnabled( false );
        _ui->pushButtonSectionRemove ->setEnabled( false );
    }
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_pushButtonSectionInsert_clicked()
{
    if ( !_wing ) _wing = new Wing();

    DialogSection *dialog = new DialogSection( this );

    int n = _wing->getSections();

    if ( n == 0 )
    {
        dialog->setSpanMin( 0.0 );
        dialog->setSpanMax( 0.0 );
        dialog->setSpanDisabled( true );
    }
    else
    {
        int row = _ui->tableWidgetSectionsData->currentRow();

        if ( row == -1 || row == n - 1 )
        {
            dialog->setSpanMin( _wing->getSectionY( n - 1 ) );
        }
        else
        {
            dialog->setSpanMin( _wing->getSectionY( row - 1 ) );
            dialog->setSpanMax( _wing->getSectionY( row ) );
        }
    }

    if ( dialog->exec() == QDialog::Accepted )
    {
        bool result = _wing->addSectionData( dialog->getSpan(),
                                             dialog->getLE(),
                                             dialog->getTE(),
                                             dialog->getSlope(),
                                             dialog->getAngle() );

        if ( result )
        {
            updateAll();

            _params_saved = false;
            _file_changed = true;
        }
    }

    if ( dialog ) delete dialog;
    dialog = Q_NULLPTR;
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_pushButtonSectionEdit_clicked()
{

}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_pushButtonSectionRemove_clicked()
{
    QString title = windowTitle();
    QString text  = tr( "Do you want to remove section data?" );

    QMessageBox::StandardButton result = QMessageBox::question( this, title, text,
                                                                QMessageBox::Yes | QMessageBox::No,
                                                                QMessageBox::No );

    if ( result == QMessageBox::Yes )
    {
        bool result = _wing->deleteSectionData( _ui->tableWidgetSectionsData->currentRow() );

        if ( result )
        {
            updateAll();

            _params_saved = false;
            _file_changed = true;
        }
    }
}
