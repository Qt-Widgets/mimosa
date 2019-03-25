QT = core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

HEADERS += \
    mimosa/mainwindow.h \
    mimosa/downloaddialog.h \
    mimosa/downloadprocess.h \
    mimosa/downloadtable.h \
    mimosa/mimosa.h \
    mimosa/downloaditemdelegate.h

SOURCES += \
    mimosa/mainwindow.cpp \
    mimosa/main.cpp \
    mimosa/downloaddialog.cpp \
    mimosa/downloadprocess.cpp \
    mimosa/mimosa.cpp \
    mimosa/downloadtable.cpp \
    mimosa/downloaditemdelegate.cpp

RESOURCES += \
    mimosa.qrc
