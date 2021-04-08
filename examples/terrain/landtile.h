#ifndef LANDTILE_H
#define LANDTILE_H

#include <QtGui/QtGui>
#include <QQuick3DGeometry>

#include "perlinnoise.h"

class LandTile : public QQuick3DGeometry
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QVector3D tileSize READ tileSize WRITE setTileSize NOTIFY tileSizeChanged)
    Q_PROPERTY(QVector3D resolution READ resolution WRITE setResolution NOTIFY resolutionChanged)
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QVector3D sampleScale READ sampleScale WRITE setSampleScale NOTIFY sampleScaleChanged)

public:
    LandTile();

    QVector3D tileSize() const;
    void setTileSize(QVector3D tileSize);

    QVector3D resolution() const;
    void setResolution(QVector3D resolution);

    QVector3D position() const;
    void setPosition(QVector3D position);

    QVector3D sampleScale() const;
    void setSampleScale(QVector3D sampleScale);

signals:
    void tileSizeChanged();
    void resolutionChanged();
    void positionChanged();
    void sampleScaleChanged();

protected:
    void componentComplete() override;

private:
    float getHeight(const QVector2D &pos);
    void recreate();
    void updateData();

private:
    QVector3D m_position;
    QVector3D m_tileSize = QVector3D(100, 100, 100);
    QVector3D m_resolution = QVector3D(10, 10, 10);
    QVector3D m_sampleScale = QVector3D(0.1, 0.1, 0.1);

    QByteArray m_vertexData;
    PerlinNoise m_perlin;
};

#endif
