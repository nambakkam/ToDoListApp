QT += quick sql

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        dbmanager.cpp \
        eventlogsmodel.cpp \
        logger.cpp \
        main.cpp \
        todolistmodel.cpp \
        todonotesmodel.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    dbmanager.h \
    eventlogsmodel.h \
    logger.h \
    todolistmodel.h \
    todonotesmodel.h

# --- Place this at the very end of your .pro ---
defineReplace(toWindowsPath) {
    result = $$1
    result ~= s,/,\\,g
    return($$result)
}

SCHEMA_DIR = $$toWindowsPath($$PWD)\schema
DEST_SCHEMA_DIR = $$toWindowsPath($$OUT_PWD)\schema


message($$SCHEMA_DIR)
message($$DEST_SCHEMA_DIR)

# Make sure the schema folder exists in build dir
#QMAKE_POST_LINK += $$QMAKE_MKDIR_CMD $$DEST_SCHEMA_DIR $$escape_expand(\\n\\t)
#message($$QMAKE_MKDIR_CMD $$DEST_SCHEMA_DIR $$escape_expand(\\n\\t))
## Copy schema files after build
QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$SCHEMA_DIR $$DEST_SCHEMA_DIR
#message($$QMAKE_COPY_DIR $$SCHEMA_DIR $$DEST_SCHEMA_DIR)




