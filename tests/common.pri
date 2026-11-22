# Shared settings for every test executable.
QT += testlib sql
QT -= gui
CONFIG += console testcase
CONFIG -= app_bundle

APP_DIR = $$PWD/..
INCLUDEPATH += $$APP_DIR $$PWD
DEFINES += TEST_SCHEMA_DIR=\\\"$$APP_DIR/schema\\\"

# Real application sources (main.cpp is deliberately excluded).
APP_SOURCES = \
    $$APP_DIR/dbmanager.cpp \
    $$APP_DIR/logger.cpp \
    $$APP_DIR/eventlogsmodel.cpp \
    $$APP_DIR/todonotesmodel.cpp \
    $$APP_DIR/todolistmodel.cpp
APP_HEADERS = \
    $$APP_DIR/dbmanager.h \
    $$APP_DIR/logger.h \
    $$APP_DIR/eventlogsmodel.h \
    $$APP_DIR/todonotesmodel.h \
    $$APP_DIR/todolistmodel.h

HEADERS += $$PWD/testenv.h $$APP_HEADERS
SOURCES += $$APP_SOURCES
