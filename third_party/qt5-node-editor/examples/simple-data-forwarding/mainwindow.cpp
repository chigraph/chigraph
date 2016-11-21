/* See LICENSE file for copyright and license details. */

#include "mainwindow.hpp"
#include <QPen>
#include <QColor>
#include <QFont>
#include <QBrush>
#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QColorDialog>
#include <QPushButton>
#include <QTextEdit>
#include <QStatusBar>
#include <QResizeEvent>
#include <QButtonGroup>
#include <QString>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QLineEdit>

#include <iostream>

#include <graphicsnodescene.hpp>
#include <graphicsnodeview.hpp>
#include <graphicsnodeview.hpp>
#include <graphicsnode.hpp>
#include <graphicsbezieredge.hpp>
#include <qobjectnode.hpp>

#include "test_qobjects.hpp"


MainWindow::MainWindow()
: _view(nullptr)
, _scene(nullptr)
{
	_mnuFile = new QMenu(tr("File"));
	menuBar()->addMenu(_mnuFile);

	_actClear = new QAction(tr("Clear"), this);
	connect(_actClear, &QAction::triggered, this, &MainWindow::onClearTriggered);
	_mnuFile->addAction(_actClear);

	// create and configure scene
	_scene = new GraphicsNodeScene(this);
	_scene->setSceneRect(-32000, -32000, 64000, 64000);

	//  view setup
	_view = new GraphicsNodeView(this);
	_view->setScene(_scene);
	this->setCentralWidget(_view);

	// add some content
	//addFakeContent();
	addNodeViews();
}


void MainWindow::
onClearTriggered()
{
	_scene->clear();
}


void MainWindow::
resizeEvent(QResizeEvent *event)
{
	QMainWindow::resizeEvent(event);
}


void MainWindow::
addFakeContent()
{
	// fill with some content
	QBrush greenBrush(Qt::green);
	QPen outlinePen(Qt::black);
	outlinePen.setWidth(2);

	// populate with a of lines
	auto gridColor = QColor::fromRgbF(0.4, 0.4, 0.4, 1.0);
	QBrush gridBrush(gridColor);
	QPen gridPen(gridColor);

	// populate with 'content'
	auto rect = _scene->addRect(100, 0, 80, 100, outlinePen, greenBrush);
	rect->setFlag(QGraphicsItem::ItemIsMovable);

	auto text = _scene->addText("scene01", QFont("Ubuntu Mono"));
	text->setDefaultTextColor(QColor::fromRgbF(1.0, 1.0, 1.0, 1.0));
	text->setFlag(QGraphicsItem::ItemIsMovable);
	text->setPos(0, -25);

	auto widget1 = new QPushButton("Hello World");
	auto proxy1 = _scene->addWidget(widget1);
	proxy1->setPos(0, 30);

	auto widget2 = new QTextEdit();
	auto proxy2 = _scene->addWidget(widget2);
	proxy2->setPos(0, 60);
}


void MainWindow::
addNodeViews()
{
#if 1
	for (int i = 0; i < 5; i++) {
		auto n = new GraphicsNode();
		for (int j = i; j < 5; j++) {
			n->setPos(i * 25, i * 25);
			n->add_sink("sink");
			n->add_source("source");
		}

		if (i == 4) {
			QTextEdit *te = new QTextEdit();
			n->setCentralWidget(te);
		}

		n->setTitle(QString("GraphicsNode %1").arg(i));

		_scene->addItem(n);
	}

#else
	QObject* t1 = new QLineEdit();
	QObjectnode* n1 = new QObjectnode(t1);
	_scene->addItem(n1);
	n1->setPos(0,0);

	t1 = new testnode1();
	QObjectnode* n2 = new QObjectnode(t1);
	_scene->addItem(n2);
	n2->setPos(0+n1->width()*1.5,0);

	GraphicsDirectedEdge* e12 = new GraphicsBezierEdge();
	e12->connect(n1,0,n2,0);
	_scene->addItem(e12);

	t1 = new QLineEdit();
	QObjectnode* n3 = new QObjectnode(t1);
	_scene->addItem(n3);
	n3->setPos(n2->pos().x()+n2->width()*1.5,0);

	GraphicsDirectedEdge* e23 = new GraphicsBezierEdge();
	e23->connect(n2,0,n3,0);
	_scene->addItem(e23);
#endif
}

