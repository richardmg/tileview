#include "tileengine.h"

#include <QtMath>

QPoint TileEngine::matrixCoordForWorldPos(QVector3D worldPos) const
{
    const QPoint tileCoord = tileCoordAtWorldPos(worldPos);
    const QPoint tileOffset = m_topRight.tileCoord - tileCoord;

    if (tileOffset.x() >= 0 && tileOffset.y() >= 0 && tileOffset.x() <= m_rowCount && tileOffset.y() <= m_rowCount) {
        qmlWarning(this) << "worldPos is outside the current area covered by the viewport";
        return QPoint();
    }

    const int matrixX = matrixPos(m_topRight.matrixCoord.x(), -tileOffset.x());
    const int matrixY = matrixPos(m_topRight.matrixCoord.y(), -tileOffset.y());
    return QPoint(matrixX, matrixY);
}

QPoint TileEngine::tileCoordForMatrixCoord(QPoint matrixCoord) const
{
    // Normalize matrix coord (as if the matrix were unshifted)
    const int matrixXNorm = matrixPos(matrixCoord.x(), -m_topRight.matrixCoord.x() + (m_rowCount - 1));
    const int matrixYNorm = matrixPos(matrixCoord.y(), -m_topRight.matrixCoord.y() + (m_rowCount - 1));

    const int tileOffsetX = m_rowCount - matrixXNorm - 1;
    const int tileOffsetY = m_rowCount - matrixYNorm - 1;

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
int TileEngine::matrixPos(int startEdge, int edgeShifted) const
{
    return (m_rowCount + startEdge + (edgeShifted % m_rowCount)) % m_rowCount;
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
    for (int matrixY = 0; matrixY < m_rowCount; ++matrixY) {
        for (int matrixX = 0; matrixX < m_rowCount; ++matrixX) {
            TileDescription &desc = m_tileMoveDesc[matrixX];

            desc.matrixCoord = QPoint(matrixX, matrixY);
            desc.tileCoord = tileCoordForMatrixCoord(desc.matrixCoord);
            desc.worldPos = worldPosAtTileCoord(desc.tileCoord);
            // TODO: ensure that we m_tileMoveDesc was updated!

//            setNeighbours(m_tileMoveDesc[matrixX].matrixCoord, ref m_tileMoveDesc[matrixX].neighbours);
        }

        updateTiles(m_tileMoveDesc);
//        updateNeighbours(m_tileMoveDesc);
    }
}

QPoint TileEngine::tileCoordAtWorldPosShifted(QVector3D worldPos) const
{
    // Note: tileCoordAtWorldPosShifted is an internal concept, and is only used to
    // determine when to update the tile matrix. We use tileCoordAtWorldPosShifted to
    // shift the target position half a tile north-east to roll the matrix
    // when the user passes the center of a tile, rather than at the edge.
    const qreal offset = qreal(m_tileSize / 2.);
    const QVector3D shifted(worldPos.x() + offset, worldPos.y() + offset, worldPos.z() + offset);
    return tileCoordAtWorldPos(shifted);
}

void TileEngine::updateTilesHelp(int shifted, int topRightX, int topRightY, bool updateAxisY)
{
    int moveDirection = shifted > 0 ? 1 : -1;
    int shiftCount = qMin(qAbs(shifted), m_rowCount);
    int shiftCountIncludingNeighbours = shiftCount;// + (neighbourCallback != null ? 1 : 0);

    for (int i = 0; i < shiftCountIncludingNeighbours; ++i) {
        int matrixFrontX = matrixPos(topRightX, i * -moveDirection);
        if (moveDirection < 0)
            matrixFrontX = matrixPos(matrixFrontX, 1);

        for (int j = 0; j < m_rowCount; ++j) {
            QPoint &matrixCoord = m_tileMoveDesc[j].matrixCoord;
            matrixCoord = QPoint(matrixFrontX, matrixPos(topRightY, -j));
            if (updateAxisY) {
                const int tmp = matrixCoord.x();
                matrixCoord.setX(matrixCoord.y());
                matrixCoord.setY(tmp);
            }

            m_tileMoveDesc[j].tileCoord = tileCoordForMatrixCoord(matrixCoord);
            m_tileMoveDesc[j].worldPos = worldPosAtTileCoord(m_tileMoveDesc[j].tileCoord);

//            if (neighbourCallback != null)
//                setNeighbours(matrixCoord, ref m_tileMoveDesc[j].neighbours);
        }

        if (i < shiftCount)
            updateTiles(m_tileMoveDesc);
//        if (neighbourCallback != null)
//            neighbourCallback(m_tileMoveDesc);
    }
}

void TileEngine::createDelegates()
{

    for (int matrixY = 0; matrixY < m_rowCount; ++matrixY) {
        for (int matrixX = 0; matrixX < m_rowCount; ++matrixX) {
            TileDescription &desc = m_tileMoveDesc[matrixX];

            desc.matrixCoord = QPoint(matrixX, matrixY);
            desc.tileCoord = tileCoordForMatrixCoord(desc.matrixCoord);
            desc.worldPos = worldPosAtTileCoord(desc.tileCoord);
            // TODO: ensure that we m_tileMoveDesc was updated!

//            setNeighbours(m_tileMoveDesc[matrixX].matrixCoord, ref m_tileMoveDesc[matrixX].neighbours);
        }
    }
}

// *******************************************************************

TileEngine::TileEngine(QObject *parent)
    : QObject(parent)
{
}

TileEngine::~TileEngine()
{
}

QVector3D TileEngine::worldPosAtTileCoord(QPoint tileCoord) const
{
    // NB: we here assume that the tile engine is aligned with x, z rather than x, y
    return QVector3D(tileCoord.x() * m_tileSize, 0, tileCoord.y() * m_tileSize);
}

QPoint TileEngine::tileCoordAtWorldPos(QVector3D worldPos) const
{
    const int tileX = int(qFloor(worldPos.x() / m_tileSize));
    const int tileY = int(qFloor(worldPos.z() / m_tileSize));
    return QPoint(tileX, tileY);
}

void TileEngine::updateTiles(const QVector<TileDescription> &tiles)
{
    qDebug() << "Update tiles:";
    for (const auto &tile : qAsConst(tiles)) {
        qDebug() << "   tile:" << tile.tileCoord << "pos:" << tile.worldPos << "matrix coord:" << tile.matrixCoord;
    }
}

void TileEngine::updateNeighbours(const QVector<TileNeighbours> &neighbours)
{
    qDebug() << __FUNCTION__;
}

QQmlComponent *TileEngine::delegate() const
{
    return m_delegate;
}

void TileEngine::setDelegate(QQmlComponent *delegate)
{
    if (m_delegate == delegate)
        return;

    m_delegate = delegate;
    emit delegateChanged();
}

void TileEngine::componentComplete()
{
    if (m_rowCount % 2 != 0) {
        qmlWarning(this) << "rowCount must be a multiple of 2";
        m_rowCount = m_rowCount + 1;
    }

    // TODO: Take m_worldPos into account when calculating initial coordinates!

    m_tileMoveDesc.resize(m_rowCount);
    m_topRight.matrixCoord = QPoint(m_rowCount - 1, m_rowCount - 1);
    m_topRight.tileCoord = QPoint((m_rowCount / 2) - 1, (m_rowCount / 2) - 1);

    updateAllTiles();

    m_componentComplete = true;
}

QVector3D TileEngine::targetPosition() const
{
    return m_targetPosition;
}

int TileEngine::rowCount() const
{
    return m_rowCount;
}

qreal TileEngine::tileSize() const
{
    return m_tileSize;
}

void TileEngine::setRowCount(int rowCount)
{
    if (m_rowCount == rowCount)
        return;

    m_rowCount = rowCount;
    emit rowCountChanged();
}

void TileEngine::setTileSize(qreal tileSize)
{
    if (qFuzzyCompare(m_tileSize, tileSize))
        return;

    m_tileSize = tileSize;
    emit tileSizeChanged();
}

void TileEngine::setTargetPosition(QVector3D position)
{
    if (position == m_targetPosition)
        return;

    const QVector3D prevWorldPos = m_targetPosition;
    m_targetPosition = position;

    if (!m_componentComplete)
        return;

    const QPoint prevShiftedTileCoord = tileCoordAtWorldPosShifted(prevWorldPos);
    const QPoint shiftedTileCoord = tileCoordAtWorldPosShifted(position);

    if (shiftedTileCoord == prevShiftedTileCoord)
        return;

    // Update matrix top-right
    const QPoint shiftedTiles = shiftedTileCoord - prevShiftedTileCoord;
    m_topRight.tileCoord += shiftedTiles;

    const int matrixRight = matrixPos(m_topRight.matrixCoord.x(), shiftedTiles.x());
    const int matrixTop = matrixPos(m_topRight.matrixCoord.y(), shiftedTiles.y());
    m_topRight.matrixCoord = QPoint(matrixRight, matrixTop);

    if (shiftedTiles.x() != 0)
        updateTilesHelp(shiftedTiles.x(), m_topRight.matrixCoord.x(), m_topRight.matrixCoord.y(), false);

    if (shiftedTiles.y() != 0)
        updateTilesHelp(shiftedTiles.y(), m_topRight.matrixCoord.y(), m_topRight.matrixCoord.x(), true);

    emit targetPositionChanged();
}
