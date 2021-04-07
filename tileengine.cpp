#include "tileengine.h"

#include <QtMath>

QVector3D TileView::mapTileCoordToPosition(QVector3D tileCoord) const
{
    return tileCoord * m_tileSize;
}

QVector3D TileView::mapPositionToTileCoord(QVector3D position) const
{
    const int tileX = int(qFloor(position.x() / m_tileSize));
    const int tileY = int(qFloor(position.y() / m_tileSize));
    const int tileZ = int(qFloor(position.z() / m_tileSize));
    return QVector3D(tileX, tileY, tileZ);
}

QVector3D TileView::mapPositionToMatrixCoord(QVector3D position) const
{
    const QVector3D tileCoord = mapPositionToTileCoord(position);
    const QVector3D tileOffset = m_cornerTile.tileCoord - tileCoord;
    const int matrixX = matrixCoordShifted(m_cornerTile.matrixCoord.x(), -tileOffset.x());
    const int matrixY = matrixCoordShifted(m_cornerTile.matrixCoord.y(), -tileOffset.y());
    const int matrixZ = matrixCoordShifted(m_cornerTile.matrixCoord.z(), -tileOffset.z());
    return QVector3D(matrixX, matrixY, matrixZ);
}

QVector3D TileView::mapMatrixCoordToTileCoord(QVector3D matrixCoord) const
{
    // Return which tile that maps to to given coordinate in the matrix. Which tile
    // that is depends on which tile the corner maps to. So we need to calculate
    // the offset between the matrix coordinate of the corner and the given argument.
    // This is most easily done by normalizing the corner tile so that we don't need to
    // take wrapping into account.
    const int coordXNorm = matrixCoordShifted(matrixCoord.x(), -m_cornerTile.matrixCoord.x() + (m_tileCount - 1));
    const int coordYNorm = matrixCoordShifted(matrixCoord.y(), -m_cornerTile.matrixCoord.y() + (m_tileCount - 1));
    const int coordZNorm = matrixCoordShifted(matrixCoord.z(), -m_cornerTile.matrixCoord.z() + (m_tileCount - 1));
    const int offsetX = coordXNorm - m_tileCount + 1;
    const int offsetY = coordYNorm - m_tileCount + 1;
    const int offsetZ = coordZNorm - m_tileCount + 1;
    const int tileX = m_cornerTile.tileCoord.x() + offsetX;
    const int tileY = m_cornerTile.tileCoord.y() + offsetY;
    const int tileZ = m_cornerTile.tileCoord.z() + offsetZ;
    return QVector3D(tileX, tileY, tileZ);
}

QVector3D TileView::mapPositionToTileCoordShifted(QVector3D position) const
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

void TileView::resetAllTiles()
{
    if (!isComponentComplete())
        return;

    if (m_tileCount % 2 != 0) {
        qmlWarning(this) << "tileCount must be a multiple of 2";
        m_tileCount = m_tileCount + 1;
    }

    m_cornerTile.matrixCoord = QVector3D(m_tileCount - 1, m_tileCount - 1, m_tileCount - 1);
    m_cornerTile.tileCoord = m_cornerTile.matrixCoord;
    m_cornerTile.position = mapTileCoordToPosition(m_cornerTile.tileCoord);

    recreateDelegates();

    Tile tile;
    for (int matrixZ = 0; matrixZ < m_tileCount; ++matrixZ) {
        for (int matrixY = 0; matrixY < m_tileCount; ++matrixY) {
            for (int matrixX = 0; matrixX < m_tileCount; ++matrixX) {
                tile.matrixCoord = QVector3D(matrixX, matrixY, matrixZ);
                tile.tileCoord = tile.matrixCoord;
                tile.position = mapTileCoordToPosition(tile.tileCoord);
                updateDelegate(tile);
            }
        }
    }
}

void TileView::shiftMatrixAlongX(int shiftCount)
{
    // Update corner tile
    m_cornerTile.tileCoord += QVector3D(shiftCount, 0, 0);
    const int cornerX = int(m_cornerTile.matrixCoord.x());
    const int newCornerX = matrixCoordShifted(cornerX, shiftCount);
    m_cornerTile.matrixCoord.setX(newCornerX);

    const int direction = shiftCount > 0 ? 1 : -1;
    shiftCount = qMin(qAbs(shiftCount), m_tileCount);
    Tile tile;

    // Shift delegate items according to the new corner tile
    for (int x = 0; x < shiftCount; ++x) {
        int xPlaneToMove = matrixCoordShifted(cornerX, x * -direction);
        if (direction > 0) {
            // When we go up the positive axis, we don't want to update the
            // plane at the corner (which is the one visually in front).
            // Instead we want to update the plane at the opposite end, and
            // place it in front. We can get that side by simply shifting the
            // position by 1, since that will wrap it over to the other side.
            xPlaneToMove = matrixCoordShifted(xPlaneToMove, 1);
        }
        for (int z = 0; z < m_tileCount; ++z) {
            for (int y = 0; y < m_tileCount; ++y) {
                tile.matrixCoord = QVector3D(xPlaneToMove, y, z);
                tile.tileCoord = mapMatrixCoordToTileCoord(tile.matrixCoord);
                tile.position = mapTileCoordToPosition(tile.tileCoord);
                updateDelegate(tile);
            }
        }
    }
}

void TileView::shiftMatrixAlongY(int shiftCount)
{
    // Update corner tile
    m_cornerTile.tileCoord += QVector3D(0, shiftCount, 0);
    const int cornerY = int(m_cornerTile.matrixCoord.y());
    const int newCornerY = matrixCoordShifted(cornerY, shiftCount);
    m_cornerTile.matrixCoord.setY(newCornerY);

    const int direction = shiftCount > 0 ? 1 : -1;
    shiftCount = qMin(qAbs(shiftCount), m_tileCount);
    Tile tile;

    // Shift delegate items according to the new corner tile
    for (int y = 0; y < shiftCount; ++y) {
        int yPlaneToMove = matrixCoordShifted(cornerY, y * -direction);
        if (direction > 0) {
            // When we go up the positive axis, we don't want to update the
            // plane at the corner (which is the one visually in front).
            // Instead we want to update the plane at the opposite end, and
            // place it in front. We can get that side by simply shifting the
            // position by 1, since that will wrap it over to the other side.
            yPlaneToMove = matrixCoordShifted(yPlaneToMove, 1);
        }
        for (int z = 0; z < m_tileCount; ++z) {
            for (int x = 0; x < m_tileCount; ++x) {
                tile.matrixCoord = QVector3D(x, yPlaneToMove, z);
                tile.tileCoord = mapMatrixCoordToTileCoord(tile.matrixCoord);
                tile.position = mapTileCoordToPosition(tile.tileCoord);
                updateDelegate(tile);
            }
        }
    }
}

void TileView::shiftMatrixAlongZ(int shiftCount)
{
    // Update corner tile
    m_cornerTile.tileCoord += QVector3D(0, 0, shiftCount);
    const int cornerZ = int(m_cornerTile.matrixCoord.z());
    const int newCornerZ = matrixCoordShifted(cornerZ, shiftCount);
    m_cornerTile.matrixCoord.setZ(newCornerZ);

    const int direction = shiftCount > 0 ? 1 : -1;
    shiftCount = qMin(qAbs(shiftCount), m_tileCount);
    Tile tile;

    // Shift delegate items according to the new corner tile
    for (int z = 0; z < shiftCount; ++z) {
        int zPlaneToMove = matrixCoordShifted(cornerZ, z * -direction);
        if (direction > 0) {
            // When we go up the positive axis, we don't want to update the
            // plane at the corner (which is the one visually in front).
            // Instead we want to update the plane at the opposite end, and
            // place it in front. We can get that side by simply shifting the
            // position by 1, since that will wrap it over to the other side.
            zPlaneToMove = matrixCoordShifted(zPlaneToMove, 1);
        }
        for (int x = 0; x < m_tileCount; ++x) {
            for (int y = 0; y < m_tileCount; ++y) {
                tile.matrixCoord = QVector3D(x, y, zPlaneToMove);
                tile.tileCoord = mapMatrixCoordToTileCoord(tile.matrixCoord);
                tile.position = mapTileCoordToPosition(tile.tileCoord);
                updateDelegate(tile);
            }
        }
    }
}

void TileView::recreateDelegates()
{
    qDeleteAll(m_delegateNodes);
    m_delegateNodes.clear();

    const int delegateCount = m_tileCount * m_tileCount * m_tileCount;
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

void TileView::updateDelegate(const Tile &tile)
{
    // NOTE: Should I take scale into account?

    int index = tile.matrixCoord.x()
            + (tile.matrixCoord.y() * m_tileCount)
            + (tile.matrixCoord.z() * m_tileCount * m_tileCount);
    QQuick3DNode *node = m_delegateNodes[index];

    const qreal centerDistance = (m_tileCount - 1) * m_tileSize / 2;
    const QVector3D centerVector(centerDistance, centerDistance, centerDistance);
    node->setPosition(tile.position - centerVector);
    getAttachedObject(node)->setTile(tile.tileCoord);
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

    const QVector3D oldTileCoord = mapPositionToTileCoordShifted(oldCenterPosition);
    const QVector3D newTileCoord = mapPositionToTileCoordShifted(m_centerPosition);

    if (oldTileCoord == newTileCoord)
        return;

    const QVector3D shiftedTiles = newTileCoord - oldTileCoord;

    if (shiftedTiles.x() != 0)
        shiftMatrixAlongX(shiftedTiles.x());
    if (shiftedTiles.y() != 0)
        shiftMatrixAlongY(shiftedTiles.y());
    if (shiftedTiles.z() != 0)
        shiftMatrixAlongZ(shiftedTiles.z());

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
