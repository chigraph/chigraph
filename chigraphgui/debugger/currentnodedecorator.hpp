#pragma once

#ifndef CHIGRAPHGUI_DEBUGGER_CURRENT_NODE_DECORATOR_HPP
#define CHIGRAPHGUI_DEBUGGER_CURRENT_NODE_DECORATOR_HPP

#include <nodes/NodeDataModel>

class CurrentNodeDecorator : public QtNodes::NodePainterDelegate {
	void paint(QPainter* painter, const QtNodes::NodeGeometry& geom, QtNodes::NodeDataModel* const model) override {
		painter->drawRect(QRect(geom.width() / 2, 0, 10, 10));
	}
};

#endif // CHIGRAPHGUI_DEBUGGER_CURRENT_NODE_DECORATOR_HPP
