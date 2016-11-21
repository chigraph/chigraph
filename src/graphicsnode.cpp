/* See LICENSE file for copyright and license details. */

#include "graphicsnode.hpp"
#include <QtCore/QDebug>
#include <QPen>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsTextItem>
#include <QGraphicsDropShadowEffect>
#include <QtWidgets/QWidget>

#include <algorithm>

#include "graphicsnode_p.h"

#include "graphicsbezieredge.hpp"
#include "graphicsnodesocket.hpp"
#include "graphicsnodesocket_p.h"

#include "qnodeeditorsocketmodel.h"

#if QT_VERSION < 0x050700
//Q_FOREACH is deprecated and Qt CoW containers are detached on C++11 for loops
template<typename T>
const T& qAsConst(const T& v)
{
    return const_cast<const T&>(v);
}
#endif

class GraphicsNodePrivate final
{
public:
    explicit GraphicsNodePrivate(GraphicsNode* q) : q_ptr(q) {}

    QNodeEditorSocketModel* m_pModel;
    QPersistentModelIndex m_Index;

    // TODO: change pairs of sizes to QPointF, QSizeF, or quadrupels to QRectF

    constexpr static const qreal _hard_min_width  = 150.0;
    constexpr static const qreal _hard_min_height = 120.0;

    QSizeF m_MinSize {150.0, 120.0};

    constexpr static const qreal _top_margin    = 30.0;
    constexpr static const qreal _bottom_margin = 15.0;
    constexpr static const qreal _item_padding  = 5.0;
    constexpr static const qreal _lr_padding    = 10.0;

    constexpr static const qreal _pen_width = 1.0;
    constexpr static const qreal _socket_size = 6.0;

    NodeGraphicsItem* m_pGraphicsItem;

    bool _changed {false};

    QSizeF m_Size {150, 120};

    QPen _pen_default  {QColor("#7F000000")};
    QPen _pen_selected {QColor("#FFFF36A7")};
    QPen _pen_sources  {QColor("#FF000000")};
    QPen _pen_sinks    {QColor("#FF000000")};

    QBrush _brush_title      {QColor("#E3212121")};
    QBrush _brush_background {QColor("#E31a1a1a")};
    QBrush m_brushSources    {QColor("#FFFF7700")};
    QBrush m_brushSinks      {QColor("#FF0077FF")};

    //TODO lazy load, add option to disable, its nice, but sllooowwww
    QGraphicsDropShadowEffect *_effect        {new QGraphicsDropShadowEffect()};
    QGraphicsTextItem         *_title_item    {nullptr};
    QGraphicsProxyWidget      *_central_proxy {nullptr};

    // Helpers
    void updateGeometry();
    void updateSizeHints();

    GraphicsNode* q_ptr;
};

// Necessary for some compilers...
constexpr const qreal GraphicsNodePrivate::_hard_min_width;
constexpr const qreal GraphicsNodePrivate::_hard_min_height;


GraphicsNode::GraphicsNode(QNodeEditorSocketModel* model, const QPersistentModelIndex& index, QGraphicsItem *parent)
: QObject(nullptr), d_ptr(new GraphicsNodePrivate(this))
{
    d_ptr->m_pModel = model;
    d_ptr->m_Index = index;

    d_ptr->m_pGraphicsItem = new NodeGraphicsItem(parent);
    d_ptr->m_pGraphicsItem->d_ptr = d_ptr;
    d_ptr->m_pGraphicsItem->q_ptr = this;

    d_ptr->_title_item = new QGraphicsTextItem(d_ptr->m_pGraphicsItem);

    for (auto p : {
      &d_ptr->_pen_default, &d_ptr->_pen_selected,
      &d_ptr->_pen_default, &d_ptr->_pen_selected
    })
        p->setWidth(0);

    d_ptr->m_pGraphicsItem->setFlag(QGraphicsItem::ItemIsMovable);
    d_ptr->m_pGraphicsItem->setFlag(QGraphicsItem::ItemIsSelectable);
    d_ptr->m_pGraphicsItem->setFlag(QGraphicsItem::ItemSendsGeometryChanges);

    d_ptr->_title_item->setDefaultTextColor(Qt::white);
    d_ptr->_title_item->setPos(0, 0);
    d_ptr->_title_item->setTextWidth(d_ptr->m_Size.width() - 2*d_ptr->_lr_padding);

    d_ptr->_effect->setBlurRadius(13.0);
    d_ptr->_effect->setColor(QColor("#99121212"));

    d_ptr->m_pGraphicsItem->setGraphicsEffect(d_ptr->_effect);

}


void GraphicsNode::
setTitle(const QString &title)
{
    if (auto m = const_cast<QAbstractItemModel*>(d_ptr->m_Index.model()))
        m->setData(d_ptr->m_Index, title, Qt::DisplayRole);

    d_ptr->_title_item->setPlainText(d_ptr->m_Index.data().toString());
}

QString GraphicsNode::
title() const
{
    return d_ptr->m_Index.data().toString();
}

QAbstractItemModel *GraphicsNode::
sinkModel() const
{
    return d_ptr->m_pModel->sinkSocketModel(d_ptr->m_Index);
}

QAbstractItemModel *GraphicsNode::
sourceModel() const
{
    return d_ptr->m_pModel->sourceSocketModel(d_ptr->m_Index);
}

int NodeGraphicsItem::
type() const
{
    return GraphicsNodeItemTypes::TypeNode;
}

QSizeF GraphicsNode::
size() const
{
    return d_ptr->m_Size;
}

QGraphicsItem *GraphicsNode::
graphicsItem() const
{
    return d_ptr->m_pGraphicsItem;
}

GraphicsNode::
~GraphicsNode()
{
    if (d_ptr->_central_proxy) delete d_ptr->_central_proxy;
    delete d_ptr->_title_item;
    delete d_ptr->_effect;
    delete d_ptr;
}


QRectF NodeGraphicsItem::
boundingRect() const
{
    return QRectF(
        -d_ptr->_pen_width/2.0 - d_ptr->_socket_size,
        -d_ptr->_pen_width/2.0,
        d_ptr->m_Size.width()  + d_ptr->_pen_width/2.0 + 2.0 * d_ptr->_socket_size,
        d_ptr->m_Size.height() + d_ptr->_pen_width/2.0
    ).normalized();
}


void NodeGraphicsItem::
paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    const qreal edge_size = 10.0;
    const qreal title_height = 20.0;

    // path for the caption of this node
    QPainterPath path_title;
    path_title.setFillRule(Qt::WindingFill);
    path_title.addRoundedRect(QRect(0, 0, d_ptr->m_Size.width(), title_height), edge_size, edge_size);
    path_title.addRect(0, title_height - edge_size, edge_size, edge_size);
    path_title.addRect(d_ptr->m_Size.width() - edge_size, title_height - edge_size, edge_size, edge_size);
    painter->setPen(Qt::NoPen);
    painter->setBrush(d_ptr->_brush_title);
    painter->drawPath(path_title.simplified());

    // path for the content of this node
    QPainterPath path_content;
    path_content.setFillRule(Qt::WindingFill);
    path_content.addRoundedRect(QRect(0, title_height, d_ptr->m_Size.width(), d_ptr->m_Size.height() - title_height), edge_size, edge_size);
    path_content.addRect(0, title_height, edge_size, edge_size);
    path_content.addRect(d_ptr->m_Size.width() - edge_size, title_height, edge_size, edge_size);
    painter->setPen(Qt::NoPen);
    painter->setBrush(d_ptr->_brush_background);
    painter->drawPath(path_content.simplified());

    // path for the outline
    QPainterPath path_outline = QPainterPath();
    path_outline.addRoundedRect(QRect(0, 0, d_ptr->m_Size.width(), d_ptr->m_Size.height()), edge_size, edge_size);
    painter->setPen(isSelected() ? d_ptr->_pen_selected : d_ptr->_pen_default);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path_outline.simplified());

    // debug bounding box
#if 0
    QPen debugPen = QPen(QColor(Qt::red));
    debugPen.setWidth(0);
    auto r = boundingRect();
    painter->setPen(debugPen);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(r);

    painter->drawPoint(0,0);
#endif
}


void NodeGraphicsItem::
mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // TODO: ordering after selection/deselection cycle
    QGraphicsItem::mousePressEvent(event);

    setZValue(isSelected() ? 1 : 0);
}


void GraphicsNode::
setSize(const qreal width, const qreal height)
{
    setSize(QPointF(width, height));
}


void GraphicsNode::
setSize(const QPointF size)
{
    setSize(QSizeF(size.x(), size.y()));
}


void GraphicsNode::
setSize(const QSizeF size)
{
    d_ptr->m_Size = size;
    d_ptr->_changed = true;
    d_ptr->m_pGraphicsItem->prepareGeometryChange();
    d_ptr->updateGeometry();
}


QVariant NodeGraphicsItem::
itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case QGraphicsItem::ItemSelectedChange:
        setZValue(value.toBool() ? 1 : 0);
        break;
    case QGraphicsItem::ItemPositionChange:
    case QGraphicsItem::ItemPositionHasChanged: {
        auto m = const_cast<QAbstractItemModel*>(d_ptr->m_Index.model());

        m->setData(d_ptr->m_Index, QRectF(pos(), d_ptr->m_Size), Qt::SizeHintRole);
    }
        break;

    default:
        break;
    }

    return QGraphicsItem::itemChange(change, value);
}

void GraphicsNode::update()
{
    d_ptr->_changed = true;
    d_ptr->m_pGraphicsItem->prepareGeometryChange();
    d_ptr->updateGeometry();
}

QModelIndex GraphicsNode::index() const
{
    return d_ptr->m_Index;
}

QAbstractItemModel* GraphicsNode::model() const
{
    return d_ptr->m_pModel;
}

const QModelIndex GraphicsNode::socketIndex(const QString& name) const
{
    const auto c = model()->rowCount(index());
    for (int i = 0; i < c; i++) {
        auto idx = model()->index(i, 0, index());
        if (idx.data() == name)
            return idx;
    }

    return {};
}

void GraphicsNodePrivate::
updateGeometry()
{
    if (!_changed) return;

    // compute if we have reached the minimum size
    updateSizeHints();

    m_Size = {
        std::max(m_MinSize.width() , m_Size.width()  ),
        std::max(m_MinSize.height(), m_Size.height() )
    };

    // title
    _title_item->setTextWidth(m_Size.width());

    qreal yposSink = _top_margin;
    qreal yposSrc  = m_Size.height() - _bottom_margin;

    const int count = m_pModel->rowCount(m_Index);

    for (int i = 0; i < count; i++) {
        const auto idx = m_pModel->index(i, 0, m_Index);

        // sinks
        if (const auto s = m_pModel->getSinkSocket(idx)) {
            const auto size = s->size();

            s->graphicsItem()->setPos(0, yposSink + size.height()/2.0);
            s->d_ptr->update();
            yposSink += size.height() + _item_padding;

            s->graphicsItem()->setOpacity(s->index().flags() & Qt::ItemIsEnabled ?
                1.0 : 0.1
            );
        }

        // sources
        if (const auto s = m_pModel->getSourceSocket(idx)) {
            const auto size = s->size();

            yposSrc -= size.height();
            s->graphicsItem()->setPos(m_Size.width(), yposSrc + size.height()/2.0);
            s->d_ptr->update();
            yposSrc -= _item_padding;

            s->graphicsItem()->setOpacity(s->index().flags() & Qt::ItemIsEnabled ?
                1.0 : 0.1
            );
        }
    }

    // central widget
    if (_central_proxy) {
        _central_proxy->setGeometry({
            _lr_padding,
            yposSink,
            m_Size.width() - 2.0 * _lr_padding,
            yposSrc - yposSink
        });
    }

    _changed = false;
}

void GraphicsNode::
setCentralWidget (QWidget *widget)
{
    if (d_ptr->_central_proxy)
        delete d_ptr->_central_proxy;

    d_ptr->_central_proxy = new QGraphicsProxyWidget(d_ptr->m_pGraphicsItem);
    d_ptr->_central_proxy->setWidget(widget);
    d_ptr->_changed = true;
    d_ptr->m_pGraphicsItem->prepareGeometryChange();
    d_ptr->updateGeometry();
}


void GraphicsNodePrivate::
updateSizeHints() {
    qreal min_width(0.0), min_height(_top_margin + _bottom_margin);

    const int count = m_pModel->rowCount(m_Index);

    // sinks
    for (int i = 0; i < count; i++) {
        if (const auto s = m_pModel->getSinkSocket(m_pModel->index(i, 0, m_Index))) {
            auto size = s->minimalSize();

            min_height += size.height() + _item_padding;
            min_width   = std::max(size.width(), min_width);
        }
    }

    // central widget
    if (_central_proxy) {
        if (const auto wgt = _central_proxy->widget()) {
            // only take the size hint if the value is valid, and if
            // the minimumSize is not set (similar to
            // QWidget/QLayout standard behavior
            const auto sh = wgt->minimumSizeHint();
            const auto sz = wgt->minimumSize();

            if (sh.isValid()) {
                min_height += (sz.height() > 0) ? sz.height() : sh.height();

                min_width = std::max(
                    qreal((sz.width() > 0) ? sz.width() : sh.width())
                        + 2.0*_lr_padding,
                    min_width
                );

            } else {
                min_height += sh.height();
                min_width   = std::max(
                    qreal(sh.width()) + 2.0*_lr_padding,
                    min_width
                );
            }
        }
    }

    // sources
    for (int i = 0; i < count; i++) {
        if (const auto s = m_pModel->getSourceSocket(m_pModel->index(i, 0, m_Index))) {
            const auto size = s->minimalSize();

            min_height += size.height() + _item_padding;
            min_width   = std::max(size.width(), min_width);
        }
    }

    m_MinSize = {
        std::max(min_width, _hard_min_width  ),
        std::max(min_height, _hard_min_height)
    };
}
