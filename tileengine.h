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

    Q_PROPERTY(int tileCount READ tileCount WRITE setTileCount NOTIFY tileCountChanged)
    Q_PROPERTY(qreal tileSize READ tileSize WRITE setTileSize NOTIFY tileSizeChanged)
    Q_PROPERTY(QVector3D center READ center WRITE setCenter NOTIFY centerChanged)
    Q_PROPERTY(QQmlComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)

public:
    explicit TileView(QQuick3DNode *parent = nullptr);
    ~TileView() override;

    int tileCount() const;
    void setTileCount(int tileCount);

    qreal tileSize() const;
    void setTileSize(qreal tileSize);

    QVector3D center() const;
    void setCenter(QVector3D center);

    QQmlComponent* delegate() const;
    void setDelegate(QQmlComponent *delegate);

signals:
    void tileCountChanged();
    void tileSizeChanged();
    void centerChanged();
    void delegateChanged();

public:
    virtual void recreateDelegates();
    virtual void updateDelegates(const QVector<Tile> &tiles);
    virtual void updateNeighbours(const QVector<TileNeighbours> &neighbours);

protected:
    void componentComplete() override;

private:
    QVector3D mapTileCoordToPosition(QPoint tileCoord) const;
    QPoint mapPositionToTileCoord(QVector3D position) const;
    int matrixCoordShifted(int startCoord, int shiftCount) const;
    QPoint mapPositionToMatrixCoord(QVector3D position) const;
    QPoint mapMatrixCoordToTileCoord(QPoint matrixCoord) const;

//    void setNeighbours(QPoint pos, TileNeighbours &result);
    void resetAllTiles();
    QPoint mapPositionToTileCoordShifted(QVector3D position) const;
    void updateTiles(int shifted, int cornerX, int cornerY, bool updateAxisY);

private:
    int m_tileCount = 0;
    qreal m_tileSize = 100;
    QVector3D m_centerPosition;

    QPoint m_shiftedTileCoord;
    QPoint m_prevShiftedTileCoord;
    Tile m_cornerTile;

    QVector<Tile> m_tilesToUpdate;
    QVector<QQuick3DNode *> m_delegateNodes;

    QQmlComponent *m_delegate;
};

#endif // TILEENGINE_H
