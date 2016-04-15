#-------------------------------------------------
#
# Project created by QtCreator 2015-03-22T19:52:02
#
#-------------------------------------------------

QT       -= core

QT       -= gui

TARGET = fit-model
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += \
    -std=c++0x

DESTDIR = ../../bin

INCLUDEPATH += \
    ../include

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
    /home/ren/MyGit/eos_lib/eos_lib/lib/libeos_lib.a

LIBS += \
    -L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgproc

SOURCES += \
    ../examples/fit-model.cpp

HEADERS += \
    ../include/eos_lib/eos_run.h
