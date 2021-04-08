#ifndef TILEVIEW_H
#define TILEVIEW_H

#include <QtCore/QtCore>
#include <QtQml/QtQml>
#include <QtQuick3D/QtQuick3D>
#include <QtQuick3D/private/qquick3dnode_p.h>

struct Tile
{
    QVector3D position;
    QVector3D tileCoord;
    QVector3D matrixCoord;
};

class TileViewAttached;

class TileView : public QQuick3DNode
{
    Q_OBJECT
    QML_ELEMENT
    QML_ATTACHED(TileViewAttached)
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(QVector3D tileCount READ tileCount WRITE setTileCount NOTIFY tileCountChanged)
    Q_PROPERTY(QVector3D tileSize READ tileSize WRITE setTileSize NOTIFY tileSizeChanged)
    Q_PROPERTY(QVector3D center READ center WRITE setCenter NOTIFY centerChanged)
    Q_PROPERTY(QVector3D direction READ direction WRITE setDirection NOTIFY directionChanged)
    Q_PROPERTY(QQmlComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)

public:
    explicit TileView(QQuick3DNode *parent = nullptr);
    ~TileView() override;

    QVector3D tileCount() const;
    void setTileCount(const QVector3D &tileCount);

    QVector3D tileSize() const;
    void setTileSize(QVector3D tileSize);

    QVector3D center() const;
    void setCenter(const QVector3D &center);

    QVector3D direction() const;
    void setDirection(QVector3D direction);

    QQmlComponent* delegate() const;
    void setDelegate(QQmlComponent *delegate);

    static TileViewAttached *qmlAttachedProperties(QObject *obj);

signals:
    void tileCountChanged();
    void tileSizeChanged();
    void centerChanged();
    void delegateChanged();
    void directionChanged();

public:
    virtual void recreateDelegates();
    virtual void updateDelegate(const Tile &tile);

protected:
    void componentComplete() override;

private:
    QVector3D mapTileCoordToPosition(QVector3D tileCoord) const;
    QVector3D mapPositionToTileCoord(QVector3D position) const;
    QVector3D mapPositionToMatrixCoord(QVector3D position) const;
    QVector3D mapMatrixCoordToTileCoord(QVector3D matrixCoord) const;
    QVector3D mapPositionToTileCoordShifted(QVector3D position) const;

    void resetAllTiles();

    int matrixCoordShiftedX(int startCoord, int shiftCount) const;
    int matrixCoordShiftedY(int startCoord, int shiftCount) const;
    int matrixCoordShiftedZ(int startCoord, int shiftCount) const;

    void updateTiles();

    TileViewAttached *getAttachedObject(const QObject *obj) const;

private:
    QVector3D m_tileCount;
    QVector3D m_tileSize;
    QVector3D m_centerPosition;
    QVector3D m_direction;

    QPoint m_shiftedTileCoord;
    QPoint m_prevShiftedTileCoord;
    Tile m_cornerTile;

    QVector<QQuick3DNode *> m_delegateNodes;

    QQmlComponent *m_delegate = nullptr;
};

class TileViewAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(TileView *view READ view NOTIFY viewChanged)
    Q_PROPERTY(QVector3D tile READ tile NOTIFY tileChanged)

public:
    TileViewAttached(QObject *parent);

    TileView *view() const;
    void setView(TileView *tileView);

    QVector3D tile() const;
    void setTile(const QVector3D &tile);

signals:
    void viewChanged();
    void tileChanged();

private:
    QPointer<TileView> m_view;
    QVector3D m_tile;
};

#endif // TILEVIEW_H
