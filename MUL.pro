#-------------------------------------------------
#
# Project created by QtCreator 2014-04-01T12:07:33
#
#-------------------------------------------------

QT       += core gui opengl widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MUL
TEMPLATE = app

QMAKE_CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic -g

mac {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
    INCLUDEPATH += /opt/local/include
    LIBS += -L/opt/local/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_calib3d -lopencv_features2d
}

win32 {
INCLUDEPATH += \
  D:/AC601/ostatne/Programovanie/libraries/opencv/build/include

LIBS += -L"${MY_LIB_PATH}/opencv/build/x86/mingw47/lib" \
        -lopencv_core248.dll \
        -lopencv_highgui248.dll \
        -lopencv_imgproc248.dll
}

SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/mattoqimage.cpp \
    src/controller.cpp \
    src/player.cpp \
    src/FilterGray.cpp

HEADERS += src/mainwindow.h \
    src/mattoqimage.h \
    src/controller.h \
    src/player.h \
    src/Filter.h \
    src/FilterGray.h

FORMS += src/mainwindow.ui
