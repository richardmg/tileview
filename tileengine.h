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

class TileEngine : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    QML_ELEMENT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(int rowCount READ rowCount WRITE setRowCount NOTIFY rowCountChanged)
    Q_PROPERTY(qreal tileSize READ tileSize WRITE setTileSize NOTIFY tileSizeChanged)
    Q_PROPERTY(QVector3D targetPosition READ targetPosition WRITE setTargetPosition NOTIFY targetPositionChanged)
    Q_PROPERTY(QQuick3DNode *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged FINAL)

public:
    explicit TileEngine(QObject *parent = nullptr);
    ~TileEngine() override;

    Q_INVOKABLE QVector3D worldPosAtTileCoord(QPoint tileCoord) const;
    Q_INVOKABLE QPoint tileCoordAtWorldPos(QVector3D worldPos) const;

    int rowCount() const;
    void setRowCount(int rowCount);

    qreal tileSize() const;
    void setTileSize(qreal tileSize);

    QVector3D targetPosition() const;
    void setTargetPosition(QVector3D targetPosition);

    QQuick3DNode* delegate() const;
    void setDelegate(QQuick3DNode *delegate);

signals:
    void rowCountChanged();
    void tileSizeChanged();
    void targetPositionChanged();
    void delegateChanged();

public:
    virtual void updateTiles(const QVector<TileDescription> &tiles);
    virtual void updateNeighbours(const QVector<TileNeighbours> &neighbours);

protected:
    void classBegin() override {};
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

private:
    int m_rowCount = 0;
    int m_rowCountHalf;
    qreal m_tileSize = 100;

    QVector3D m_targetWorldPos;

    QPoint m_shiftedTileCoord;
    QPoint m_prevShiftedTileCoord;
    TileDescription m_topRight;

    QVector<TileDescription> m_tileMoveDesc;
    QVector3D m_targetPosition;

    bool m_componentComplete = false;
    QQuick3DNode * m_delegate;
};

#endif // TILEENGINE_H
