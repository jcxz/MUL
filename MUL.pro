#-------------------------------------------------
#
# Project created by QtCreator 2014-04-01T12:07:33
#
#-------------------------------------------------

QT       += core gui opengl widgets concurrent

CONFIG += console
CONFIG -= app_bundle

TARGET = MUL
TEMPLATE = app

QMAKE_CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic -g -Wno-c++11-narrowing

DEFINES += QT_BUILD_CL_LIB
DEFINES += QT_BUILD_CLGL_LIB

#opencl_1_1 {
    DEFINES += QT_OPENCL_1_1
#}

mac {
    LIBS += -framework OpenCL

    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
    INCLUDEPATH += /opt/local/include
    LIBS += -L/opt/local/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_calib3d -lopencv_features2d
}

win32 {
  INCLUDEPATH += D:/AC601/ostatne/Programovanie/libraries/opencv/build/include
  LIBS += -L"${MY_LIB_PATH}/opencv/build/x86/mingw47/lib"
  LIBS += -lopencv_core248.dll
  LIBS += -lopencv_highgui248.dll
  LIBS += -lopencv_imgproc248.dll

  INCLUDEPATH += "C:/Program Files (x86)/AMD APP/include"
  INCLUDEPATH += ./opencl
  INCLUDEPATH += ./openclgl
  LIBS += -L"C:/Program Files (x86)/AMD APP/lib/x86"
  LIBS += -lOpenCL

  OBJECTS_DIR = obj
  MOC_DIR = obj
  RCC_DIR = obj
  UI_DIR = obj
  DESTDIR = ./
}

#Specify build folder
release: DESTDIR = build/release
debug:   DESTDIR = build/debug

OBJECTS_DIR = $$DESTDIR/obj
MOC_DIR = $$DESTDIR/moc
RCC_DIR = $$DESTDIR/qrc
UI_DIR = $$DESTDIR/ui

SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/mattoqimage.cpp \
    src/controller.cpp \
    src/player.cpp \
    src/FilterGray.cpp \
    src/converter.cpp \
    src/glrenderer.cpp \
    src/FilterPipeline.cpp \
    src/GrayScaleFilter.cpp \
    src/PointFilter.cpp \
    src/TransformFilter.cpp \
    src/opencl/qclbuffer.cpp \
    src/opencl/qclcommandqueue.cpp \
    src/opencl/qclcontext.cpp \
    src/opencl/qcldevice.cpp \
    src/opencl/qclevent.cpp \
    src/opencl/qclimage.cpp \
    src/opencl/qclimageformat.cpp \
    src/opencl/qclkernel.cpp \
    src/opencl/qclmemoryobject.cpp \
    src/opencl/qclplatform.cpp \
    src/opencl/qclprogram.cpp \
    src/opencl/qclsampler.cpp \
    src/opencl/qcluserevent.cpp \
    src/opencl/qclvector.cpp \
    src/opencl/qclworksize.cpp \
    src/openclgl/qclcontextgl.cpp

HEADERS += src/mainwindow.h \
    src/mattoqimage.h \
    src/controller.h \
    src/player.h \
    src/Filter.h \
    src/FilterGray.h \
    src/converter.h \
    src/glrenderer.h \
    src/ColorInvertFilter.h \
    src/FilterPipeline.h \
    src/GrayScaleFilter.h \
    src/PointFilter.h \
    src/SepiaFilter.h \
    src/TransformFilter.h \
    src/opencl/qclbuffer.h \
    src/opencl/qclcommandqueue.h \
    src/opencl/qclcontext.h \
    src/opencl/qcldevice.h \
    src/opencl/qclevent.h \
    src/opencl/qclext_p.h \
    src/opencl/qclglobal.h \
    src/opencl/qclimage.h \
    src/opencl/qclimageformat.h \
    src/opencl/qclkernel.h \
    src/opencl/qclmemoryobject.h \
    src/opencl/qclplatform.h \
    src/opencl/qclprogram.h \
    src/opencl/qclsampler.h \
    src/opencl/qcluserevent.h \
    src/opencl/qclvector.h \
    src/opencl/qclworksize.h \
    src/openclgl/qcl_gl_p.h \
    src/openclgl/qclcontextgl.h

FORMS += src/mainwindow.ui

OTHER_FILES += \
    src/clkernels/colorinvert.cl \
    src/clkernels/convolution.cl \
    src/clkernels/grayscale.cl \
    src/clkernels/sepia.cl \
    src/clkernels/transform.cl
