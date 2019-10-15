CXX_MODULE = qml
TARGET = qtquick3dhelpersplugin
TARGETPATH = QtQuick3D/Helpers
QT += quick qml
IMPORT_VERSION = 1.$$QT_MINOR_VERSION

QML_FILES = \
    AxisHelper.qml \
    DebugView.qml \
    WasdController.qml

MESH_FILES = \
    meshes/axisGrid.mesh

QML_FILES += $$MESH_FILES

OTHER_FILES += $$QML_FILES

SOURCES += plugin.cpp

DESTFILES += qmldir

load(qml_plugin)
