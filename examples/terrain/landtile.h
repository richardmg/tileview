#ifndef LANDTILE_H
#define LANDTILE_H

#include <QQuick3DGeometry>

class LandTile : public QQuick3DGeometry
{
    Q_OBJECT
    QML_ELEMENT

public:
    LandTile();

private:
    void updateData();
};

#endif
