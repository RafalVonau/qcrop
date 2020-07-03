VERSION="1.0.0"
message("QCrop v$${VERSION}")

INCLUDEPATH += ./ \
    ../.build/ui

QT  += sql script network core gui

unix: {
    OBJECTS_DIR = ./.build_lin/obj
    MOC_DIR =     ./.build_lin/moc
    UI_DIR =      ./.build_lin/ui
    RCC_DIR =     ./.build_lin/rcc
}

win32: {
    OBJECTS_DIR = ./.build_win/obj
    MOC_DIR =     ./.build_win/moc
    UI_DIR =      ./.build_win/ui
    RCC_DIR =     ./.build_win/rcc
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += *.cpp
HEADERS += *.h
FORMS   += *.ui
win32:LIBS += -lws2_32
win32:RC_FILE = ../winicon.rc
DESTDIR = ../
TEMPLATE = app
RESOURCES += *.qrc

DEFINES += APP_VERSION=$$VERSION
include($$PWD/qtsingleapplication/src/qtsingleapplication.pri)


wince*: {
    CONFIG(debug, debug|release):sqlPlugins.sources = $$QT_BUILD_TREE/plugins/sqldrivers/*d4.dll
    CONFIG(release, debug|release):sqlPlugins.sources = $$QT_BUILD_TREE/plugins/sqldrivers/*[^d]4.dll
    sqlPlugins.path = sqldrivers
    DEPLOYMENT += sqlPlugins
}
