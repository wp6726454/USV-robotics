#-------------------------------------------------
#
# Project created by QtCreator 2018-05-18T21:04:20
#
#-------------------------------------------------

QT       += core gui gamepad

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = myplottest
TEMPLATE = app
CONFIG += c++14
INCLUDEPATH+=\
    /home/skloe/Coding/CPP1X/USV/DPfloatover/third_party/eigen   \
    /opt/mosek/7/tools/platform/linux32x86/h \
    /home/skloe/Coding/CPP1X/USV/DPfloatover/include \
    /home/skloe/Coding/CPP1X/USV/timer \

QMAKE_LFLAGS += -m32
QMAKE_CXXFLAGS += -m32 -std=c++14 \
                  -march=native -O3 -mavx

LIBS += -L/opt/mosek/7/tools/platform/linux32x86/bin \
        -Wl,-rpath-link,/opt/mosek/7/tools/platform/linux32x86/bin \
        -Wl,-rpath,'/opt/mosek/7/tools/platform/linux32x86/bin' \
        -lsqlite3 \
        -lmosek


# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
    dataviewer.cpp \
    display2ddialog.cpp \
    mainwindow.cpp \
    qcustomplot.cpp \
    thrusterdiag.cpp \
    ../network/src/crccheck.c \
    ../network/src/datapack.c \
    ../network/libcrc/src/crc8.c \
    ../network/libcrc/src/crc16.c \
    ../network/libcrc/src/crc32.c \
    ../network/libcrc/src/crc64.c \
    ../network/libcrc/src/crcccitt.c \
    ../network/libcrc/src/crcdnp.c \
    ../network/libcrc/src/crckrmit.c \
    ../network/libcrc/src/crcsick.c \
    ../network/libcrc/src/nmea-chk.c \
    gamepadmonitor.cpp


HEADERS += \
    dataviewer.h \
    globalvar.h \
    mainwindow.h \
    qcustomplot.h \
    thrusterdiag.h \
    display2ddialog.h \
    ../include/constants.h \
    ../network/include/crccheck.h \
    ../network/include/datapack.h \
    ../network/libcrc/include/checksum.h \
    gamepadmonitor.h \
    ../motioncapture/include/motioncapture.h \
    ../include/threaded_Loop.h \
    ../network/include/tcpserver_t.h \
    ../sql/include/databasecpp.h \
    ../include/realtimedata.h

FORMS += \
        mainwindow.ui \
        display2ddialog.ui \
        thrusterdiag.ui
