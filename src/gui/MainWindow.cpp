/*******************************************************************************
*                                                                              *
* File:         MainWindow.cpp                                                 *
* Author(s):    Marek Cel                                                      *
*                                                                              *
********************************************************************************
*                                                                              *
* Copyright (C) 2010 Marek Cel (mcel@mcel.pl)                                  *
*                                                                              *
********************************************************************************
*                                                                              *
*  This file is part of BScAeroWing. You can redistribute and modify it under  *
*  the terms of GNU General Public License as published by the Free Software   *
*  Foundation; either version 3 of the License, or (at your option) any later  *
*  version.                                                                    *
*                                                                              *
*  This program is distributed in the hope that it will be useful, but         *
*  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY  *
*  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License     *
*  for more details.                                                           *
*                                                                              *
*  You should have received a copy of the GNU General Public License along     *
*  with this program; if not, write to the Free Software Foundation, Inc.      *
*  59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                   *
*                                                                              *
*  Further information about the GNU General Public License can also be found  *
*  on the world wide web at http://www.gnu.org.                                *
*                                                                              *
*******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
// INCLUDES                                                                   //
////////////////////////////////////////////////////////////////////////////////

#include <QCloseEvent>
#include <QMessageBox>
#include <QTime>

#include <ui/MainWindow.h>
#include <ui/ui_MainWindow.h>

#include <qwt_plot_curve.h>
#include <qwt_scale_engine.h>

////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS                                                           //
////////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow ( parent ),
    ui          ( new Ui::MainWindow ),

    wing             ( new BSc_Wing ),
    parameters_saved ( false ),
    file_changed     ( false ),
    results_uptodate ( false ),
    working_on_file  ( false )
{
    ui->setupUi(this);

    ui->tabWidget->setTabEnabled( 1, false );
    ui->tabWidget->setTabEnabled( 2, false );

    ui->plotWingGeo->axisScaleEngine( 0 )->setAttribute( QwtScaleEngine::Inverted, true );
    ui->plotWingGeo->setAxisTitle( 0, "X Coordinate [m]" );
    ui->plotWingGeo->setAxisTitle( 2, "Y Coordinate [m]" );
    ui->plotWingGeo->hide() ;

    ui->plotLiftVsAngle->setAxisTitle( 0, "Lift Coefficient [-]" ) ;
    ui->plotLiftVsAngle->setAxisTitle( 2, "Angle of Attack [deg]" ) ;

    ui->plotDragVsLift->setAxisTitle( 0, "Induced Drag Coefficient [-]" ) ;
    ui->plotDragVsLift->setAxisTitle( 2, "Lift Coefficient [-]" ) ;

    ui->plotCircDist->setAxisTitle( 0, "Circulation [m/s]" ) ;
    ui->plotCircDist->setAxisTitle( 2, "Wingspan (2y/b) [-]" ) ;

    ui->plotLiftDist->setAxisTitle( 0, "Lift Coefficient [1/m]" ) ;
    ui->plotLiftDist->setAxisTitle( 2, "Wingspan (2y/b) [-]" ) ;

    ui->plotVelDist->setAxisTitle( 0, "Induced Velocity [-]" ) ;
    ui->plotVelDist->setAxisTitle( 2, "Wingspan (2y/b) [-]" ) ;

    ui->plotDragDist->setAxisTitle( 0, "Induced Drag Coefficient [1/m]" ) ;
    ui->plotDragDist->setAxisTitle( 2, "Wingspan (2y/b) [-]" ) ;
}

////////////////////////////////////////////////////////////////////////////////

MainWindow::~MainWindow()
{
    if ( wing ) delete wing; wing = 0;

    delete ui;
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::show( void )
{
    QMainWindow::show();
    settingsRead();
}

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS                                                        //
////////////////////////////////////////////////////////////////////////////////

void MainWindow::closeEvent( QCloseEvent * event )
{
    if ( file_changed )
    {
        int iRetValue = messageDropChanges();

        if ( iRetValue == QMessageBox::Save )
        {
            fileSave();
            settingsWrite();
            QMainWindow::closeEvent( event );
        }
        else
        {
            settingsWrite();
            QMainWindow::closeEvent( event );
        }
    }
    else
    {
        settingsWrite();
        QMainWindow::closeEvent( event );
    }
}

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS                                                          //
////////////////////////////////////////////////////////////////////////////////

void MainWindow::addToReport( const char *field_name, bool success )
{
    QString tmp_text = ui->textReports->toPlainText();
    if ( tmp_text.length() > 0 )
        tmp_text.insert( tmp_text.length(), "\n" );
    if ( success )
        tmp_text.insert( tmp_text.length(), "[SUCCESS] Saving " );
    else
        tmp_text.insert( tmp_text.length(), "[ FAILURE ] Invalid value of " );
    tmp_text.insert( tmp_text.length(), field_name );
    ui->textReports->setText( tmp_text );
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::fileOpen()
{
    fileName = QFileDialog::getOpenFileName( this, tr("Open File"), "", tr("BScAeroWing Files (*.wng)\nAll Files (*.*)") );
    if ( fileName.length() > 0 )
    {
        if ( wing ) delete wing; wing = 0;
        wing = new BSc_Wing;

        if ( wing->readFromFile( fileName.toAscii() ) )
        {
            file_changed     = false;
            working_on_file  = true;
            results_uptodate = wing->isResultsUpToDate();
            parameters_saved = wing->isParametersSaved();
            updateAll();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::fileNew()
{
    if ( wing ) delete wing;
    wing = new BSc_Wing;
    file_changed     = false;
    working_on_file  = false;
    parameters_saved = false;
    results_uptodate = false;
    updateAll();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::fileSave()
{
    if ( working_on_file )
    {
        if ( wing->writeToFile( fileName.toAscii() ) )
        {
            file_changed = false;
        }
    }
    else
    {
        fileSaveAs();
    }
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::fileSaveAs()
{
    fileName = QFileDialog::getSaveFileName( this, tr("Save File As..."), "", tr("BScAeroWing Files (*.wng)") );

    if ( fileName.length() > 0 )
    {
        if ( wing->writeToFile( fileName.toAscii() ) )
        {
            file_changed    = false;
            working_on_file = true;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

int MainWindow::messageDropChanges()
{
    QString sTitle   = "Save Changes?";
    QString sMessage = "File has been modified.\nDo you want to save changes?";

    return QMessageBox::question( this, sTitle, sMessage, QMessageBox::Save, QMessageBox::Discard );
}

////////////////////////////////////////////////////////////////////////////////

int MainWindow::messageDeleteData()
{
    QString sTitle   = "Delete Data?";
    QString sMessage = "Are you sure you want to delete Wing Cross Section Data?";

    return QMessageBox::question( this, sTitle, sMessage, QMessageBox::Yes, QMessageBox::No );
}

////////////////////////////////////////////////////////////////////////////////

int MainWindow::messageInputData()
{
    QString sTitle   = "Wrong Cross Section Data";
    QString sMessage = "Following conditions of input data are required:\n";
    sMessage += "1. Wingspan Y Coordinate of the first cross section should be 0.0,\n";
    sMessage += "2. Wingspan Y Coordinate should be unique,\n";
    sMessage += "3. Trailing Edge X Coordinate should be greater than Leading Edge X Coordinate,\n";
    sMessage += "4. There can be maximum 1,000 Cross Sections data sets";

    return QMessageBox::warning( this, sTitle, sMessage );
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::settingsRead( void )
{
    QSettings settings( BSC_AERO_ORG_NAME, BSC_AERO_APP_NAME );
    
    int w = 800;
    int h = 600;

    resize( settings.value( "mainwindow/size", QSize(w, h) ).toSize() );
    move( settings.value( "mainwindow/pos", QPoint(100, 100) ).toPoint() );
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::settingsWrite( void )
{
    QSettings settings( BSC_AERO_ORG_NAME, BSC_AERO_APP_NAME );

    settings.setValue( "mainwindow/size", size() );
    settings.setValue( "mainwindow/pos",  pos()  );
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::updateAll()
{
    updateButtonNext1();
    updateButtonNext2();
    updateGroupAccuracy();
    updateGroupAngleOfAttack();
    updateGroupComputations();
    updateGroupSectionEditor();
    updateGroupSpanInterpolation();
    updatePlotWingGeo();
    updateResults();
    updateTableWingData();
    updateTabs();
    updateWingData();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::updateButtonNext1()
{
    if ( wing && wing->getSections() > 1 )
        ui->buttonNext1->setEnabled( true ) ;
    else
        ui->buttonNext1->setEnabled( false ) ;
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::updateButtonNext2()
{
    if ( results_uptodate )
        ui->buttonNext2->setEnabled( true ) ;
    else
        ui->buttonNext2->setEnabled( false ) ;
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::updateGroupAccuracy()
{
    ui->spinFourierAccuracy->setMaximum( ui->spinInterSections->value() - 1 ) ;
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::updateGroupAngleOfAttack()
{
    if ( ui->spinAngleStart->value() != ui->spinAngleFinish->value() )
    {
        ui->spinAngleIter->setMaximum( 100 );
        ui->spinAngleIter->setMinimum( 2 );

        double step = ( ui->spinAngleFinish->value() - ui->spinAngleStart->value() ) / (double)( ui->spinAngleIter->value() - 1 );
        ui->spinAngleStep->setValue( step );
    }
    else
    {
        ui->spinAngleIter->setMaximum( 1 );
        ui->spinAngleIter->setMinimum( 1 );

        ui->spinAngleStep->setValue( 0.0 );
    }

    ui->spinAngleFinish->setMinimum( ui->spinAngleStart->value() );
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::updateGroupComputations()
{
    if ( parameters_saved )
    {
        ui->buttonCompute->setEnabled( true );
    }
    else
    {
        ui->buttonCompute->setEnabled( false );
    }
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::updateGroupSectionEditor()
{
    ui->spinWingSpan->setValue( 0.0 );
    ui->spinLeadingX->setValue( 0.0 );
    ui->spinTrailingX->setValue( 0.0 );
    ui->spinLiftSlope->setValue( 0.0 );
    ui->spinAngle0Lift->setValue( 0.0 );

    ui->buttonSaveSection->setEnabled( false );
    ui->buttonDeleteSection->setEnabled( false );

    if ( wing->getSections() > 99 )
        ui->buttonAddSection->setEnabled( false );
    else
        ui->buttonAddSection->setEnabled( true );
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::updateGroupSpanInterpolation()
{
    ui->spinInterSections->setMinimum( wing->getSections() );
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::updatePlotWingGeo()
{
    double *span_y;
    double *le_x;
    double *te_x;

    // clearing current plot
    ui->plotWingGeo->clear();

    // show plot if required numbers of sections, otherwise hide
    if ( wing->getSections() > 1 )
        ui->plotWingGeo->show();
    else
        ui->plotWingGeo->hide();

    // creating arrays of size eq. to number of cross sections
    span_y = new double[wing->getSections()];
    le_x   = new double[wing->getSections()];
    te_x   = new double[wing->getSections()];

    QwtPlotCurve* le_curve = new QwtPlotCurve( "Leading Edge");
    QwtPlotCurve* te_curve = new QwtPlotCurve( "Trailing Edge");

    // comping cross sections data into arrays
    for ( int i = 0; i < wing->getSections(); i++ )
    {
        span_y[i] = wing->getSectionY(i);
        le_x[i]   = wing->getSectionLEX(i);
        te_x[i]   = wing->getSectionTEX(i);
    }

    // setting curves data
    le_curve->setData( span_y, le_x, wing->getSections() );
    te_curve->setData( span_y, te_x, wing->getSections() );

    le_curve->setPen( QPen::QPen( QColor::QColor( 0, 0, 255 ), 2 ) );
    te_curve->setPen( QPen::QPen( QColor::QColor( 255, 0, 0 ), 2 ) );

    // inserting curves into plot
    le_curve->attach( ui->plotWingGeo );
    te_curve->attach( ui->plotWingGeo );

    // reploting plot
    ui->plotWingGeo->replot();

    delete [] span_y;
    delete [] le_x;
    delete [] te_x;
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::updateResults()
{
    ui->plotLiftVsAngle->clear();
    ui->tableLiftVsAngle->clearContents();
    for ( int i = ui->tableLiftVsAngle->rowCount() - 1; i >= 0  ; i-- )
    {
        ui->tableLiftVsAngle->removeRow( i );
    }
    ui->plotDragVsLift->clear();
    ui->tableDragVsLift->clearContents();
    for ( int i = ui->tableDragVsLift->rowCount() - 1; i >= 0  ; i-- )
    {
        ui->tableDragVsLift->removeRow( i );
    }

    if ( results_uptodate )
    {
        ui->sliderAoA->setMaximum( wing->getAoA_Iterations() - 1 );

        double *drag_coef;
        double *lift_coef;
        double *aoa_deg;

        // creating Angle of Attack independent arrays
        drag_coef = new double[ wing->getAoA_Iterations() ];
        lift_coef = new double[ wing->getAoA_Iterations() ];
        aoa_deg   = new double[ wing->getAoA_Iterations() ];

        // getting values of arrays
        for ( int i = 0; i < wing->getAoA_Iterations(); i++ )
        {
            drag_coef[i] = wing->getDragCoef( i );
            lift_coef[i] = wing->getLiftCoef( i );
            aoa_deg[i]   = wing->getAoADeg( i );
        }

        // refreshing plotLiftVsAngle
        QwtPlotCurve* lift_coef_curve = new QwtPlotCurve( "Lift Coefficient" );
        lift_coef_curve->setData( aoa_deg, lift_coef, wing->getAoA_Iterations() );
        lift_coef_curve->setPen( QPen::QPen( QColor::QColor( 0, 0, 0 ), 2 ) );
        lift_coef_curve->attach( ui->plotLiftVsAngle );
        ui->plotLiftVsAngle->replot();

        // refreshing tableLiftVsAngle
        for ( int i = 0; i < wing->getAoA_Iterations(); i++ )
        {
            ui->tableLiftVsAngle->insertRow( i );
            ui->tableLiftVsAngle->setItem( i, 0, new QTableWidgetItem( QString::number( wing->getAoADeg(i),'f' ) ) );
            ui->tableLiftVsAngle->setItem( i, 1, new QTableWidgetItem( QString::number( wing->getLiftCoef(i),'f' ) ) );
        }

        // refreshing plotDragVsLift
        QwtPlotCurve* drag_coef_curve = new QwtPlotCurve( "Drag Coefficient" );
        drag_coef_curve->setData( lift_coef, drag_coef, wing->getAoA_Iterations() );
        drag_coef_curve->setPen( QPen::QPen( QColor::QColor( 0, 0, 0 ), 2 ) );
        drag_coef_curve->attach( ui->plotDragVsLift );
        ui->plotDragVsLift->replot();

        // refreshing tableDragVsLift
        for ( int i = 0; i < wing->getAoA_Iterations(); i++ )
        {
            ui->tableDragVsLift->insertRow( i );
            ui->tableDragVsLift->setItem( i, 0, new QTableWidgetItem( QString::number( wing->getLiftCoef(i),'f' ) ) );
            ui->tableDragVsLift->setItem( i, 1, new QTableWidgetItem( QString::number( wing->getDragCoef(i),'f' ) ) );
        }

        delete [] drag_coef;
        delete [] lift_coef;
        delete [] aoa_deg;

        updateResultsDistributions();
    }

}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::updateResultsDistributions()
{
    ui->plotCircDist->clear();
    ui->plotLiftDist->clear();
    ui->plotVelDist->clear();
    ui->plotDragDist->clear();

    ui->tableCircDist->clearContents();
    for ( int i = ui->tableCircDist->rowCount() - 1; i >= 0  ; i-- )
    {
        ui->tableCircDist->removeRow( i );
    }

    ui->tableLiftDist->clearContents();
    for ( int i = ui->tableLiftDist->rowCount() - 1; i >= 0  ; i-- )
    {
        ui->tableLiftDist->removeRow( i );
    }

    ui->tableVelDist->clearContents();
    for ( int i = ui->tableVelDist->rowCount() - 1; i >= 0  ; i-- )
    {
        ui->tableVelDist->removeRow( i );
    }

    if ( results_uptodate )
    {
        int aoa_index = ui->sliderAoA->value();
        ui->spinCurrentAoA->setValue( wing->getAoADeg( aoa_index ) );

        double *wingspan_coef;
        double *wingspan_coef_extip;
        double *gamma;
        double *lift;
        double *velocity;
        double *drag;

        wingspan_coef       = new double[ wing->getSectionsFullspan() ];
        wingspan_coef_extip = new double[ wing->getSectionsFullspan() - 2 ];
        gamma               = new double[ wing->getSectionsFullspan() ];
        lift                = new double[ wing->getSectionsFullspan() ];
        velocity            = new double[ wing->getSectionsFullspan() - 2 ];
        drag                = new double[ wing->getSectionsFullspan() - 2 ];

        for ( int i = 0; i < wing->getSectionsFullspan(); i++ )
        {
            wingspan_coef[i] = wing->getWingSpanCoef( i ) ;
            gamma[i]         = wing->getGamma( aoa_index, i ) ;
            lift[i]          = wing->getLiftCoefDist( aoa_index, i ) ;
        }

        // refreshing plotCircDist
        QwtPlotCurve* circ_dist_curve = new QwtPlotCurve( "Circulation Distribution" );
        circ_dist_curve->setData( wingspan_coef, gamma, wing->getSectionsFullspan() );
        circ_dist_curve->setPen( QPen::QPen( QColor::QColor( 0, 0, 0 ), 2 ) );
        circ_dist_curve->attach( ui->plotCircDist );
        ui->plotCircDist->replot();

        // refreshing tableCircDist
        for ( int i = 0; i < wing->getSectionsFullspan(); i++ )
        {
            ui->tableCircDist->insertRow( i );
            ui->tableCircDist->setItem( i, 0, new QTableWidgetItem( QString::number( wingspan_coef[i],'f' ) ) );
            ui->tableCircDist->setItem( i, 1, new QTableWidgetItem( QString::number( gamma[i],'f' ) ) );
        }

        // refreshing plotLiftDist
        QwtPlotCurve* lift_dist_curve = new QwtPlotCurve( "Lift Coefficient Distribution" );
        lift_dist_curve->setData( wingspan_coef, lift, wing->getSectionsFullspan() );
        lift_dist_curve->setPen( QPen::QPen( QColor::QColor( 0, 0, 0 ), 2 ) );
        lift_dist_curve->attach( ui->plotLiftDist );
        ui->plotLiftDist->replot();

        // refreshing tableLiftDist
        for ( int i = 0; i < wing->getSectionsFullspan(); i++ )
        {
            ui->tableLiftDist->insertRow( i );
            ui->tableLiftDist->setItem( i, 0, new QTableWidgetItem( QString::number( wingspan_coef[i],'f' ) ) );
            ui->tableLiftDist->setItem( i, 1, new QTableWidgetItem( QString::number( lift[i],'f' ) ) );
        }

        for ( int i = 1; i < wing->getSectionsFullspan() - 1; i++ )
        {
            wingspan_coef_extip[i-1] = wing->getWingSpanCoef( i );
            velocity[i-1]            = wing->getVelocityDist( aoa_index, i );
            drag[i-1]                = wing->getDragCoefDist( aoa_index, i );
        }

        // refreshing plotVelDist
        QwtPlotCurve* vel_dist_curve = new QwtPlotCurve( "Induced Velocity Distribution" );
        vel_dist_curve->setData( wingspan_coef_extip, velocity, wing->getSectionsFullspan() - 2 );
        vel_dist_curve->setPen( QPen::QPen( QColor::QColor( 0, 0, 0 ), 2 ) );
        vel_dist_curve->attach( ui->plotVelDist );
        ui->plotVelDist->replot();

        // refreshing tableVelDist
        for ( int i = 1; i < wing->getSectionsFullspan() - 2; i++ )
        {
            ui->tableVelDist->insertRow( i - 1 );
            ui->tableVelDist->setItem( i - 1, 0, new QTableWidgetItem( QString::number( wingspan_coef_extip[i],'f' ) ) );
            ui->tableVelDist->setItem( i - 1, 1, new QTableWidgetItem( QString::number( velocity[i],'f' ) ) );
        }

        // refreshing plotDragDist
        QwtPlotCurve* drag_dist_curve = new QwtPlotCurve( "Induced Drag Coefficient Distribution" );
        drag_dist_curve->setData( wingspan_coef_extip, drag, wing->getSectionsFullspan() - 2 );
        drag_dist_curve->setPen( QPen::QPen( QColor::QColor( 0, 0, 0 ), 2 ) );
        drag_dist_curve->attach( ui->plotDragDist );
        ui->plotDragDist->replot();

        // refreshing tableDragDist
        for ( int i = 1; i < wing->getSectionsFullspan() - 2; i++ )
        {
            ui->tableDragDist->insertRow( i - 1 );
            ui->tableDragDist->setItem( i - 1, 0, new QTableWidgetItem( QString::number( wingspan_coef_extip[i],'f' ) ) );
            ui->tableDragDist->setItem( i - 1, 1, new QTableWidgetItem( QString::number( drag[i],'f' ) ) );
        }

        delete [] wingspan_coef;
        delete [] wingspan_coef_extip;
        delete [] gamma;
        delete [] lift;
        delete [] velocity;
        delete [] drag;
    }
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::updateTableWingData()
{
    // removing all rows
    ui->tableWingData->clearContents();
    for ( int i = ui->tableWingData->rowCount() - 1; i >= 0  ; i-- )
    {
        ui->tableWingData->removeRow( i );
    }

    // adding new rows
    for ( int i = 0; i < wing->getSections(); i++ )
    {
        ui->tableWingData->insertRow( i );
        ui->tableWingData->setItem( i, 0, new QTableWidgetItem( QString::number( wing->getSectionY(i),'f' ) ) );
        ui->tableWingData->setItem( i, 1, new QTableWidgetItem( QString::number( wing->getSectionLEX(i),'f' ) ) );
        ui->tableWingData->setItem( i, 2, new QTableWidgetItem( QString::number( wing->getSectionTEX(i),'f' ) ) );
        ui->tableWingData->setItem( i, 3, new QTableWidgetItem( QString::number( wing->getSectionSlope(i),'f' ) ) );
        ui->tableWingData->setItem( i, 4, new QTableWidgetItem( QString::number( wing->getSectionAngle(i),'f' ) ) );
    }
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::updateTabs()
{
    if ( wing->getSections() > 1 )
    {
        ui->tabWidget->setTabEnabled( 1, true );
    }
    else
    {
        if ( ui->tabWidget->currentIndex() != 0 ) ui->tabWidget->setCurrentIndex( 0 );
        ui->tabWidget->setTabEnabled( 1, false );
    }

    if ( results_uptodate )
    {
        ui->tabWidget->setTabEnabled( 2, true );
    }
    else
    {
        if ( ui->tabWidget->currentIndex() > 1 ) ui->tabWidget->setCurrentIndex( 1 );
        ui->tabWidget->setTabEnabled( 2, false );
    }

}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::updateWingData()
{
    if ( wing->getSections() > 1 )
    {
        ui->spinTotalSpan->setValue( wing->getWingSpan() );
        ui->spinTaperRatio->setValue( wing->getTaperRatio() );
        ui->spinWingArea->setValue( wing->getWingArea() );
        ui->spinAspectRatio->setValue( wing->getAspectRatio() );
    }
    else
    {
        ui->spinTotalSpan->setValue( 0.0 );
        ui->spinTaperRatio->setValue( 0.0 );
        ui->spinWingArea->setValue( 0.0 );
        ui->spinAspectRatio->setValue( 0.0 );
    }
}

////////////////////////////////////////////////////////////////////////////////
// PRIVATE SLOTS                                                              //
////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_actionAbout_Qt_activated()
{
    QMessageBox::aboutQt( this );
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_actionAbout_BScAeroWing_activated()
{
    QString sTitle = "About BScAeroWing";
    QString sAbout = "BScAeroWing is free software for estimating basic aerodynamic characteristics of an aircraft wing.\n\n";
    sAbout += "This software is available under the terms of GNU General Public License.\n\n";
    sAbout += "This software is a part of Bechelor's thesis made at Faculty of Power and Aerospace Engineering of Warsaw University of Technology under supervision of Zbigniew Paturski, Ph.D.\n\n";
    sAbout += "For more information visit:\n";
    sAbout += "\thttp://marekcel.pl/bscaerowing\n";
    sAbout += "\thttp://www.meil.pw.edu.pl/\n";
    sAbout += "\nCredits:\n";
    sAbout += "\tZbigniew Paturski, Ph.D. (patur@meil.pw.edu.pl),\n";
    sAbout += "\tFranciszek Dul, Ph.D. (fdul@meil.pw.edu.pl),\n";
    sAbout += "\tMarek Cel, MSc (marekcel@marekcel.pl)";

    QMessageBox::about( this, sTitle, sAbout );
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_actionExit_activated()
{
    close();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_actionNew_activated()
{
    if ( file_changed )
    {
        int iRetValue = messageDropChanges();

        if ( iRetValue == QMessageBox::Save )
        {
            fileSave();
            fileNew();
        }
        else
        {
            fileNew();
        }
    }
    else
    {
        fileNew();
    }
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_actionOpen_activated()
{
    if ( file_changed )
    {
        int iRetValue = messageDropChanges();

        if ( iRetValue == QMessageBox::Save )
        {
            fileSave();
            fileOpen();
        }
        else
        {
            fileOpen();
        }
    }
    else
    {
        fileOpen();
    }
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_actionSave_activated()
{
    fileSave();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_actionSave_As_activated()
{
    fileSaveAs();
}

////////////////////////////////////////////////////////////////////////////////


void MainWindow::on_buttonAddSection_clicked()
{
    if ( !wing ) wing = new BSc_Wing;

    bool result = wing->addSectionData( ui->spinWingSpan->value(),
                                        ui->spinLeadingX->value(),
                                        ui->spinTrailingX->value(),
                                        ui->spinLiftSlope->value(),
                                        ui->spinAngle0Lift->value() );

    if ( result )
    {
        updateAll();
        parameters_saved = false;
        file_changed     = true;
    }
    else
    {
        messageInputData();
    }
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_buttonCompute_clicked()
{
    QString tmp_text = ui->textReports->toPlainText();

    // starting QTime
    QTime qtime;
    qtime.start();
    QString cur_time = qtime.toString("hh:mm:ss.zzz");

    // writing computations' start time and announcement
    tmp_text.insert( tmp_text.length(), "\n\n[" );
    tmp_text.insert( tmp_text.length(), cur_time );
    tmp_text.insert( tmp_text.length(), "] Computations started..." );
    ui->textReports->setText( tmp_text );

    bool success = wing->compute();

    if ( success )
    {
        qtime.start();
        cur_time = qtime.toString("hh:mm:ss.zzz");
        tmp_text.insert( tmp_text.length(), "\n[" );
        tmp_text.insert( tmp_text.length(), cur_time );
        tmp_text.insert( tmp_text.length(), "][SUCCESS] Computations finished." );
        ui->textReports->setText( tmp_text ) ;
        results_uptodate = true;
        file_changed = true;
        updateAll();
    }
    else
    {
        qtime.start() ;
        cur_time = qtime.toString("hh:mm:ss.zzz") ;
        tmp_text.insert( tmp_text.length(), "\n[" ) ;
        tmp_text.insert( tmp_text.length(), cur_time ) ;
        tmp_text.insert( tmp_text.length(), "][ FAILURE ] Computations stopped. Error on Wing::computeCharacteristics()." ) ;
        ui->textReports->setText( tmp_text ) ;
    }
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_buttonDeleteSection_clicked()
{
    int iRetVal = messageDeleteData();

    if ( iRetVal == QMessageBox::Yes )
    {
        bool result = wing->deleteSectionData( ui->tableWingData->currentRow() );

        if ( result )
        {
            updateAll();
            parameters_saved = false;
            file_changed     = true;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_buttonNext1_clicked()
{
    ui->tabWidget->setCurrentIndex( 1 );
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_buttonNext2_clicked()
{
    ui->tabWidget->setCurrentIndex( 2 );
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_buttonSaveParams_clicked()
{
    // reset textReports
    ui->textReports->setText("") ;

    // AoA Iteration Start Value cannot be greater than AoA Iteration Finish Value
    if ( ui->spinAngleStart->value() < ui->spinAngleFinish->value() ) {

        addToReport( "Angle of Attack Iteration Start Value", true );
        addToReport( "Angle of Attack Iteration Finish Value", true );

        // if AoA Iteration Start Value eq. to AoA Iteration Finish Value
        // AoA Number of Iterations should be 1
        if ( ui->spinAngleIter->value() > 1 )
            addToReport( "Angle of Attack Number of Iterations", true );
        else {
            addToReport( "Angle of Attack Number of Iterations", false );
            return;
        }
    } else if ( ui->spinAngleStart->value() == ui->spinAngleFinish->value() ) {

        addToReport( "Angle of Attack Iteration Start Value", true );
        addToReport( "Angle of Attack Iteration Finish Value", true );

        // if AoA Iteration Start Value less than AoA Iteration Finish Value
        // AoA Number of Iterations should be greater than 1
        if ( ui->spinAngleIter->value() == 1 )
            addToReport( "Angle of Attack Number of Iterations", true );
        else {
            addToReport( "Angle of Attack Number of Iterations", false );
            return;
        }
    } else {

        addToReport( "Angle of Attack Iteration Start Value", false );
        return;
    }

    // Fluid Velocity should be greater than 0.0
    if ( ui->spinFluidVelocity->value() > 0.0 ) {
        addToReport( "Fluid Velocity", true );
    } else {
        addToReport( "Fluid Velocity", false );
        return;
    }

    // Fluid Density should be greater than 0.0
    if ( ui->spinFluidDensity->value() > 0.0 ) {
        addToReport( "Fluid Density", true );
    } else {
        addToReport( "Fluid Density", false );
        return;
    }

    // Number of Cross Sections should be greater or equal to Wing::sections
    if ( ui->spinInterSections->value() >= wing->getSections() ) {
        addToReport( "Number of Cross Sections", true );
    } else {
        addToReport( "Number of Cross Sections", false );
        return;
    }

    // Fourier Series Accuracy should be smaller than Number of Cross Section less by 1
    if ( ui->spinInterSections->value() > ui->spinFourierAccuracy->value() ) {
        addToReport( "Fourier Series Accuracy", true );
    } else {
        addToReport( "Fourier Series Accuracy", false );
        return;
    }

    bool result = wing->setParameters( ui->spinAngleStart->value(),
                                       ui->spinAngleFinish->value(),
                                       ui->spinAngleIter->value(),
                                       ui->spinFluidVelocity->value(),
                                       ui->spinFluidDensity->value(),
                                       ui->spinInterSections->value(),
                                       ui->spinFourierAccuracy->value() );
    if ( result ) {
        parameters_saved = true;
        file_changed     = true;
        results_uptodate = false;
        addToReport( "ALL parameters succeed.", true );
        updateAll();
    } else {
        addToReport( "some parameters failed. Error on Wing::setParameter(..).", false );
    }
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_buttonSaveSection_clicked()
{
    bool result = wing->editSectionData( ui->spinWingSpan->value(),
                                         ui->spinLeadingX->value(),
                                         ui->spinTrailingX->value(),
                                         ui->spinLiftSlope->value(),
                                         ui->spinAngle0Lift->value(),
                                         ui->tableWingData->currentRow() );

    if ( result )
    {
        results_uptodate = false;
        file_changed     = true;
        updateAll();
    }
    else
    {
        messageInputData();
    }
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_sliderAoA_valueChanged(int value)
{
    updateResultsDistributions();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_spinAngleFinish_valueChanged(double )
{
    parameters_saved = false;
    results_uptodate = false;
    updateAll();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_spinAngleIter_valueChanged(int )
{
    parameters_saved = false;
    results_uptodate = false;
    updateAll();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_spinAngleStart_valueChanged(double )
{
    parameters_saved = false;
    results_uptodate = false;
    updateAll();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_spinFluidDensity_valueChanged(double )
{
    parameters_saved = false;
    results_uptodate = false;
    updateAll();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_spinFluidVelocity_valueChanged(double )
{
    parameters_saved = false;
    results_uptodate = false;
    updateAll();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_spinFourierAccuracy_valueChanged(int )
{
    parameters_saved = false;
    results_uptodate = false;
    updateAll();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_spinInterSections_valueChanged(int )
{
    parameters_saved = false;
    results_uptodate = false;
    updateAll();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_tableWingData_currentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous)
{
    if ( wing && ui->tableWingData->currentRow() >= 0 )
    {
        ui->spinWingSpan->setValue( wing->getSectionY( ui->tableWingData->currentRow() ) );
        ui->spinLeadingX->setValue( wing->getSectionLEX( ui->tableWingData->currentRow() ) );
        ui->spinTrailingX->setValue( wing->getSectionTEX( ui->tableWingData->currentRow() ) );
        ui->spinLiftSlope->setValue( wing->getSectionSlope( ui->tableWingData->currentRow() ) );
        ui->spinAngle0Lift->setValue( wing->getSectionAngle( ui->tableWingData->currentRow() ) );

        ui->buttonSaveSection->setEnabled( true );
        ui->buttonDeleteSection->setEnabled( true );
    }
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_tableWingData_itemSelectionChanged()
{
    ui->tableWingData->selectRow( ui->tableWingData->currentRow() );
}

////////////////////////////////////////////////////////////////////////////////
// END OF FILE                                                                //
////////////////////////////////////////////////////////////////////////////////
