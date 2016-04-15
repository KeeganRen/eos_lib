#-------------------------------------------------
#
# Project created by QtCreator 2015-03-22T19:52:02
#
#-------------------------------------------------

#QT       -= core

#QT       -= gui

#TARGET = fit-model
#CONFIG   += console
#CONFIG   -= app_bundle

#TEMPLATE = app

#QMAKE_CXXFLAGS += \
#    -std=c++0x

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
TEMPLATE = lib
CONFIG += staticlib

unix {
    target.path = /usr/lib
    INSTALLS += target
}

QMAKE_CXXFLAGS += \
    -std=c++0x

DESTDIR = ../../lib

INCLUDEPATH += \
    ../include \
    ../3rdparty/cereal-1.1.1/include \
    ../3rdparty/glm-0.9.7.0

INCLUDEPATH += \
    /usr/local/include/opencv2 \
    /usr/local/include/opencv \
    /usr/local/include \
    /usr/include/boost \
    /usr/include/eigen3

# lib-boost
LIBS += \
    -lboost_filesystem -lboost_system -lboost_thread -lboost_program_options

# lib-opencv 2.4.11
LIBS += \
    -L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgproc

#SOURCES += \
#    ../examples/fit-model.cpp

SOURCES += \
    ../src/eos/core/LandmarkMapper.cpp \
    ../src/eos/fitting/affine_camera_estimation.cpp \
    ../src/eos/fitting/linear_shape_fitting.cpp \
    ../src/eos/fitting/nonlinear_camera_estimation.cpp \
    ../src/eos/morphablemodel/io/cvssp.cpp \
    ../src/eos/morphablemodel/MorphableModel.cpp \
    ../src/eos/morphablemodel/PcaModel.cpp \
    ../src/eos/render/detail/render_affine_detail.cpp \
    ../src/eos/render/detail/render_detail.cpp \
    ../src/eos/render/detail/texture_extraction_detail.cpp \
    ../src/eos/render/Mesh.cpp \
    ../src/eos/render/render.cpp \
    ../src/eos/render/render_affine.cpp \
    ../src/eos/render/texture_extraction.cpp \
    ../src/eos/render/utils.cpp \
    ../src/eos/fitting/detail/nonlinear_camera_estimation_detail.cpp \
    ../src/eos_lib/eos_run.cpp

HEADERS += \
    ../include/eos/core/Landmark.h \
    ../include/eos/core/LandmarkMapper.h \
    ../include/eos/fitting/affine_camera_estimation.h \
    ../include/eos/fitting/linear_shape_fitting.h \
    ../include/eos/fitting/nonlinear_camera_estimation.h \
    ../include/eos/fitting/detail/nonlinear_camera_estimation_detail.h \
    ../include/eos/morphablemodel/MorphableModel.h \
    ../include/eos/morphablemodel/PcaModel.h \
    ../include/eos/morphablemodel/io/cvssp.h \
    ../include/eos/morphablemodel/io/mat_cerealisation.h \
    ../include/eos/render/Mesh.h \
    ../include/eos/render/render.h \
    ../include/eos/render/render_affine.h \
    ../include/eos/render/texture_extraction.h \
    ../include/eos/render/utils.h \
    ../include/eos/render/detail/render_affine_detail.h \
    ../include/eos/render/detail/render_detail.h \
    ../include/eos/render/detail/texture_extraction_detail.h \
    ../include/eos_lib/eos_run.h
