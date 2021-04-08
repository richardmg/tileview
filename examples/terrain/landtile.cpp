#include "landtile.h"

#define COORD(v) *p++ = v.x(); *p++ = getHeight(v); *p++ = v.y()

LandTile::LandTile()
{
    setStride(3 * sizeof(float));
    setPrimitiveType(QQuick3DGeometry::PrimitiveType::Triangles);
    addAttribute(QQuick3DGeometry::Attribute::PositionSemantic, 0, QQuick3DGeometry::Attribute::F32Type);

    recreate();
}

void LandTile::componentComplete()
{
    QQuick3DGeometry::componentComplete();
    recreate();
}

void LandTile::recreate()
{
//    if (!isComponentComplete())
//        return;

    clear();

    const int vertexCountPerSquare = 6; // two triangles
    const int vertexCount = m_resolution.x() * m_resolution.z() * vertexCountPerSquare;
//    m_vertexData.resize(vertexCount * stride());
    m_vertexData = QByteArray(vertexCount * stride(), Qt::Initialization::Uninitialized);
    qDebug() << "size:" << m_vertexData.size() << m_resolution;

    updateData();
}

QVector3D LandTile::tileSize() const
{
    return m_tileSize;
}

void LandTile::setTileSize(QVector3D tileSize)
{
    if (m_tileSize == tileSize)
        return;

    m_tileSize = tileSize;
    recreate();
    emit tileSizeChanged();
}

QVector3D LandTile::resolution() const
{
    return m_resolution;
}

void LandTile::setResolution(QVector3D resolution)
{
    if (m_resolution == resolution)
        return;

    m_resolution = resolution;
    recreate();
    emit resolutionChanged();
}

QVector3D LandTile::position() const
{
    return m_position;
}

void LandTile::setPosition(QVector3D position)
{
    if (m_position == position)
        return;

    m_position = position;
    updateData();
    emit positionChanged();
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
    const float distX = m_tileSize.x() / m_resolution.x();
    const float distZ = m_tileSize.z() / m_resolution.z();
    qDebug() << "DIST:" << distX << distZ;

    // Front face = counter-clockwise
    for (int x = 0; x < m_resolution.x(); ++x) {
        for (int z = 0; z < m_resolution.z(); ++z) {
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
