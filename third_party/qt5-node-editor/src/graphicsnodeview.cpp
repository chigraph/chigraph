/* See LICENSE file for copyright and license details. */

#include "graphicsnodeview.hpp"
#include <QWheelEvent>
#include <QScrollBar>
#include <QResizeEvent>
#include <QMimeData>
#include <QGraphicsDropShadowEffect>
#include <QResizeEvent>
#include <QGraphicsItem>

#include "graphicsnode.hpp"
#include "graphicsnodesocket.hpp"
#include "graphicsnodedefs.hpp"
#include "graphicsbezieredge.hpp"

#include "graphicsbezieredge_p.h"
#include "graphicsnodesocket_p.h"
#include "graphicsnode_p.h"

#include "qnodeeditorsocketmodel.h"

GraphicsNodeView::GraphicsNodeView(QWidget *parent)
: GraphicsNodeView(nullptr, parent)
{ }


GraphicsNodeView::GraphicsNodeView(QGraphicsScene *scene, QWidget *parent)
: QGraphicsView(scene, parent)
{
	setRenderHints(QPainter::Antialiasing |
			QPainter::TextAntialiasing |
			QPainter::HighQualityAntialiasing |
			QPainter::SmoothPixmapTransform);

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setResizeAnchor(NoAnchor);
	setTransformationAnchor(AnchorUnderMouse);
	// setDragMode(QGraphicsView::RubberBandDrag);
}


void GraphicsNodeView::
resizeEvent(QResizeEvent *event)
{
	// always have the origin in the top left corner
	static bool first_resize = true;
	if (first_resize) {
		// TODO: scale awareness
		centerOn(width()/2 - 50, height()/2 - 50);
		first_resize = false;
	}
	QGraphicsView::resizeEvent(event);
}



void GraphicsNodeView::
wheelEvent(QWheelEvent *event) {
	if (event->modifiers() & Qt::ControlModifier) {
		setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
		double scaleFactor = 1.25;
		if (event->delta() < 0) {
			// zoom in
			scale(scaleFactor, scaleFactor);
		}
		else {
			// zoom out
			scale(1.0 / scaleFactor, 1.0 / scaleFactor);
		}
		event->accept();
	}
	else {
		QGraphicsView::wheelEvent(event);
	}
}


void GraphicsNodeView::
mousePressEvent(QMouseEvent *event)
{
	switch (event->button()) {
	case Qt::MiddleButton:
		middleMouseButtonPress(event);
		break;
	case Qt::LeftButton:
		leftMouseButtonPress(event);
		break;
	default:
		QGraphicsView::mousePressEvent(event);
	}
}


void GraphicsNodeView::
middleMouseButtonRelease(QMouseEvent *event)
{
	QMouseEvent fakeEvent(event->type(), event->localPos(), event->screenPos(), event->windowPos(),
			Qt::LeftButton, event->buttons() & ~Qt::LeftButton, event->modifiers());
	QGraphicsView::mouseReleaseEvent(&fakeEvent);
	// setDragMode(QGraphicsView::RubberBandDrag);
	setDragMode(QGraphicsView::NoDrag);
}

#include <QDebug>
void GraphicsNodeView::
leftMouseButtonRelease(QMouseEvent *event)
{
	if (_drag_event) {
		auto sock = socket_at(event->pos());


		if (!sock || !can_accept_edge(sock)) {
			_drag_event->e->setSource({});
			_drag_event->e->setSink({});
		} else {
			switch (_drag_event->mode) {
			case EdgeDragEvent::to_source:
				_drag_event->e->setSource(sock->index());
				break;
			case EdgeDragEvent::to_sink:
				_drag_event->e->setSink(sock->index());
				break;
			}

		}
		_drag_event->mimeData->deleteLater();
		delete _drag_event;
		_drag_event = nullptr;
	} else
	if (_resize_event) {
		// TODO: finally set the width/height of the node ?
		delete _resize_event;
		_resize_event = nullptr;
		setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
	}
	QGraphicsView::mouseReleaseEvent(event);
}


void GraphicsNodeView::
mouseReleaseEvent(QMouseEvent *event)
{
	viewport()->setCursor(Qt::ArrowCursor);

	switch (event->button()) {
	case Qt::MiddleButton:
		middleMouseButtonRelease(event);
		break;
	case Qt::LeftButton:
		leftMouseButtonRelease(event);
		break;
	default:
		QGraphicsView::mouseReleaseEvent(event);
	}
}


void GraphicsNodeView::
mouseMoveEvent(QMouseEvent *event)
{
	// if the left mouse button was pressed and we actually have a mode and
	// temporary edge already set
	if (_drag_event && (event->buttons() & Qt::LeftButton)) {

		// set start/stop (ignored if the sockets are set)
		_drag_event->e->d_ptr->setStart(mapToScene(event->pos()));
		_drag_event->e->d_ptr->setStop (mapToScene(event->pos()));

		// update visual feedback
		auto sock = socket_at(event->pos());
		if (!sock) {
			viewport()->setCursor(Qt::ClosedHandCursor);
		}
		else {
			if (!can_accept_edge(sock))
				viewport()->setCursor(Qt::ForbiddenCursor);
			else {
				viewport()->setCursor(Qt::DragMoveCursor);
			}
		}
	}
	else if (_resize_event && (event->buttons() & Qt::LeftButton)) {
		QPointF size = mapToScene(event->pos())
            - _resize_event->node->graphicsItem()->mapToScene(0,0);
		_resize_event->node->setSize(size);
	}
	else {
		// no button is pressed, so indicate what the user can do with
		// the item by changing the cursor
		if (event->buttons() == 0) {
			auto sock = socket_at(event->pos());
			if (sock) {
				QPointF scenePos = mapToScene(event->pos());
				QPointF itemPos = sock->graphicsItem()->mapFromScene(scenePos);
				if (sock->d_ptr->isInSocketCircle(itemPos))
					viewport()->setCursor(Qt::OpenHandCursor);
				else
					viewport()->setCursor(Qt::ArrowCursor);
			}
			else
				viewport()->setCursor(Qt::ArrowCursor);
		}
		QGraphicsView::mouseMoveEvent(event);
	}
}


void GraphicsNodeView::
leftMouseButtonPress(QMouseEvent *event)
{
	QGraphicsView::mousePressEvent(event);
	// GUI logic: if we click on a socket, we need to handle
	// the event appropriately
	QGraphicsItem *item = itemAt(event->pos());
	if (!item) {
		QGraphicsView::mousePressEvent(event);
		return;
	}

	switch (item->type())
	{
	case GraphicsNodeItemTypes::TypeSocket: {
		const QPointF scenePos = mapToScene(event->pos());
		const QPointF itemPos  = item->mapFromScene(scenePos);

		GraphicsNodeSocket *sock = static_cast<SocketGraphicsItem*>(item)->d_ptr->q_ptr;

		if (sock->d_ptr->isInSocketCircle(itemPos)) {
			viewport()->setCursor(Qt::ClosedHandCursor);

			// initialize a new drag mode event
			_drag_event = new EdgeDragEvent();
			_drag_event->mimeData = m_pModel->mimeData({sock->index()});

			switch (sock->socketType()) {
			case GraphicsNodeSocket::SocketType::SINK:
				if (_drag_event->e = m_pModel->getSinkEdge(sock->edge()))
					_drag_event->mode = EdgeDragEvent::to_sink;
				else {
					_drag_event->e = m_pModel->initiateConnectionFromSink(
						sock->index(), scenePos
					);
					_drag_event->mode = EdgeDragEvent::to_source;
				}
				_drag_event->e->setSink(sock->index());
				break;
			case GraphicsNodeSocket::SocketType::SOURCE:
				if (_drag_event->e = m_pModel->getSinkEdge(sock->edge()))
					_drag_event->mode = EdgeDragEvent::to_source;
				else {
					_drag_event->e = m_pModel->initiateConnectionFromSource(
						sock->index(), scenePos
					);
					_drag_event->mode = EdgeDragEvent::to_sink;
				}
				_drag_event->e->setSource(sock->index());
				break;
			}

			// set the fallback positions, they are ignored is the socket is set
			_drag_event->e->d_ptr->setStop (scenePos);
			_drag_event->e->d_ptr->setStart(scenePos);

			event->ignore();
		}
		else {
			QGraphicsView::mousePressEvent(event);
		}
		break;
	}

	case GraphicsNodeItemTypes::TypeNode: {
		QPointF scenePos = mapToScene(event->pos());
		QPointF itemPos = item->mapFromScene(scenePos);
		GraphicsNode *node = static_cast<NodeGraphicsItem*>(item)->q_ptr;

		if (itemPos.x() > (node->size().width() - 10) && (itemPos.y() > (node->size().height() - 10))) {
			setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
			_resize_event = new NodeResizeEvent();
			_resize_event->node = node;
			_resize_event->orig_width = node->size().width();
			_resize_event->orig_height = node->size().height();
			_resize_event->pos = event->pos();
			event->ignore();
		}
		else {
			QGraphicsView::mousePressEvent(event);
		}
		break;
	}

	default:
		QGraphicsView::mousePressEvent(event);
	}
}


void GraphicsNodeView::
middleMouseButtonPress(QMouseEvent *event)
{
	QMouseEvent releaseEvent(QEvent::MouseButtonRelease,
			event->localPos(), event->screenPos(), event->windowPos(),
			Qt::LeftButton, 0, event->modifiers());
	QGraphicsView::mouseReleaseEvent(&releaseEvent);
	setDragMode(QGraphicsView::ScrollHandDrag);
	QMouseEvent fakeEvent(event->type(), event->localPos(), event->screenPos(), event->windowPos(),
			Qt::LeftButton,
			event->buttons() | Qt::LeftButton,
			event->modifiers());
	QGraphicsView::mousePressEvent(&fakeEvent);
}


bool GraphicsNodeView::
can_accept_edge(GraphicsNodeSocket *sock)
{
    return sock && sock->isEnabled() && _drag_event &&(
         (sock->isSink  () && _drag_event->mode == EdgeDragEvent::to_sink  )
      || (sock->isSource() && _drag_event->mode == EdgeDragEvent::to_source)
    );
}


GraphicsNodeSocket* GraphicsNodeView::
socket_at(QPoint pos)
{
	// figure out if we are above another socket. this requires
	// stepping through all items that we can actually see, and
	// taking the topmost one

	QList<QGraphicsItem*> itms = items(pos);
	QGraphicsItem *item = nullptr;
	for (int i = itms.count() - 1; i >= 0; i--) {
		if (itms[i]->type() == GraphicsNodeItemTypes::TypeSocket) {
			item = itms[i];
			break;
		}
	}

	if (item && item->type() == GraphicsNodeItemTypes::TypeSocket)
		return static_cast<SocketGraphicsItem*>(item)->d_ptr->q_ptr;
	else {
//         qDebug() << "\n\nNOT FOUND";
		return nullptr;
    }
}
