/* See LICENSE file for copyright and license details. */

#include "graphicsbezieredge.hpp"
#include <algorithm>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPathItem>
#include <QMetaProperty>

#include <QtCore/QDebug>

#include "graphicsnode.hpp"
#include "graphicsnodesocket.hpp"

#include "graphicsnodesocket_p.h"
#include "graphicsbezieredge_p.h"

#include "qreactiveproxymodel.h"

#include "qnodeeditorsocketmodel.h"

class GraphicsEdgeItem : public QGraphicsPathItem
{
public:
    explicit GraphicsEdgeItem(GraphicsDirectedEdgePrivate* s) : d_ptr(s) {}

    virtual int type() const override;

    virtual void updatePath() = 0;

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    GraphicsDirectedEdgePrivate* d_ptr;
};

class GraphicsBezierItem final : public GraphicsEdgeItem
{
public:
    explicit GraphicsBezierItem(GraphicsDirectedEdgePrivate* s) :
        GraphicsEdgeItem(s) {}

    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0) override;
    virtual int type() const override;
    virtual void updatePath() override;
};

GraphicsDirectedEdge::
GraphicsDirectedEdge(QNodeEditorEdgeModel* m, const QModelIndex& index, qreal factor)
: QObject(), d_ptr(new GraphicsDirectedEdgePrivate(this))
{
    Q_ASSERT(index.isValid());

    d_ptr->m_pModel = m;
    d_ptr->m_Index  = index;
    d_ptr->_factor  = factor;
    d_ptr->m_pGrpahicsItem = new GraphicsBezierItem(d_ptr);

    d_ptr->_pen.setWidth(2);
    d_ptr->m_pGrpahicsItem->setZValue(-1);

    d_ptr->_effect->setBlurRadius(15.0);
    d_ptr->_effect->setColor(QColor("#99050505"));
    d_ptr->m_pGrpahicsItem->setGraphicsEffect(d_ptr->_effect);
}

void GraphicsDirectedEdge::update()
{
    d_ptr->m_pGrpahicsItem->updatePath();
}

int GraphicsBezierItem::
type() const
{
    return GraphicsNodeItemTypes::TypeBezierEdge;
}

QGraphicsItem *GraphicsDirectedEdge::
graphicsItem() const
{
    Q_ASSERT(d_ptr->m_pGrpahicsItem);
    return d_ptr->m_pGrpahicsItem;
}


GraphicsDirectedEdge::
~GraphicsDirectedEdge()
{
    delete d_ptr->_effect;
    delete d_ptr->m_pGrpahicsItem;
    delete d_ptr;
}


void GraphicsEdgeItem::
mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //FIXME currently dead code, need to be implemented
    QGraphicsPathItem::mousePressEvent(event);
}

void GraphicsDirectedEdgePrivate::
setStart(QPointF p)
{
    _start = p;
    m_pGrpahicsItem->updatePath();
}


void GraphicsDirectedEdgePrivate::
setStop(QPointF p)
{
    _stop = p;
    m_pGrpahicsItem->updatePath();
}

GraphicsBezierEdge::GraphicsBezierEdge(QNodeEditorEdgeModel* m, const QModelIndex& index, qreal factor)
    : GraphicsDirectedEdge(m, index, factor)
{
    d_ptr->m_pGrpahicsItem = new GraphicsBezierItem(d_ptr);
}

void GraphicsBezierItem::
updatePath()
{
    Q_ASSERT(d_ptr->m_Index.isValid());

    auto srcI  = d_ptr->m_pModel->index(d_ptr->m_Index.row(), 0)
        .data(Qt::SizeHintRole);
    auto sinkI = d_ptr->m_pModel->index(d_ptr->m_Index.row(), 2)
        .data(Qt::SizeHintRole);

    // compute anchor point offsets
    const qreal min_dist = 0.f; //FIXME this is dead code? can the code below ever get negative?

    QPointF c1 = srcI.canConvert<QPointF>() ? srcI.toPointF() : d_ptr->_start;

    QPointF c2 = sinkI.canConvert<QPointF>() ? sinkI.toPointF() : d_ptr->_stop;

    const qreal dist = (c1.x() <= c2.x()) ?
        std::max(min_dist, (c2.x() - c1.x()) * d_ptr->_factor):
        std::max(min_dist, (c1.x() - c2.x()) * d_ptr->_factor);

    QPainterPath path(c1);

    const auto c3 = c2;

    c1.rx() += dist;
    c2.rx() -= dist;

    path.cubicTo(c1, c2, c3);
    setPath(path);

    //FIXME technically, all edges currently are at {0,0}, they should be at c1
    // This makes many "objects under cursor" call slow
}

int GraphicsEdgeItem::
type() const
{
    return GraphicsNodeItemTypes::TypeBezierEdge;
}

void GraphicsBezierItem::
paint(QPainter * painter, const QStyleOptionGraphicsItem * opt, QWidget *w)
{
    Q_UNUSED(opt)
    Q_UNUSED(w)

    const auto fg = d_ptr->m_Index.data(Qt::ForegroundRole);

    // Set the line color
    if (fg.canConvert<QBrush>())
        painter->setPen(QPen(qvariant_cast<QBrush>(fg),d_ptr->_pen.width()));
    else if (fg.canConvert<QPen>())
        painter->setPen(qvariant_cast<QPen>(fg));
    else
        painter->setPen(d_ptr->_pen);


    painter->drawPath(path());
}

QModelIndex GraphicsDirectedEdge::index() const
{
    return d_ptr->m_Index;
}

void GraphicsDirectedEdge::setSink(const QModelIndex& idx)
{
    const auto i = d_ptr->m_pModel->index(d_ptr->m_Index.row(), 2);
    d_ptr->m_pModel->setData(i, idx, QReactiveProxyModel::ConnectionsRoles::SOURCE_INDEX);
}

void GraphicsDirectedEdge::setSource(const QModelIndex& idx)
{
    const auto i = d_ptr->m_pModel->index(d_ptr->m_Index.row(), 0);
    d_ptr->m_pModel->setData(i, idx, QReactiveProxyModel::ConnectionsRoles::DESTINATION_INDEX);
}
