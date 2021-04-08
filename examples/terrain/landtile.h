#ifndef LANDTILE_H
#define LANDTILE_H

#include <QQuick3DGeometry>

#include "perlinnoise.h"

class LandTile : public QQuick3DGeometry
{
    Q_OBJECT
    QML_ELEMENT

public:
    LandTile();

private:
    float getHeight(const QVector2D &pos);
    void updateData();

private:
    float m_tileWidth = 200;
    float m_tileHeight = 200;
    int m_rows = 100;
    int m_columns = 100;

    int m_stride = 3 * sizeof(float);
    int m_vertexCount = 0;

    QByteArray m_vertexData;
    PerlinNoise m_perlin;
};

#endif
