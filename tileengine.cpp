#include "tileengine.h"

#include <QtMath>

/**
 * The matrix is an imagined matrix with the size m_tileCount * m_tileCount.
 * It represents the "window" of tiles that we're currently on top of.
 * (It's imagined, since we don't need to allocate room for it, we only need
 * a vector of m_tileCount TileDescriptions, that we can reuse).
 * This function returns the matrix coordinate under the given
 * world pos. This coordinate will be in the range [0, m_tileCount - 1] (for
 * each dimention).
 * This should not be confused with the tile coordinate, which has an infinite
 * range, and represents the logical tile under the world position.
 */
QPoint TileEngine::matrixCoordForWorldPos(QVector3D worldPos)
{
    const QPoint tileCoord = tileCoordAtWorldPos(worldPos);
    const QPoint tileOffset = m_topRight.tileCoord - tileCoord;

    if (tileOffset.x() >= 0 && tileOffset.y() >= 0 && tileOffset.x() <= m_tileCount && tileOffset.y() <= m_tileCount) {
        qmlWarning(this) << "worldPos is outside the current area covered by the matrix";
        return QPoint();
    }

    const int matrixX = matrixPos(m_topRight.matrixCoord.x(), -tileOffset.x());
    const int matrixY = matrixPos(m_topRight.matrixCoord.y(), -tileOffset.y());
    return QPoint(matrixX, matrixY);
}

QPoint TileEngine::tileCoordForMatrixCoord(QPoint matrixCoord)
{
    // Normalize matrix coord (as if the matrix were unshifted)
    const int matrixXNorm = matrixPos(matrixCoord.x(), -m_topRight.matrixCoord.x() + (m_tileCount - 1));
    const int matrixYNorm = matrixPos(matrixCoord.y(), -m_topRight.matrixCoord.y() + (m_tileCount - 1));

    const int tileOffsetX = m_tileCount - matrixXNorm - 1;
    const int tileOffsetY = m_tileCount - matrixYNorm - 1;

    const int tileX = m_topRight.tileCoord.x() - tileOffsetX;
    const int tileY = m_topRight.tileCoord.y() - tileOffsetY;

    return QPoint(tileX, tileY);
}

/**
 * The user of TileEngine is supposed to have a matrix of m_tileCount * m_tileCount size.
 * Whenever our target position moves, new rows and columns with tiles will move into the
 * current "window" of tiles that the engine at any given time covers. Since we don't want
 * to move all the tiles in the matrix, e.g one row up or down when a new row appears, to make
 * room for the new row, we simply move a "pointer" instead. This pointer is called
 * m_topRight.matrixCoord. m_topRight.matrixCoord therefore points to the cells in the matrix
 * that is furthest away from the the target pos (north-east side).
 * We can simply move the pointer since we know that when one row appears at one side of the
 * matrix, the row on the oppsite side moves out, and is free to be used to show the new row.
 *
 * This function will return the new position of startEdge (which typically will be the
 * either m_topRight.matrixCoord.x() or m_topRight.matrixCoord.y()), and give you it's new
 * position the matrix if you shift it edgeShifted in one direction. This might mean that
 * it wraps around on the other side of the matrix.
 */
int TileEngine::matrixPos(int startEdge, int edgeShifted)
{
    return (m_tileCount + startEdge + (edgeShifted % m_tileCount)) % m_tileCount;
}

//void TileEngine::setNeighbours(QPoint pos, ref TileNeighbours result)
//{
//    int matrixTopEdge = m_topRight.matrixCoord.y;
//    int matrixBottomEdge = matrixPos(matrixTopEdge, 1);
//    int matrixRightEdge = m_topRight.matrixCoord.x;
//    int matrixLeftEdge = matrixPos(matrixRightEdge, 1);

//    bool onTopEdge = (pos.y == matrixTopEdge);
//    bool onBottomEdge = (pos.y == matrixBottomEdge);
//    bool onLeftEdge = (pos.x == matrixLeftEdge);
//    bool onRightEdge = (pos.x == matrixRightEdge);

//    if (onTopEdge) result.top.set(-1, -1); else result.top.set(pos.x, matrixPos(pos.y, 1));
//    if (onBottomEdge) result.bottom.set(-1, -1); else result.bottom.set(pos.x, matrixPos(pos.y, -1));
//    if (onLeftEdge) result.left.set(-1, -1); else result.left.set(matrixPos(pos.x, -1), pos.y);
//    if (onRightEdge) result.right.set(-1, -1); else result.right.set(matrixPos(pos.x, 1), pos.y);
//}

void TileEngine::updateAllTiles()
{
    for (int matrixY = 0; matrixY < m_tileCount; ++matrixY) {
        for (int matrixX = 0; matrixX < m_tileCount; ++matrixX) {
            TileDescription &desc = m_tileMoveDesc[matrixX];

            desc.matrixCoord = QPoint(matrixX, matrixY);
            desc.tileCoord = tileCoordForMatrixCoord(desc.matrixCoord);
            desc.worldPos = worldPosForTileCoord(desc.tileCoord);
            // TODO: ensure that we m_tileMoveDesc was updated!

//            setNeighbours(m_tileMoveDesc[matrixX].matrixCoord, ref m_tileMoveDesc[matrixX].neighbours);
        }

        updateTiles(m_tileMoveDesc);
//        updateNeighbours(m_tileMoveDesc);
    }
}

QPoint TileEngine::tileCoordAtWorldPosShifted(QVector3D worldPos)
{
    // Note: tileCoordAtWorldPosShifted is an internal concept, and is only used to
    // determine when to update the tile matrix. We use tileCoordAtWorldPosShifted to
    // shift the target position half a tile north-east to roll the matrix
    // when the user passes the center of a tile, rather than at the edge.
    const qreal offset = qreal(m_tileWorldSize / 2.);
    const QVector3D shifted(worldPos.x() + offset, worldPos.y() + offset, worldPos.z() + offset);
    return tileCoordAtWorldPos(shifted);
}

void TileEngine::updateTilesHelp(int shifted, int topRightX, int topRightY, bool updateAxisY)
{
    int moveDirection = shifted > 0 ? 1 : -1;
    int shiftCount = qMin(qAbs(shifted), m_tileCount);
    int shiftCountIncludingNeighbours = shiftCount;// + (neighbourCallback != null ? 1 : 0);

    for (int i = 0; i < shiftCountIncludingNeighbours; ++i) {
        int matrixFrontX = matrixPos(topRightX, i * -moveDirection);
        if (moveDirection < 0)
            matrixFrontX = matrixPos(matrixFrontX, 1);

        for (int j = 0; j < m_tileCount; ++j) {
            QPoint &matrixCoord = m_tileMoveDesc[j].matrixCoord;
            matrixCoord = QPoint(matrixFrontX, matrixPos(topRightY, -j));
            if (updateAxisY) {
                const int tmp = matrixCoord.x();
                matrixCoord.setX(matrixCoord.y());
                matrixCoord.setY(tmp);
            }

            m_tileMoveDesc[j].tileCoord = tileCoordForMatrixCoord(matrixCoord);
            m_tileMoveDesc[j].worldPos = worldPosForTileCoord(m_tileMoveDesc[j].tileCoord);

//            if (neighbourCallback != null)
//                setNeighbours(matrixCoord, ref m_tileMoveDesc[j].neighbours);
        }

        if (i < shiftCount)
            updateTiles(m_tileMoveDesc);
//        if (neighbourCallback != null)
//            neighbourCallback(m_tileMoveDesc);
    }
}

// *******************************************************************

TileEngine::TileEngine(int tileCount, qreal tileWorldSize, QObject *parent)
    : QObject(parent)
{
    m_tileCount = tileCount;
    if (m_tileCount % 2 != 0) {
        qmlWarning(this) << "tileCount must be a multiple of 2";
        m_tileCount = m_tileCount + 1;
    }

    m_tileWorldSize = tileWorldSize;
    m_tileCountHalf = tileCount / 2;
    m_tileMoveDesc.resize(m_tileCount);

    m_topRight.matrixCoord = QPoint(m_tileCount - 1, m_tileCount - 1);
    m_topRight.tileCoord = QPoint(m_tileCountHalf - 1, m_tileCountHalf - 1);
}

QVector3D TileEngine::worldPosForTileCoord(QPoint tileCoord)
{
    // NB: we here assume that the tile engine is aligned with x, z rather than x, y
    return QVector3D(tileCoord.x() * m_tileWorldSize, 0, tileCoord.y() * m_tileWorldSize);
}

QPoint TileEngine::tileCoordAtWorldPos(QVector3D worldPos)
{
    const int tileX = int(qFloor(worldPos.x() / m_tileWorldSize));
    const int tileY = int(qFloor(worldPos.z() / m_tileWorldSize));
    return QPoint(tileX, tileY);
}

void TileEngine::updateTiles(const QVector<TileDescription> &tiles)
{
    qDebug() << __FUNCTION__;
}

void TileEngine::updateNeighbours(const QVector<TileNeighbours> &neighbours)
{
    qDebug() << __FUNCTION__;
}

void TileEngine::setTargetPosition(QVector3D worldPos)
{
    if (worldPos == m_targetWorldPos)
        return;

    const QPoint prevShiftedTileCoord = tileCoordAtWorldPosShifted(m_targetWorldPos);
    const QPoint shiftedTileCoord = tileCoordAtWorldPosShifted(worldPos);
    m_targetWorldPos = worldPos;

    if (shiftedTileCoord == prevShiftedTileCoord)
        return;

    // Update matrix top-right
    const QPoint shiftedTiles = shiftedTileCoord - prevShiftedTileCoord;
    m_topRight.tileCoord += shiftedTiles;

    const int matrixRight = matrixPos(m_topRight.matrixCoord.x(), shiftedTiles.x());
    const int matrixTop = matrixPos(m_topRight.matrixCoord.y(), shiftedTiles.y());
    m_topRight.matrixCoord = QPoint(matrixRight, matrixTop);

    // Inform listeners about the change
    if (shiftedTiles.x() != 0)
        updateTilesHelp(shiftedTiles.x(), m_topRight.matrixCoord.x(), m_topRight.matrixCoord.y(), false);

    if (shiftedTiles.y() != 0)
        updateTilesHelp(shiftedTiles.y(), m_topRight.matrixCoord.y(), m_topRight.matrixCoord.x(), true);
}
