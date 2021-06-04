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

    _scSave ( Q_NULLPTR ),

    _wing ( Q_NULLPTR ),

    _file_changed ( false )
{
    _ui->setupUi( this );

    _scSave = new QShortcut( QKeySequence(Qt::CTRL + Qt::Key_S), this, SLOT(on_actionFileSave_triggered()) );

    _wing = new Wing();

    settingsRead();
}

////////////////////////////////////////////////////////////////////////////////

MainWindow::~MainWindow()
{
    settingsSave();

    if ( _scSave ) delete _scSave;
    _scSave = Q_NULLPTR;

    if ( _ui ) delete _ui;
    _ui = Q_NULLPTR;

    if ( _wing ) delete _wing;
    _wing = Q_NULLPTR;
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::closeEvent( QCloseEvent *event )
{
    if ( _file_changed ) askIfSave();

    /////////////////////////////////
    QMainWindow::closeEvent( event );
    /////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::askIfSave()
{
    QString title = BSC_AERO_APP_NAME;
    QString text  = tr( "Do you want to save changes?" );

    QMessageBox::StandardButton result = QMessageBox::question( this, title, text,
                                                                QMessageBox::Yes | QMessageBox::No,
                                                                QMessageBox::Yes );

    if ( result == QMessageBox::Yes )
    {
        fileSave();
    }
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::fileOpen()
{
    QString caption = "Open...";
    QString dir = ( _fileName.length() > 0 ) ? QFileInfo( _fileName ).absolutePath() : "";
    QString filter;
    QString selectedFilter;

    filter += selectedFilter = "DAT (*.dat)";

    QString file = QFileDialog::getOpenFileName( this, caption, dir, filter, &selectedFilter );

    if ( file.length() > 0 )
    {
        _fileName = file;

        if ( _wing ) delete _wing;
        _wing = new Wing();

        if ( _wing->readFromFile( _fileName.toStdString().c_str() ) )
        {
            int spanwiseSteps   = _wing->getSectionsIterations();
            int fourierAccuracy = _wing->getFourierAccuracy();

            _ui->spinBoxAoASteps->setValue( _wing->getAoA_Iterations() );
            updateGroupBoxCompParams();

            _ui->spinBoxAoAStart  ->setValue( _wing->getAoA_Start() );
            _ui->spinBoxAoAFinish ->setValue( _wing->getAoA_Finish() );

            _ui->spinBoxAirDensity  ->setValue( _wing->getFluidDensity() );
            _ui->spinBoxAirVelocity ->setValue( _wing->getFluidVelocity() );

            _ui->spinBoxSpanwiseSteps->setValue( spanwiseSteps );
            _ui->spinBoxFourierAccuracy->setValue( fourierAccuracy );

            _file_changed = false;

            updateAll();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::fileNew()
{
    if ( _wing ) delete _wing;
    _wing = new Wing();

    _file_changed = false;

    _fileName = "";

    updateAll();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::fileSave()
{
    if ( _fileName.length() > 0 )
    {
        if ( _wing->writeToFile( _fileName.toStdString().c_str() ) )
        {
            _file_changed = false;
            updateAll();
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
    QString caption = "Save as...";
    QString dir = ( _fileName.length() > 0 ) ? QFileInfo( _fileName ).absolutePath() : ".";
    QString filter;
    QString selectedFilter;

    filter += selectedFilter = "DAT (*.dat)";

    QString newFile = QFileDialog::getSaveFileName( this, caption, dir, filter, &selectedFilter );

    if ( newFile.length() > 0 )
    {
        _fileName = newFile;

        if ( _wing->writeToFile( _fileName.toStdString().c_str() ) )
        {
            _file_changed = false;
            updateAll();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::sectionInsert()
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

            _file_changed = true;
        }
    }

    if ( dialog ) delete dialog;
    dialog = Q_NULLPTR;
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::sectionEdit()
{
    DialogSection *dialog = new DialogSection( this );

    int n = _wing->getSections();

    if ( n == 0 ) return;

    int row = _ui->tableWidgetSectionsData->currentRow();

    if ( row > 0 )
    {
        dialog->setSpanMin( _wing->getSectionY( row - 1 ) );
    }
    else
    {
        dialog->setSpanMin( 0.0 );
    }

    if ( row < n - 2 )
    {
        dialog->setSpanMax( _wing->getSectionY( row + 1 ) );
    }
    else
    {
        dialog->setSpanMax( 99.99 );
    }



    dialog->setSpan( _wing->getSectionY( row ) );
    dialog->setLE( _wing->getSectionLEX( row ) );
    dialog->setTE( _wing->getSectionTEX( row ) );
    dialog->setSlope( _wing->getSectionSlope( row ) );
    dialog->setAngle( _wing->getSectionAngle( row ) );

    if ( dialog->exec() == QDialog::Accepted )
    {
        bool result = _wing->editSectionData( dialog->getSpan(),
                                              dialog->getLE(),
                                              dialog->getTE(),
                                              dialog->getSlope(),
                                              dialog->getAngle(),
                                              row );

        if ( result )
        {
            updateAll();

            _file_changed = true;
        }
    }

    if ( dialog ) delete dialog;
    dialog = Q_NULLPTR;
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::sectionRemove()
{
    QString title = BSC_AERO_APP_NAME;
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

            _file_changed = true;
        }
    }
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

void MainWindow::setWingParameters()
{
    bool result = _wing->setParameters( _ui->spinBoxAoAStart        ->value(),
                                        _ui->spinBoxAoAFinish       ->value(),
                                        _ui->spinBoxAoASteps        ->value(),
                                        _ui->spinBoxAirVelocity     ->value(),
                                        _ui->spinBoxAirDensity      ->value(),
                                        _ui->spinBoxSpanwiseSteps   ->value(),
                                        _ui->spinBoxFourierAccuracy ->value() );

    if ( result )
    {
        _file_changed = true;
        updateAll();
    }
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::updateAll()
{
    QString title = BSC_AERO_APP_NAME;

    if ( _fileName.length() > 0 )
    {
        title += " - " + QFileInfo( _fileName ).fileName();
    }

    if ( _file_changed )
    {
        title += " (*)";
    }

    setWindowTitle( title );

    updateGraphicsViewPlanform();
    updateGroupBoxCompParams();
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

void MainWindow::updateGroupBoxCompParams()
{
    if ( _ui->spinBoxAoASteps->value() > 1 )
    {
        _ui->labelAoAFinish     ->setEnabled( true );
        _ui->spinBoxAoAFinish   ->setEnabled( true );
        _ui->labelUnitAoAFinish ->setEnabled( true );
    }
    else
    {
        _ui->labelAoAFinish     ->setEnabled( false );
        _ui->spinBoxAoAFinish   ->setEnabled( false );
        _ui->labelUnitAoAFinish ->setEnabled( false );

        _ui->spinBoxAoAFinish->setValue( _ui->spinBoxAoAStart->value() );
    }

    _ui->spinBoxAoAFinish->setMinimum( _ui->spinBoxAoAStart->value() );

    //_ui->spinBoxSpanwiseSteps->setMinimum( _wing->getSections() );
}

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
    if ( _file_changed ) askIfSave();

    fileNew();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_actionFileOpen_triggered()
{
    if ( _file_changed ) askIfSave();

    fileOpen();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_actionFileSave_triggered()
{
    fileSave();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_actionFileSaveAs_triggered()
{
    fileSaveAs();
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

void MainWindow::on_tableWidgetSectionsData_cellDoubleClicked( int /*row*/, int )
{
    sectionEdit();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_pushButtonSectionInsert_clicked()
{
    sectionInsert();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_pushButtonSectionEdit_clicked()
{
    sectionEdit();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_pushButtonSectionRemove_clicked()
{
    sectionRemove();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_spinBoxAoASteps_valueChanged( int arg1 )
{
    setWingParameters();

    if ( arg1 > 1 )
    {
        _ui->labelAoAFinish     ->setEnabled( true );
        _ui->labelUnitAoAFinish ->setEnabled( true );
        _ui->spinBoxAoAFinish   ->setEnabled( true );
    }
    else
    {
        _ui->labelAoAFinish     ->setEnabled( false );
        _ui->labelUnitAoAFinish ->setEnabled( false );
        _ui->spinBoxAoAFinish   ->setEnabled( false );
    }
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_spinBoxAoAStart_valueChanged( double /*arg1*/ )
{
    setWingParameters();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_spinBoxAoAFinish_valueChanged( double /*arg1*/ )
{
    setWingParameters();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_spinBoxAirDensity_valueChanged( double /*arg1*/ )
{
    setWingParameters();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_spinBoxAirVelocity_valueChanged( double /*arg1*/ )
{
    setWingParameters();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_spinBoxSpanwiseSteps_valueChanged( int arg1 )
{
    _ui->spinBoxFourierAccuracy->setMaximum( arg1 - 2 );

    setWingParameters();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_spinBoxFourierAccuracy_valueChanged( int /*arg1*/ )
{
    setWingParameters();
}
