#-------------------------------------------------
#
# Project created by QtCreator 2015-10-18T12:37:12
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = epollServer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    TcpSocket.cpp \
    Lock.cpp \
    LockAuto.cpp \
    SocketPacket.cpp \
    Timeval.cpp \
    SocketBuffer.cpp \
    SqlManager.cpp \
    SocketClient.cpp \
    TaskPool.cpp \
    Thread.cpp \
    ThreadPool.cpp \
    ThreadPoolManager.cpp \
    Config.cpp

HEADERS += \
    TcpSocket.h \
    global.h \
    Lock.h \
    LockAuto.h \
    SocketPacket.h \
    Timeval.h \
    SocketBuffer.h \
    SqlManager.h \
    SocketClient.h \
    TaskPool.h \
    Thread.h \
    ThreadPool.h \
    ThreadPoolManager.h \
    Config.h

LIBS += \
    -lpthread \
    -lmysqlclient
