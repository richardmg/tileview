#include "landtile.h"

#define COORD(v) *p++ = v.x(); *p++ = getHeight(v); *p++ = v.y()
#define UV(v) *p++ = v.x(); *p++ = v.y()

LandTile::LandTile()
{
}

void LandTile::componentComplete()
{
    QQuick3DGeometry::componentComplete();
    recreate();
}

void LandTile::recreate()
{
    if (!isComponentComplete())
        return;

    clear();

    setStride((3 + 2) * sizeof(float)); // Vertices + UV
    setPrimitiveType(QQuick3DGeometry::PrimitiveType::Triangles);
    addAttribute(QQuick3DGeometry::Attribute::PositionSemantic, 0, QQuick3DGeometry::Attribute::F32Type);
    addAttribute(QQuick3DGeometry::Attribute::TexCoordSemantic,
                 3 * sizeof(float),
                 QQuick3DGeometry::Attribute::F32Type);

    const int vertexCountPerSquare = 6; // two triangles
    const int vertexCount = m_resolution.x() * m_resolution.z() * vertexCountPerSquare;
    const int bufferSize = vertexCount * stride();

    if (m_vertexData.size() != bufferSize)
        m_vertexData.resize(vertexCount * stride());

    updateData();
    markAllDirty();
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
    updateData();
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

QVector3D LandTile::sampleScale() const
{
    return m_sampleScale;
}

void LandTile::setSampleScale(QVector3D sampleScale)
{
    if (m_sampleScale == sampleScale)
        return;

    m_sampleScale = sampleScale;
    updateData();
    emit sampleScaleChanged();
}

float LandTile::getHeight(const QVector2D &pos)
{
    const qreal posX = (m_position.x() + pos.x()) * m_sampleScale.x();
    const qreal posY = (m_position.z() + pos.y()) * m_sampleScale.z();
    const qreal height = m_perlin.noise(posX, posY, 0.1);
    return height;//* 10;
}

void LandTile::updateData()
{
    if (!isComponentComplete())
        return;

    float *p = reinterpret_cast<float *>(m_vertexData.data());
    const float distX = m_tileSize.x() / m_resolution.x();
    const float distZ = m_tileSize.z() / m_resolution.z();

    // Front face = counter-clockwise
    for (int x = 0; x < m_resolution.x(); ++x) {
        for (int z = 0; z < m_resolution.z(); ++z) {
            // Draw two triangles that form a square
            QVector2D c0((x + 1) * distX, (z + 1) * distZ);
            QVector2D c1((x + 1) * distX, z * distZ);
            QVector2D c2(x * distX, (z + 1) * distX);
            QVector2D c3(x * distX, z * distZ);

            QVector2D uv0(1 / m_resolution.x(), 1 / m_resolution.z());
            QVector2D uv1(1 / m_resolution.x(), 0);
            QVector2D uv2(0, 1 / m_resolution.z());
            QVector2D uv3(0, 0);

            COORD(c0);
            UV(uv0);
            COORD(c1);
            UV(uv1);
            COORD(c2);
            UV(uv2);

            COORD(c2);
            UV(uv2);
            COORD(c1);
            UV(uv1);
            COORD(c3);
            UV(uv3);
        }
    }

    setVertexData(m_vertexData);
    update();
}
