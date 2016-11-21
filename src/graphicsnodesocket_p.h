#ifndef GRAPHICSNODESOCKET_P_H
#define GRAPHICSNODESOCKET_P_H

#include "graphicsnodesocket.hpp"
#include <QtWidgets/QGraphicsItem>

#include <QtCore/QPersistentModelIndex>

#define PEN_COLOR_CIRCLE      QColor("#FF000000")
#define PEN_COLOR_TEXT        QColor("#FFFFFFFF")

class SocketGraphicsItem;

class GraphicsNodeSocketPrivate
{
public:
    explicit GraphicsNodeSocketPrivate(GraphicsNodeSocket* q) : q_ptr(q) {}

    // return the anchor position relative to the scene in which the socket
    // is living in
    QPointF sceneAnchorPos() const; //TODO move to the private class

    /**
    * determine if a point is actually within the socket circle.
    */
    bool isInSocketCircle(const QPointF &p) const; //TODO move to the private class

    void update();

    // Attributes
    GraphicsNodeSocket::SocketType _socket_type;
    QPen _pen_circle {PEN_COLOR_CIRCLE};
    const QPen _pen_text {PEN_COLOR_TEXT};
    QBrush _brush_circle;
    GraphicsNode *_node;

    QPersistentModelIndex m_PersistentIndex;
    QPersistentModelIndex m_EdgeIndex;

    const qreal _pen_width = 1.0;
    const qreal _circle_radius = 6.0;
    const qreal _text_offset = 3.0;

    const qreal _min_width = 30;
    const qreal _min_height = 12.0;

    SocketGraphicsItem* m_pGraphicsItem;

    // Helper
    void drawAlignedText(QPainter *painter);

    GraphicsNodeSocket* q_ptr;
};

class SocketGraphicsItem final : public QGraphicsItem
{
public:
    SocketGraphicsItem(QGraphicsItem* parent, GraphicsNodeSocketPrivate* d) :
        QGraphicsItem(parent), d_ptr(d) {}

    virtual QRectF boundingRect() const override;

    /*
    */
    virtual void paint(QPainter *painter,
            const QStyleOptionGraphicsItem *option,
            QWidget *widget = 0) override;

    /**
    * type of the class. usefull within a QGraphicsScene to distinguish
    * what is really behind a pointer
    */
    virtual int type() const override;

    GraphicsNodeSocketPrivate* d_ptr;

protected:
    // event handling
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

};

#endif
