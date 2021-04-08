#include "landtile.h"

#include <QtGui/QtGui>

#define COORD(v) *p++ = v.x(); *p++ = getHeight(v); *p++ = v.y()

LandTile::LandTile()
{
    const int vertexCountPerSquare = 6;
    m_vertexCount = m_rows * m_columns * vertexCountPerSquare;
    m_vertexData = QByteArray(m_vertexCount * m_stride, Qt::Initialization::Uninitialized);
    setStride(m_stride);
    setPrimitiveType(QQuick3DGeometry::PrimitiveType::Triangles);
    addAttribute(QQuick3DGeometry::Attribute::PositionSemantic, 0, QQuick3DGeometry::Attribute::F32Type);

    updateData();
}

float LandTile::getHeight(const QVector2D &pos)
{
    const qreal sampleDistanceScale = 0.1;
    qreal height = m_perlin.noise(pos.x() * sampleDistanceScale, pos.y() * sampleDistanceScale, 0.1);
    return height * 10;
}

void LandTile::updateData()
{
    float *p = reinterpret_cast<float *>(m_vertexData.data());
    const float distX = m_tileWidth / m_columns;
    const float distZ = m_tileHeight / m_rows;
    qDebug() << distX << distZ;

    // Front face = counter-clockwise
    for (int x = 0; x < m_columns; ++x) {
        for (int z = 0; z < m_rows; ++z) {
            // Draw two triangles that form a square
            QVector2D c0((x + 1) * distX, (z + 1) * distZ);
            QVector2D c1((x + 1) * distX, z * distZ);
            QVector2D c2(x * distX, (z + 1) * distX);
            QVector2D c3(x * distX, z * distZ);

            COORD(c0);
            COORD(c1);
            COORD(c2);

            COORD(c2);
            COORD(c1);
            COORD(c3);
        }
    }

    setVertexData(m_vertexData);
}
