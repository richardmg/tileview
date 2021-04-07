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

struct TileDescription
{
    QVector3D worldPos;
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

    Q_INVOKABLE QVector3D worldPosAtTileCoord(QPoint tileCoord) const;
    Q_INVOKABLE QPoint tileCoordAtWorldPos(QVector3D worldPos) const;

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
    virtual void updateDelegateNodes(const QVector<TileDescription> &tiles);
    virtual void updateNeighbours(const QVector<TileNeighbours> &neighbours);

protected:
    void componentComplete() override;

private:
    int matrixPos(int startEdge, int edgeShifted) const;
    QPoint matrixCoordForWorldPos(QVector3D worldPos) const;
    QPoint tileCoordForMatrixCoord(QPoint matrixCoord) const;

//    void setNeighbours(QPoint pos, TileNeighbours &result);
    void updateAllTiles();
    QPoint tileCoordAtWorldPosShifted(QVector3D worldPos) const;
    void updateTilesHelp(int shifted, int topRightX, int topRightY, bool updateAxisY);

    void createDelegates();
    void delegateDelegates();

private:
    int m_rowCount = 0;
    qreal m_tileSize = 100;
    QVector3D m_centerPosition;

    QPoint m_shiftedTileCoord;
    QPoint m_prevShiftedTileCoord;
    TileDescription m_topRight;

    QVector<TileDescription> m_tileMoveDesc;
    QVector<QQuick3DNode *> m_delegateNodes;

    QQmlComponent *m_delegate;
};

#endif // TILEENGINE_H
