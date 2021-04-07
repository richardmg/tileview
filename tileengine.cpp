#include "tileengine.h"

#include <QtMath>

QVector3D TileView::mapTileCoordToPosition(QPoint tileCoord) const
{
    // NB: we here assume that the tile engine is aligned with x, z rather than x, y
    return QVector3D(tileCoord.x() * m_tileSize, 0, tileCoord.y() * m_tileSize);
}

QPoint TileView::mapPositionToTileCoord(QVector3D position) const
{
    const int tileX = int(qFloor(position.x() / m_tileSize));
    const int tileY = int(qFloor(position.z() / m_tileSize));
    return QPoint(tileX, tileY);
}

QPoint TileView::mapPositionToMatrixCoord(QVector3D position) const
{
    const QPoint tileCoord = mapPositionToTileCoord(position);
    const QPoint tileOffset = m_cornerTile.tileCoord - tileCoord;

    if (tileOffset.x() >= 0 && tileOffset.y() >= 0 && tileOffset.x() <= m_tileCount && tileOffset.y() <= m_tileCount) {
        qmlWarning(this) << "worldPos is outside the current area covered by the viewport";
        return QPoint();
    }

    const int matrixX = matrixCoordShifted(m_cornerTile.matrixCoord.x(), -tileOffset.x());
    const int matrixY = matrixCoordShifted(m_cornerTile.matrixCoord.y(), -tileOffset.y());
    return QPoint(matrixX, matrixY);
}

QPoint TileView::mapMatrixCoordToTileCoord(QPoint matrixCoord) const
{
    // Return which tile that maps to to given coordinate in the matrix. Which tile
    // that is depends on which tile the corner maps to. So we need to calculate
    // the offset between the matrix coordinate of the corner and the given argument.
    // This is most easily done by normalizing the corner tile so that we don't need to
    // take wrapping into account.
    const int coordXNorm = matrixCoordShifted(matrixCoord.x(), -m_cornerTile.matrixCoord.x() + (m_tileCount - 1));
    const int coordYNorm = matrixCoordShifted(matrixCoord.y(), -m_cornerTile.matrixCoord.y() + (m_tileCount - 1));
    const int offsetX = coordXNorm - m_tileCount + 1;
    const int offsetY = coordYNorm - m_tileCount + 1;
    const int tileX = m_cornerTile.tileCoord.x() + offsetX;
    const int tileY = m_cornerTile.tileCoord.y() + offsetY;

    return QPoint(tileX, tileY);
}

QPoint TileView::mapPositionToTileCoordShifted(QVector3D position) const
{
    // Note: tileCoordinateShifted is an internal concept, and is only used to
    // determine when to update the tile matrix. We use tileCoordinateShifted to
    // shift the position half a tile north-east to roll the matrix
    // when the user passes the center of a tile, rather than at the edge.
    const qreal offset = qreal(m_tileSize / 2.);
    const QVector3D shifted(position.x() + offset, position.y() + offset, position.z() + offset);
    return mapPositionToTileCoord(shifted);
}

/**
 * The user of TileEngine is supposed to have a matrix of m_tileCount * m_tileCount size.
 * Whenever our target position moves, new rows and columns with tiles will move into the
 * current "window" of tiles that the engine at any given time covers. Since we don't want
 * to move all the tiles in the matrix, e.g one row up or down when a new row appears, to make
 * room for the new row, we simply move a "pointer" instead. This pointer is called
 * m_cornerTile.matrixCoord. m_cornerTile.matrixCoord therefore points to the cells in the matrix
 * that is furthest away from the the target pos (north-east side).
 * We can simply move the pointer since we know that when one row appears at one side of the
 * matrix, the row on the oppsite side moves out, and is free to be used to show the new row.
 *
 * This function will return the new position of startCoord (which typically will be the
 * either m_cornerTile.matrixCoord.x() or m_cornerTile.matrixCoord.y()), and give you it's new
 * position the matrix if you shift it shiftCount in one direction. This might mean that
 * it wraps around on the other side of the matrix.
 */
int TileView::matrixCoordShifted(int startCoord, int shiftCount) const
{
    return (m_tileCount + startCoord + (shiftCount % m_tileCount)) % m_tileCount;
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

void TileView::resetAllTiles()
{
    if (!isComponentComplete())
        return;

    if (m_tileCount % 2 != 0) {
        qmlWarning(this) << "tileCount must be a multiple of 2";
        m_tileCount = m_tileCount + 1;
    }

    m_cornerTile.matrixCoord = QPoint(m_tileCount - 1, m_tileCount - 1);
    m_cornerTile.tileCoord = m_cornerTile.matrixCoord;
    m_cornerTile.position = mapTileCoordToPosition(m_cornerTile.tileCoord);

    recreateDelegates();

    m_tilesToUpdate.resize(m_tileCount);
    for (int matrixY = 0; matrixY < m_tileCount; ++matrixY) {
        for (int matrixX = 0; matrixX < m_tileCount; ++matrixX) {
            Tile &tile = m_tilesToUpdate[matrixX];

            tile.matrixCoord = QPoint(matrixX, matrixY);
            tile.tileCoord = tile.matrixCoord;
            tile.position = mapTileCoordToPosition(tile.tileCoord);
//            setNeighbours(m_tileMoveDesc[matrixX].matrixCoord, ref m_tileMoveDesc[matrixX].neighbours);
        }

        updateDelegates(m_tilesToUpdate);
//        updateNeighbours(m_tileMoveDesc);
    }
}

void TileView::shiftMatrix(int shiftCount, bool alongYAxis)
{
    const int cornerX = alongYAxis ? m_cornerTile.matrixCoord.y() : m_cornerTile.matrixCoord.x();
    const int cornerY = alongYAxis ? m_cornerTile.matrixCoord.x() : m_cornerTile.matrixCoord.y();

    int moveDirection = shiftCount > 0 ? 1 : -1;
    shiftCount = qMin(qAbs(shiftCount), m_tileCount);
    int shiftCountIncludingNeighbours = shiftCount;// + (neighbourCallback != null ? 1 : 0);

    for (int i = 0; i < shiftCountIncludingNeighbours; ++i) {
        int matrixFrontX = matrixCoordShifted(cornerX, i * -moveDirection);
        if (moveDirection < 0)
            matrixFrontX = matrixCoordShifted(matrixFrontX, 1);

        for (int j = 0; j < m_tileCount; ++j) {
            Tile &tile = m_tilesToUpdate[j];

            QPoint matrixCoord(matrixFrontX, matrixCoordShifted(cornerY, -j));
            if (alongYAxis) {
                const int tmp = matrixCoord.x();
                matrixCoord.setX(matrixCoord.y());
                matrixCoord.setY(tmp);
            }

            tile.matrixCoord = matrixCoord;
            tile.tileCoord = mapMatrixCoordToTileCoord(matrixCoord);
            tile.position = mapTileCoordToPosition(m_tilesToUpdate[j].tileCoord);

//            if (neighbourCallback != null)
//                setNeighbours(matrixCoord, ref m_tileMoveDesc[j].neighbours);
        }

        if (i < shiftCount)
            updateDelegates(m_tilesToUpdate);

//        if (neighbourCallback != null)
//            neighbourCallback(m_tileMoveDesc);
    }
}

void TileView::recreateDelegates()
{
    qDeleteAll(m_delegateNodes);
    m_delegateNodes.clear();

    const int delegateCount = m_tileCount * m_tileCount;
    m_delegateNodes.reserve(delegateCount);

    // Create all delegate items
    for (int i = 0; i < delegateCount; ++i) {
        QObject *obj = m_delegate->create();
        QQuick3DNode *node = qobject_cast<QQuick3DNode *>(obj);
        if (!node) {
            qmlWarning(this) << "Delegate is not a Node";
            delete node;
            node = new QQuick3DNode();
        }
        node->setParentItem(this);
        node->setParent(this);
        node->setVisible(true);
        m_delegateNodes.append(node);
    }
}

void TileView::updateDelegates(const QVector<Tile> &tiles)
{
    // NOTE: Should I take scale into account?
    const qreal centerDistance = (m_tileCount - 1) * m_tileSize / 2;
    const QVector3D centerVector(centerDistance, 0, centerDistance);

    for (const auto &tile : qAsConst(tiles)) {
        int index = tile.matrixCoord.x() + (tile.matrixCoord.y() * m_tileCount);
        QQuick3DNode *node = m_delegateNodes[index];
        node->setPosition(tile.position - centerVector);
        getAttachedObject(node)->setTile(QVector3D(tile.tileCoord.x(), 0, tile.tileCoord.y()));
    }
}

void TileView::updateNeighbours(const QVector<TileNeighbours> &neighbours)
{
    qDebug() << __FUNCTION__;
}

void TileView::componentComplete()
{
    QQuick3DNode::componentComplete();
    resetAllTiles();
}

TileViewAttached *TileView::getAttachedObject(const QObject *obj) const
{
    QObject *attachedObject = qmlAttachedPropertiesObject<TileView>(obj);
    return static_cast<TileViewAttached *>(attachedObject);
}

TileViewAttached *TileView::qmlAttachedProperties(QObject *obj)
{
    return new TileViewAttached(obj);
}

// *******************************************************************

TileView::TileView(QQuick3DNode *parent)
    : QQuick3DNode(parent)
{
}

TileView::~TileView()
{
    qDeleteAll(m_delegateNodes);
}

QQmlComponent *TileView::delegate() const
{
    return m_delegate;
}

void TileView::setDelegate(QQmlComponent *delegate)
{
    if (m_delegate == delegate)
        return;

    m_delegate = delegate;
    resetAllTiles();

    emit delegateChanged();
}

QVector3D TileView::center() const
{
    return m_centerPosition;
}

int TileView::tileCount() const
{
    return m_tileCount;
}

void TileView::setTileCount(int tileCount)
{
    if (m_tileCount == tileCount)
        return;

    m_tileCount = tileCount;
    resetAllTiles();

    emit tileCountChanged();
}

qreal TileView::tileSize() const
{
    return m_tileSize;
}

void TileView::setTileSize(qreal tileSize)
{
    if (qFuzzyCompare(m_tileSize, tileSize))
        return;

    m_tileSize = tileSize;
    emit tileSizeChanged();
}

void TileView::setCenter(QVector3D center)
{
    if (center == m_centerPosition)
        return;

    const QVector3D oldCenterPosition = m_centerPosition;
    m_centerPosition = center;

    if (!isComponentComplete())
        return;

    const QPoint oldTileCoord = mapPositionToTileCoordShifted(oldCenterPosition);
    const QPoint newTileCoord = mapPositionToTileCoordShifted(m_centerPosition);

    if (oldTileCoord == newTileCoord)
        return;

    // Update matrix top-right
    const QPoint shiftedTiles = newTileCoord - oldTileCoord;
    m_cornerTile.tileCoord += shiftedTiles;

    const int matrixCoordX = matrixCoordShifted(m_cornerTile.matrixCoord.x(), shiftedTiles.x());
    const int matrixCoordY = matrixCoordShifted(m_cornerTile.matrixCoord.y(), shiftedTiles.y());
    m_cornerTile.matrixCoord = QPoint(matrixCoordX, matrixCoordY);

    if (shiftedTiles.x() != 0)
        shiftMatrix(shiftedTiles.x(), false);

    if (shiftedTiles.y() != 0)
        shiftMatrix(shiftedTiles.y(), true);

    emit centerChanged();
}

TileViewAttached::TileViewAttached(QObject *parent)
    : QObject(parent)
{
}

TileView *TileViewAttached::view() const
{
    return m_view;
}

void TileViewAttached::setView(TileView *tileView)
{
    if (tileView == m_view)
        return;

    m_view = tileView;

    emit viewChanged();
}

QVector3D TileViewAttached::tile() const
{
    return m_tile;
}

void TileViewAttached::setTile(const QVector3D &tile)
{
    // Even if m_tile doesn't change for the one tile at start up
    // that happens to be on 0,0,0, we emit it as changed anyway.
    // That way the delegate can know in a uniform way, when it's
    // time to update it's contents.
    if (m_tile == tile && !m_tile.isNull())
        return;

    m_tile = tile;

    emit tileChanged();

}
