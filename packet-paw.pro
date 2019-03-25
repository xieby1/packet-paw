#-------------------------------------------------
#
# Project created by QtCreator 2019-03-20T13:08:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = packet-paw
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

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    pawthread.cpp

HEADERS += \
        mainwindow.h \
    packetheaders.h \
    pawthread.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# The following information comes from https://doc.qt.io/qt-5/qmake-project-files.html
# It is important to add pcap library, or error shows functions in pcap.h is "undefined reference"d !!!
######
# If you are using other libraries in your project in addition to those supplied with Qt,
# you need to specify them in your project file.
######
# The paths that qmake searches for libraries and the specific libraries to link against
# can be added to the list of values in the LIBS variable.
# You can specify the paths to the libraries or use the Unix-style notation for specifying libraries and paths.
LIBS += -lpcap
