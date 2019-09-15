/*******************************************************************************
*                                                                              *
* File:         MainWindow.h                                                   *
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
#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

////////////////////////////////////////////////////////////////////////////////
// INCLUDES                                                                   //
////////////////////////////////////////////////////////////////////////////////

#include <QFileDialog>
#include <QMainWindow>
#include <QSettings>
#include <QTableWidgetItem>

#include <bsc/BSc_Aero.h>
#include <bsc/BSc_Wing.h>

////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS                                                       //
////////////////////////////////////////////////////////////////////////////////

namespace Ui
{
    class MainWindow;
}

////////////////////////////////////////////////////////////////////////////////
// CLASS DOCUMENTATION                                                        //
////////////////////////////////////////////////////////////////////////////////

/** @author Marek Cel (mcel@mcel.pl)
    @todo prompt to save unsaved file or discard changes on new, open, exit activated */
class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum
    {
        BCS_ACTION_NEW = 0,
        BCS_ACTION_OPEN,
        BCS_ACTION_CLOSE
    };

////////////////////////////////////////////////////////////////////////////////
// PRIVATE MEMBERS                                                            //
////////////////////////////////////////////////////////////////////////////////
private:

    Ui::MainWindow *ui;

    QString fileName;       ///< current file name

    BSc_Wing *wing;         ///< main wing object

    bool parameters_saved;  ///< specifies if computation parameters have been set
    bool file_changed;      ///< specifies if current wing geo data, computation parameters and results has been saved to file
    bool results_uptodate;  ///< specifies if computed results are up to date due to wing geo data and computation parameters
    bool working_on_file;   ///< specifies if working copy of wing data and results was saved to or opened from a file

    int  action_on_discard; ///< specifies action after modified file saving discard

////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS                                                           //
////////////////////////////////////////////////////////////////////////////////
public:

    explicit MainWindow(QWidget *parent = 0);
    
    ~MainWindow();

    void show( void );

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS                                                        //
////////////////////////////////////////////////////////////////////////////////
protected:

    void closeEvent( QCloseEvent * event );

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS                                                          //
////////////////////////////////////////////////////////////////////////////////
private:

/** void MainWindow::addToReport( const char* field_name, bool success )
    Adds line of text to textReports. */
    void addToReport( const char *field_name, bool success );

    void fileOpen();
    void fileNew();
    void fileSave();
    void fileSaveAs();

    int messageDropChanges();
    int messageDeleteData();
    int messageInputData();

    void settingsRead  ( void );
    void settingsWrite ( void );

    void updateAll();
    void updateButtonNext1();
    void updateButtonNext2();
    void updateGroupAccuracy();
    void updateGroupAngleOfAttack();
    void updateGroupComputations();
    void updateGroupSectionEditor();
    void updateGroupSpanInterpolation();
    void updatePlotWingGeo();
    void updateResults();
    void updateResultsDistributions();
    void updateTableWingData();
    void updateTabs();
    void updateWingData();

////////////////////////////////////////////////////////////////////////////////
// PRIVATE SLOTS                                                              //
////////////////////////////////////////////////////////////////////////////////
private slots:

    void on_actionAbout_BScAeroWing_activated();
    void on_actionAbout_Qt_activated();
    void on_actionExit_activated();
    void on_actionNew_activated();
    void on_actionOpen_activated();
    void on_actionSave_activated();
    void on_actionSave_As_activated();
    void on_buttonAddSection_clicked();
    void on_buttonCompute_clicked();
    void on_buttonDeleteSection_clicked();
    void on_buttonNext1_clicked();
    void on_buttonNext2_clicked();
    void on_buttonSaveParams_clicked();
    void on_buttonSaveSection_clicked();
    void on_sliderAoA_valueChanged(int value);
    void on_spinAngleFinish_valueChanged(double );
    void on_spinAngleIter_valueChanged(int );
    void on_spinAngleStart_valueChanged(double );
    void on_spinFluidDensity_valueChanged(double );
    void on_spinFluidVelocity_valueChanged(double );
    void on_spinFourierAccuracy_valueChanged(int );
    void on_spinInterSections_valueChanged(int );
    void on_tableWingData_currentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous);
    void on_tableWingData_itemSelectionChanged();

};

////////////////////////////////////////////////////////////////////////////////
// END OF FILE                                                                //
////////////////////////////////////////////////////////////////////////////////

#endif // _MAINWINDOW_H_
