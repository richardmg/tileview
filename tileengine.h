#ifndef TILEENGINE_H
#define TILEENGINE_H

#include <QtCore/QtCore>
#include <QtQml/QtQml>
#include <QtQuick3D/QtQuick3D>

const QPoint kNoNeighbour(-1, -1);

struct TileNeighbours
{
    QPoint left = kNoNeighbour;
    QPoint right = kNoNeighbour;
    QPoint top = kNoNeighbour;
    QPoint bottom = kNoNeighbour;
};

struct TileDescription
{
    QVector3D worldPos;
    QPoint tileCoord;
    QPoint matrixCoord;
    TileNeighbours neighbours;
};

class TileEngine : public QObject
{
public:
    TileEngine();

public:
    TileEngine(int tileCount, qreal tileWorldSize, QObject *parent = nullptr);

    void setTargetPosition(QVector3D worldPos);
    QVector3D worldPosForTileCoord(QPoint tileCoord);
    QPoint tileCoordAtWorldPos(QVector3D worldPos);

    virtual void updateTiles(const QVector<TileDescription> &tiles);
    virtual void updateNeighbours(const QVector<TileNeighbours> &neighbours);

private:
    int matrixPos(int startEdge, int edgeShifted);
    QPoint matrixCoordForWorldPos(QVector3D worldPos);
    QPoint tileCoordForMatrixCoord(QPoint matrixCoord);

//    void setNeighbours(QPoint pos, TileNeighbours &result);
    void updateAllTiles();
    QPoint tileCoordAtWorldPosShifted(QVector3D worldPos);
    void updateTilesHelp(int shifted, int topRightX, int topRightY, bool updateAxisY);

private:
    int m_tileCount = 4;
    int m_tileCountHalf;
    qreal m_tileWorldSize = 100;

    QVector3D m_targetWorldPos;

    QPoint m_shiftedTileCoord;
    QPoint m_prevShiftedTileCoord;
    TileDescription m_topRight;

    QVector<TileDescription> m_tileMoveDesc;
};

#endif // TILEENGINE_H
