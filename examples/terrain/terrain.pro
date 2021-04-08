TEMPLATE = app
QT += quick quick3d gui

CONFIG += qmltypes
QML_IMPORT_NAME = LandTile
QML_IMPORT_MAJOR_VERSION = 1

SOURCES += \
    main.cpp \
    landtile.cpp \
    perlinnoise.cpp \

HEADERS += \
    landtile.h \
    perlinnoise.h

RESOURCES += \
    resources.qrc \
    materials.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/terrain
INSTALLS += target

