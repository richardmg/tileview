TEMPLATE = lib

QT += quick quick3d
QT += quick3d-private

CONFIG += qmltypes
QML_IMPORT_NAME = QtQuick3D.TileView
QML_IMPORT_MAJOR_VERSION = 1

TARGET = tileviewplugin
TARGETPATH = QtQuick3D/TileView

DYNAMIC_QMLDIR = \
"module QtQuick3D.TileView" \
"plugin tileviewplugin" \
"classname TileViewPlugin" \
"TileView 1.0 TileView.qml"

SOURCES += \
    tileview.cpp \
    tileviewplugin.cpp

HEADERS += \
    tileview.h

CONFIG += qt plugin
CONFIG += no_cxx_module install_qml_files qtquickcompiler
CONFIG += qmltypes install_qmltypes

load(qml_plugin)
