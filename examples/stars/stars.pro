TEMPLATE = app
QT += quick quick3d

SOURCES += \
    main.cpp

RESOURCES += \
    resources.qrc \
    materials.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/stars
INSTALLS += target

