TEMPLATE = app
TARGET = test.pro

QT = core gui charts serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    main.cpp \
    recv.cpp \
    thre.cpp \
    window.cpp

HEADERS += \
    recv.h \
    thre.h \
    window.h

