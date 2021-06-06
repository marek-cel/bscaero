QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

################################################################################

DESTDIR = ../bin
TARGET = bscaerowing

################################################################################

CONFIG += c++11

################################################################################

win32: RC_FILE = bscaero.rc

################################################################################

win32: CONFIG(release, debug|release): QMAKE_CXXFLAGS += -O2
unix:  CONFIG(release, debug|release): QMAKE_CXXFLAGS += -O3

#win32: QMAKE_CXXFLAGS += /Zc:wchar_t

win32: QMAKE_LFLAGS += /INCREMENTAL:NO

################################################################################

DEFINES += \

greaterThan(QT_MAJOR_VERSION, 4):win32: DEFINES += USE_QT5

win32: DEFINES += \
    WIN32 \
    _WINDOWS \
    _CRT_SECURE_NO_DEPRECATE \
    _SCL_SECURE_NO_WARNINGS \
    _USE_MATH_DEFINES

win32: CONFIG(release, debug|release): DEFINES += NDEBUG
win32: CONFIG(debug, debug|release):   DEFINES += _DEBUG

#CONFIG(release, debug|release): DEFINES += QT_NO_DEBUG_OUTPUT
#DEFINES += QT_NO_DEBUG_OUTPUT

unix: DEFINES += _LINUX_

################################################################################

INCLUDEPATH += ./

unix: INCLUDEPATH += /usr/include/qwt/

win32: INCLUDEPATH += \
    $(QWT_ROOT)/include

################################################################################

win32: LIBS += \
    -L$(QWT_ROOT)/lib

win32: CONFIG(release, debug|release): LIBS += \
    -lqwt

win32: CONFIG(debug, debug|release): LIBS += \
    -lqwtd

unix: LIBS += \
    -L/lib \
    -L/usr/lib \
    -lqwt-qt5

################################################################################

HEADERS += \
    defs.h \
    $$PWD/GaussJordan.h \
    $$PWD/Wing.h

SOURCES += \
    main.cpp \
    $$PWD/GaussJordan.cpp \
    $$PWD/Wing.cpp

RESOURCES += \
    bscaero.qrc

################################################################################

include(gui/gui.pri)
