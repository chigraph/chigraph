#ifndef GRAPHICS_DIRECTED_EDGE_PRIVATE_H
#define GRAPHICS_DIRECTED_EDGE_PRIVATE_H

#include <QGraphicsDropShadowEffect>
#include <QtCore/QPersistentModelIndex>

class GraphicsEdgeItem;
class QNodeEditorEdgeModel;

class GraphicsDirectedEdgePrivate final
{
public:
    explicit GraphicsDirectedEdgePrivate(GraphicsDirectedEdge* q) : q_ptr(q) {}

    //TODO support effect lazy loading
    QGraphicsDropShadowEffect *_effect {new QGraphicsDropShadowEffect()};

    QPen _pen {QColor("#00FF00")};
    QPointF _start;
    QPointF _stop;
    qreal  _factor;

    GraphicsEdgeItem *m_pGrpahicsItem {nullptr};

    GraphicsDirectedEdge* q_ptr;

    QNodeEditorEdgeModel* m_pModel;

    QPersistentModelIndex m_Index;

    // Helpers

    void setStart(QPointF p);
    void setStop(QPointF p);
};

#endif
