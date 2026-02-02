TEMPLATE = app
TARGET = nitrolinux
QT += core gui widgets
CONFIG += c++11

SOURCES += main_gui.cpp nitro_backend.c
HEADERS += nitro_backend.h
RESOURCES += resources.qrc