#ifndef GRAPHICS_NODE_P_H
#define GRAPHICS_NODE_P_H

#include <QtWidgets/QGraphicsItem>

class NodeGraphicsItem : public QGraphicsItem
{
    friend class GraphicsNode; // to access the protected methods
public:
    NodeGraphicsItem(QGraphicsItem* parent) : QGraphicsItem(parent) {}

    virtual int type() const override;

    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter,
            const QStyleOptionGraphicsItem *option,
            QWidget *widget = 0) override;

    GraphicsNodePrivate* d_ptr;
    GraphicsNode* q_ptr;

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};

#endif
