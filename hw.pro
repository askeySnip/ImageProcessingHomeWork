#-------------------------------------------------
#
# Project created by QtCreator 2019-04-27T14:43:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = hw
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Add opencv

INCLUDEPATH+=/usr/local/include\
/usr/local/include/opencv4\
/usr/local/include/opencv2
LIBS+=/usr/local/lib/libopencv_highgui.so\
/usr/local/lib/libopencv_core.so\
/usr/local/lib/libopencv_imgproc.so\
/usr/local/lib/libopencv_imgcodecs.so


CONFIG += c++11

SOURCES += \
        algor.cpp \
        graphicsview.cpp \
        histogram.cpp \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        algor.h \
        graphicsview.h \
        histogram.h \
        mainwindow.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
