/*******************************************************************************
*                                                                              *
* File:         main.cpp                                                       *
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

#include <iostream>

#include <QApplication>
#include <QSettings>

#include <bsc/BSc_Aero.h>
#include <gui/MainWindow.h>

#ifdef _LINUX_
    #include <QCleanlooksStyle>
#endif

////////////////////////////////////////////////////////////////////////////////

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// MAIN FUNCTIONS                                                             //
////////////////////////////////////////////////////////////////////////////////

int main( int argc, char *argv[] )
{
#   ifdef _LINUX_
    setenv( "LC_NUMERIC", "en_US", 1 );
    //QApplication::setStyle( new QCleanlooksStyle );
#   endif

    QApplication app( argc, argv );
    MainWindow   win;

    // ---
    app.setApplicationName    ( BSC_AERO_APP_NAME   );
    app.setApplicationVersion ( BSC_AERO_APP_VER    );
    app.setOrganizationDomain ( BSC_AERO_ORG_DOMAIN );
    app.setOrganizationName   ( BSC_AERO_ORG_NAME   );
    
    // ---
    QSettings settings( BSC_AERO_ORG_NAME, BSC_AERO_APP_NAME );

    win.show();

    return app.exec();
}

////////////////////////////////////////////////////////////////////////////////
// END OF FILE                                                                //
////////////////////////////////////////////////////////////////////////////////
