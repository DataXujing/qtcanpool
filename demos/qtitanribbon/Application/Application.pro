include(../../../qtproject.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets printsupport
}

TEMPLATE = app
TARGET = "Ribbon_"$$member(TARGET, 0)
DESTDIR = $$IDE_APP_PATH

QTITANDIR = $$QTCANPOOL_DIR/src/libs/qtitan

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

include($$QTCANPOOL_DIR/src/rpath.pri)

INCLUDEPATH += $$QTITANDIR/include

# lib
LIBS *= -l$$qtLibraryNameVersion(qtitan, 1)

include($$QTITANDIR/demos/ribbon/Application/src.pri)
