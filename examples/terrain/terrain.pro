TEMPLATE = app
QT += quick quick3d

CONFIG += qmltypes
QML_IMPORT_NAME = LandTile
QML_IMPORT_MAJOR_VERSION = 1

SOURCES += \
    main.cpp \
    landtile.cpp \

HEADERS += \
    landtile.h

RESOURCES += \
    resources.qrc \
    materials.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/terrain
INSTALLS += target

