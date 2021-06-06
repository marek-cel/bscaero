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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

////////////////////////////////////////////////////////////////////////////////

#include <QMainWindow>
#include <QSettings>
#include <QShortcut>

#include <Wing.h>

#include <gui/DialogResults.h>
#include <gui/DialogSection.h>

////////////////////////////////////////////////////////////////////////////////

namespace Ui
{
    class MainWindow;
}

////////////////////////////////////////////////////////////////////////////////

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow( QWidget *parent = Q_NULLPTR );

    ~MainWindow();

protected:

    void closeEvent( QCloseEvent *event );

private:

    Ui::MainWindow *_ui;            ///< UI object

    DialogResults *_dialogResults;  ///<

    QShortcut *_scSave;             ///< key shortcut - save

    QString _fileName;              ///< current file name

    Wing *_wing;                    ///< main wing object

    bool _file_changed;             ///< specifies if current wing geo data, computation parameters and results has been saved to file

    void askIfSave();

    void fileOpen();
    void fileNew();
    void fileSave();
    void fileSaveAs();

    void sectionInsert();
    void sectionEdit();
    void sectionRemove();

    void settingsRead();
    void settingsSave();

    void setWingParameters();

    void showResults();

    void updateAll();
    void updateGraphicsViewPlanform();
    void updateGroupBoxCompParams();
    void updateTableWidgetSectionsData();

private slots:

    void on_actionFileNew_triggered();
    void on_actionFileOpen_triggered();
    void on_actionFileSave_triggered();
    void on_actionFileSaveAs_triggered();

    void on_actionExit_triggered();

    void on_actionViewResults_triggered();

    void on_actionAbout_triggered();

    void on_tableWidgetSectionsData_currentCellChanged( int row, int, int, int );
    void on_tableWidgetSectionsData_cellDoubleClicked( int row, int );

    void on_pushButtonSectionInsert_clicked();
    void on_pushButtonSectionEdit_clicked();
    void on_pushButtonSectionRemove_clicked();

    void on_spinBoxAoASteps_valueChanged( int arg1 );
    void on_spinBoxAoAStart_valueChanged(double arg1);
    void on_spinBoxAoAFinish_valueChanged(double arg1);
    void on_spinBoxAirDensity_valueChanged(double arg1);
    void on_spinBoxAirVelocity_valueChanged(double arg1);
    void on_spinBoxSpanwiseSteps_valueChanged(int arg1);
    void on_spinBoxFourierAccuracy_valueChanged(int arg1);

    void on_pushButtonCompute_clicked();
};

////////////////////////////////////////////////////////////////////////////////

#endif // MAINWINDOW_H
