QT += core
QT += testlib
QT -= gui

CONFIG += c++11

TARGET = QExpression
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

DEPENDPATH += . src test
INCLUDEPATH += . src test

SOURCES += \
    src/qexpression.cpp \
    test/testqexpression.cpp

HEADERS += \
    src/qexpression.h \
    test/testqexpression.h

QMAKE_CXXFLAGS += -Werror -Wall -Wextra -Wnon-virtual-dtor
