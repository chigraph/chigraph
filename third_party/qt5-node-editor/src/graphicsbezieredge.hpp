/* See LICENSE file for copyright and license details. */

#ifndef GRAPHICS_DIRECTED_EDGE_H
#define GRAPHICS_DIRECTED_EDGE_H

#include <QtCore/QPoint>
#include "graphicsnodedefs.hpp"

class GraphicsNodeSocket;

class GraphicsDirectedEdgePrivate;

class QNodeEditorEdgeModel;

class GraphicsDirectedEdge : public QObject
{
    //TODO once the point is stored in the index those 3 can go away
    friend class GraphicsNodeView; //To allow intermediate positions
    friend class GraphicsNodeSocketPrivate; //To allow intermediate positions

    friend class QNodeEditorSocketModelPrivate; // to notify changes

    Q_OBJECT
public:

    virtual ~GraphicsDirectedEdge();

    void update();

    QGraphicsItem *graphicsItem() const;

    void setSink(const QModelIndex& idx);
    void setSource(const QModelIndex& idx);

    QModelIndex index() const;

protected:
    // It cannot be constructed by itself or the user
    explicit GraphicsDirectedEdge(QNodeEditorEdgeModel* m, const QModelIndex& index, qreal factor=0.5f);

    GraphicsDirectedEdgePrivate* d_ptr;
    Q_DECLARE_PRIVATE(GraphicsDirectedEdge)
};

class GraphicsBezierEdge : public GraphicsDirectedEdge
{
    friend class EdgeWrapper; // to create
public:

protected:
    explicit GraphicsBezierEdge(QNodeEditorEdgeModel* m, const QModelIndex& index, qreal factor=0.5f);
};

#endif /* GRAPHICS_DIRECTED_EDGE_H */

