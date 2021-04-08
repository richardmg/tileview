#include "tileengine.h"

#include <QtMath>

QVector3D TileView::mapTileCoordToPosition(QVector3D tileCoord) const
{
    const qreal coordX = m_tileCount.x() > 1 ? tileCoord.x() : 0;
    const qreal coordY = m_tileCount.y() > 1 ? tileCoord.y() : 0;
    const qreal coordZ = m_tileCount.z() > 1 ? tileCoord.z() : 0;
    return QVector3D(coordX, coordY, coordZ) * m_tileSize;
}

QVector3D TileView::mapPositionToTileCoord(QVector3D position) const
{
    const int tileX = m_tileCount.x() > 1 ? int(qFloor(position.x() / m_tileSize.x())) : 0;
    const int tileY = m_tileCount.y() > 1 ? int(qFloor(position.y() / m_tileSize.y())) : 0;
    const int tileZ = m_tileCount.z() > 1 ? int(qFloor(position.z() / m_tileSize.z())) : 0;
    return QVector3D(tileX, tileY, tileZ);
}

QVector3D TileView::mapPositionToMatrixCoord(QVector3D position) const
{
    const QVector3D tileCoord = mapPositionToTileCoord(position);
    const QVector3D tileOffset = m_cornerTile.tileCoord - tileCoord;
    const int matrixX = matrixCoordShiftedX(m_cornerTile.matrixCoord.x(), -tileOffset.x());
    const int matrixY = matrixCoordShiftedY(m_cornerTile.matrixCoord.y(), -tileOffset.y());
    const int matrixZ = matrixCoordShiftedZ(m_cornerTile.matrixCoord.z(), -tileOffset.z());
    return QVector3D(matrixX, matrixY, matrixZ);
}

QVector3D TileView::mapMatrixCoordToTileCoord(QVector3D matrixCoord) const
{
    // Return which tile that maps to to given coordinate in the matrix. Which tile
    // that is depends on which tile the corner maps to. So we need to calculate
    // the offset between the matrix coordinate of the corner and the given argument.
    // This is most easily done by normalizing the corner tile so that we don't need to
    // take wrapping into account.
    const int coordXNorm = matrixCoordShiftedX(matrixCoord.x(), -m_cornerTile.matrixCoord.x() + (int(m_tileCount.x()) - 1));
    const int coordYNorm = matrixCoordShiftedY(matrixCoord.y(), -m_cornerTile.matrixCoord.y() + (int(m_tileCount.y()) - 1));
    const int coordZNorm = matrixCoordShiftedZ(matrixCoord.z(), -m_cornerTile.matrixCoord.z() + (int(m_tileCount.z()) - 1));
    const int offsetX = coordXNorm - int(m_tileCount.x()) + 1;
    const int offsetY = coordYNorm - int(m_tileCount.y()) + 1;
    const int offsetZ = coordZNorm - int(m_tileCount.z()) + 1;
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
    const QVector3D offset = m_tileSize / 2.;
    const QVector3D shifted(position + offset);
    return mapPositionToTileCoord(shifted);
}

/**
 * The user of TileEngine is supposed to have a matrix of tileCountX() * tileCountX() size.
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
int TileView::matrixCoordShiftedX(int startCoord, int shiftCount) const
{
    const int count = int(m_tileCount.x());
    return (count + startCoord + (shiftCount % count)) % count;
}

int TileView::matrixCoordShiftedY(int startCoord, int shiftCount) const
{
    const int count = int(m_tileCount.y());
    return (count + startCoord + (shiftCount % count)) % count;
}

int TileView::matrixCoordShiftedZ(int startCoord, int shiftCount) const
{
    const int count = int(m_tileCount.z());
    return (count + startCoord + (shiftCount % count)) % count;
}

void TileView::resetAllTiles()
{
    if (!isComponentComplete())
        return;

    m_cornerTile.matrixCoord = QVector3D(int(m_tileCount.x()) - 1, int(m_tileCount.y()) - 1, int(m_tileCount.z()) - 1);
    m_cornerTile.tileCoord = m_cornerTile.matrixCoord;
    m_cornerTile.position = mapTileCoordToPosition(m_cornerTile.tileCoord);

    recreateDelegates();

    Tile tile;
    for (int matrixZ = 0; matrixZ < int(m_tileCount.z()); ++matrixZ) {
        for (int matrixY = 0; matrixY < int(m_tileCount.y()); ++matrixY) {
            for (int matrixX = 0; matrixX < int(m_tileCount.x()); ++matrixX) {
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
    const int newCornerX = matrixCoordShiftedX(cornerX, shiftCount);
    m_cornerTile.matrixCoord.setX(newCornerX);
}

void TileView::shiftMatrixAlongY(int shiftCount)
{
    // Update corner tile
    m_cornerTile.tileCoord += QVector3D(0, shiftCount, 0);
    const int cornerY = int(m_cornerTile.matrixCoord.y());
    const int newCornerY = matrixCoordShiftedY(cornerY, shiftCount);
    m_cornerTile.matrixCoord.setY(newCornerY);
}

void TileView::shiftMatrixAlongZ(int shiftCount)
{
    // Update corner tile
    m_cornerTile.tileCoord += QVector3D(0, 0, shiftCount);
    const int cornerZ = int(m_cornerTile.matrixCoord.z());
    const int newCornerZ = matrixCoordShiftedZ(cornerZ, shiftCount);
    m_cornerTile.matrixCoord.setZ(newCornerZ);
}

void TileView::updateTiles()
{
    Tile tile;
    for (int matrixZ = 0; matrixZ < int(m_tileCount.z()); ++matrixZ) {
        for (int matrixY = 0; matrixY < int(m_tileCount.y()); ++matrixY) {
            for (int matrixX = 0; matrixX < int(m_tileCount.x()); ++matrixX) {
                tile.matrixCoord = QVector3D(matrixX, matrixY, matrixZ);
                tile.tileCoord = mapMatrixCoordToTileCoord(tile.matrixCoord);
                tile.position = mapTileCoordToPosition(tile.tileCoord);
                updateDelegate(tile);
            }
        }
    }
}

// *******************************************************************

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

void TileView::recreateDelegates()
{
    qDeleteAll(m_delegateNodes);
    m_delegateNodes.clear();

    const int delegateCount = int(m_tileCount.x()) * int(m_tileCount.y()) * int(m_tileCount.z());
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
        node->setVisible(false);
        m_delegateNodes.append(node);
    }
}

void TileView::updateDelegate(const Tile &tile)
{
    int index = tile.matrixCoord.x()
            + (tile.matrixCoord.y() * int(m_tileCount.x()))
            + (tile.matrixCoord.z() * int(m_tileCount.x()) * int(m_tileCount.y()));
    QQuick3DNode *node = m_delegateNodes[index];

    const QVector3D centerVector((int(m_tileCount.x()) - 1) * m_tileSize.x() / 2,
                                 (int(m_tileCount.y()) - 1) * m_tileSize.y() / 2,
                                 (int(m_tileCount.z()) - 1) * m_tileSize.z() / 2);
    const QVector3D delegatePosition = tile.position - centerVector;

    const float inFrontOfCamera = QVector3D::dotProduct(delegatePosition - m_centerPosition, m_direction) > 0;
    node->setVisible(inFrontOfCamera || m_direction.isNull());

    if (node->visible()) {
        // Only tell the delegate to update / rebuild if it's actually visible
        node->setPosition(delegatePosition);
        getAttachedObject(node)->setTile(tile.tileCoord);
    }
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

QVector3D TileView::tileCount() const
{
    return m_tileCount;
}

void TileView::setTileCount(const QVector3D &tileCount)
{
    if (m_tileCount == tileCount)
        return;

    m_tileCount = tileCount;
    resetAllTiles();
    emit tileCountChanged();
}

QVector3D TileView::tileSize() const
{
    return m_tileSize;
}

void TileView::setTileSize(QVector3D tileSize)
{
    if (m_tileSize == tileSize)
        return;

    m_tileSize = tileSize;
    emit tileSizeChanged();
}

void TileView::setCenter(const QVector3D &center)
{
    if (center == m_centerPosition)
        return;

    const QVector3D oldCenterPosition = m_centerPosition;
    m_centerPosition = center;

    if (!isComponentComplete())
        return;

    const QVector3D oldTileCoord = mapPositionToTileCoordShifted(oldCenterPosition);
    const QVector3D newTileCoord = mapPositionToTileCoordShifted(m_centerPosition);

    if (oldTileCoord != newTileCoord) {

        // RENAME TO shiftCornerX

        const QVector3D shiftedTiles = newTileCoord - oldTileCoord;
        if (shiftedTiles.x() != 0 && m_tileCount.x() > 1)
            shiftMatrixAlongX(shiftedTiles.x());
        if (shiftedTiles.y() != 0 && m_tileCount.y() > 1)
            shiftMatrixAlongY(shiftedTiles.y());
        if (shiftedTiles.z() != 0 && m_tileCount.z() > 1)
            shiftMatrixAlongZ(shiftedTiles.z());
    }

    updateTiles();

    emit centerChanged();
}

QVector3D TileView::direction() const
{
    return m_direction;
}

void TileView::setDirection(QVector3D direction)
{
    if (m_direction == direction)
        return;

    m_direction = direction;
    updateTiles();
    emit directionChanged();
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
