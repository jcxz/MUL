#-------------------------------------------------
#
# Project created by QtCreator 2014-04-01T12:07:33
#
#-------------------------------------------------

QT += core gui opengl widgets concurrent

CONFIG += console
CONFIG -= app_bundle

TARGET = MUL
TEMPLATE = app

QMAKE_CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic -g -Wno-c++11-narrowing

CL_KERNELS_DIR = src/clkernels

DEFINES += QT_BUILD_CL_LIB
DEFINES += QT_BUILD_CLGL_LIB
DEFINES += "CLSOURCEPATH=\"$${CL_KERNELS_DIR}\""

#opencl_1_1 {
    DEFINES += QT_OPENCL_1_1
#}

INCLUDEPATH += src
INCLUDEPATH += src/opencl
INCLUDEPATH += src/openclgl
INCLUDEPATH += src/filters


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
    LIBS += -L"C:/Program Files (x86)/AMD APP/lib/x86"
    LIBS += -lOpenCL

    DEFINES += UTEST
}

#Specify build folder
#release: DESTDIR = build/release
#debug:   DESTDIR = build/debug

#OBJECTS_DIR = $$DESTDIR/obj
#MOC_DIR = $$DESTDIR/moc
#RCC_DIR = $$DESTDIR/qrc
#UI_DIR = $$DESTDIR/ui

release: BUILD_DIR = build/release
debug:   BUILD_DIR = build/debug

OBJECTS_DIR = $$BUILD_DIR/obj
MOC_DIR = $$BUILD_DIR/moc
RCC_DIR = $$BUILD_DIR/qrc
UI_DIR = $$BUILD_DIR/ui
DESTDIR = ./

HEADERS += \
    src/opencl/qclbuffer.h \
    src/opencl/qclcommandqueue.h \
    src/opencl/qclcontext.h \
    src/opencl/qcldevice.h \
    src/opencl/qclevent.h \
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
    src/opencl/qclext_p.h \
    \
    src/openclgl/qclcontextgl.h \
    src/openclgl/qcl_gl_p.h \
    \
    src/debug.h \
    src/filters/FilterPipeline.h \
    src/filters/Filter.h \
    src/filters/PointFilter.h \
    src/filters/ColorInvertFilter.h \
    src/filters/SepiaFilter.h \
    src/filters/GrayScaleFilter.h \
    src/filters/TransformFilter.h \
    src/filters/ConvolutionFilter.h \
    \
    src/mainwindow.h \
    src/mattoqimage.h \
    src/controller.h \
    src/player.h \
    src/converter.h \
    src/glrenderer.h

SOURCES += \
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
    \
    src/openclgl/qclcontextgl.cpp \
    \
    src/main.cpp \
    src/filters/FilterPipeline.cpp \
    src/filters/PointFilter.cpp \
    src/filters/GrayScaleFilter.cpp \
    src/filters/TransformFilter.cpp \
    src/filters/ConvolutionFilter.cpp \
    \
    src/mainwindow.cpp \
    src/mattoqimage.cpp \
    src/controller.cpp \
    src/player.cpp \
    src/converter.cpp \
    src/glrenderer.cpp

FORMS += src/mainwindow.ui

OTHER_FILES += \
    $${CL_KERNELS_DIR}/colorinvert.cl \
    $${CL_KERNELS_DIR}/convolution.cl \
    $${CL_KERNELS_DIR}/grayscale.cl \
    $${CL_KERNELS_DIR}/sepia.cl \
    $${CL_KERNELS_DIR}/transform.cl
