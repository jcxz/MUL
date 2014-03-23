######################################################################
# @author: Matus Fedorko
######################################################################

# Setup helper variables
SRC_ROOT = ../../src
RES_ROOT = ../../src/res
FRM_ROOT = ../../src/forms
OBJ_ROOT = ../../obj
BIN_ROOT = ./                # . to produce directories for release and debug builds

# Setup Qt
QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += link_pkgconfig

# Setup project
TEMPLATE = app
TARGET = MUL
OBJECTS_DIR = $${OBJ_ROOT}
MOC_DIR = $${OBJ_ROOT}
RCC_DIR = $${OBJ_ROOT}
UI_DIR = $${OBJ_ROOT}
DESTDIR = $${BIN_ROOT}

# Setup additional include directories
INCLUDEPATH += \
  $${SRC_ROOT} \
  D:/AC601/ostatne/Programovanie/libraries/opencv/build/include

# Setup additional libraries
LIBS += -L"${MY_LIB_PATH}/opencv/build/x86/mingw47/lib" \
        -lopencv_core248.dll \
        -lopencv_highgui248.dll

# Setup project's dependancies
PKGCONFIG +=

# Setup compiler
*g++* {
  QMAKE_CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic -g
}

# Header files
HEADERS += \
  $${SRC_ROOT}/Filter.h \
  $${SRC_ROOT}/FilterGray.h

# Source files
SOURCES += \
  $${SRC_ROOT}/FilterGray.cpp \
  $${SRC_ROOT}/main.cpp

# UI forms
FORMS +=

# Resources that shall be compiled into the binary
RESOURCES +=

# Other files (resource icons, stylesheets)
OTHER_FILES +=
