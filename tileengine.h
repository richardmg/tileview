#ifndef TILEENGINE_H
#define TILEENGINE_H

#include <QtCore/QtCore>
#include <QtQml/QtQml>
#include <QtQuick3D/QtQuick3D>
#include <QtQuick3D/private/qquick3dnode_p.h>

const QPoint kNoNeighbour(-1, -1);

struct TileNeighbours
{
    QPoint left = kNoNeighbour;
    QPoint right = kNoNeighbour;
    QPoint top = kNoNeighbour;
    QPoint bottom = kNoNeighbour;
};

struct Tile
{
    QQuick3DNode *node;
    QVector3D position;
    QPoint tileCoord;
    QPoint matrixCoord;
    TileNeighbours neighbours;
};

class TileView : public QQuick3DNode
{
    Q_OBJECT
    QML_ELEMENT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(int rowCount READ rowCount WRITE setRowCount NOTIFY rowCountChanged)
    Q_PROPERTY(qreal tileSize READ tileSize WRITE setTileSize NOTIFY tileSizeChanged)
    Q_PROPERTY(QVector3D center READ center WRITE setCenter NOTIFY centerChanged)
    Q_PROPERTY(QQmlComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)

public:
    explicit TileView(QQuick3DNode *parent = nullptr);
    ~TileView() override;

    int rowCount() const;
    void setRowCount(int rowCount);

    qreal tileSize() const;
    void setTileSize(qreal tileSize);

    QVector3D center() const;
    void setCenter(QVector3D center);

    QQmlComponent* delegate() const;
    void setDelegate(QQmlComponent *delegate);

signals:
    void rowCountChanged();
    void tileSizeChanged();
    void centerChanged();
    void delegateChanged();

public:
    virtual void updateDelegateNodes(const QVector<Tile> &tiles);
    virtual void updateNeighbours(const QVector<TileNeighbours> &neighbours);

protected:
    void componentComplete() override;

private:
    QVector3D mapTileCoordToPosition(QPoint tileCoord) const;
    QPoint mapPositionToTileCoordinate(QVector3D worldPos) const;
    int matrixPos(int startEdge, int edgeShifted) const;
    QPoint mapPositionToMatrix(QVector3D worldPos) const;
    QPoint mapMatrixCoordToTileCoord(QPoint matrixCoord) const;

//    void setNeighbours(QPoint pos, TileNeighbours &result);
    void resetAllTiles();
    QPoint tileCoordinateShifted(QVector3D worldPos) const;
    void updateTilesHelp(int shifted, int topRightX, int topRightY, bool updateAxisY);

private:
    int m_rowCount = 0;
    qreal m_tileSize = 100;
    QVector3D m_centerPosition;

    QPoint m_shiftedTileCoord;
    QPoint m_prevShiftedTileCoord;
    Tile m_topRight;

    QVector<Tile> m_tilesToUpdate;
    QVector<QQuick3DNode *> m_delegateNodes;

    QQmlComponent *m_delegate;
};

#endif // TILEENGINE_H
