TEMPLATE = app
TARGET = test.pro

QT = core gui charts serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    command.cpp \
    main.cpp \
    recv.cpp \
    thre.cpp \
    window.cpp

HEADERS += \
    command.h \
    recv.h \
    thre.h \
    window.h

