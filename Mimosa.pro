QT = core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

HEADERS += \
    mainwindow.h \
    downloaddialog.h \
    downloadprocess.h \
    downloadtable.h \
    mimosa.h \
    downloaditemdelegate.h

SOURCES += \
    mainwindow.cpp \
    main.cpp \
    downloaddialog.cpp \
    downloadprocess.cpp \
    mimosa.cpp \
    downloadtable.cpp \
    downloaditemdelegate.cpp

RESOURCES += \
    icons.qrc
