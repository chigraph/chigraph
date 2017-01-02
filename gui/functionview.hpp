#pragma once

#ifndef CHIGGUI_FUNCTIONVIEW_HPP
#define CHIGGUI_FUNCTIONVIEW_HPP

#include <nodes/FlowScene>
#include <nodes/FlowView>

#include <chig/GraphFunction.hpp>
#include <chig/JsonModule.hpp>
#include <chig/NodeInstance.hpp>

#include <memory>
#include <unordered_map>

#include <boost/bimap.hpp>

class FunctionView : public QWidget
{
	Q_OBJECT
public:
	FlowScene* scene;
	FlowView* view;

	FunctionView(chig::GraphFunction* func_, QWidget* parent = nullptr);

	chig::GraphFunction* func;

	std::unordered_map<chig::NodeInstance*, Node*> nodes;

	// this contains absolute port ids
	std::unordered_map<const Connection*, std::array<std::pair<chig::NodeInstance*, size_t>, 2>>
		conns;

	void updatePositions();

public slots:
	void nodeAdded(Node& n);
	void nodeDeleted(Node& n);

	void connectionAdded(Connection& c);
	void connectionDeleted(Connection& c);

	void connectionUpdated(Connection& c);

private:
	bool creating = true;
};

#endif  // CHIGGUI_FUNCTIONVIEW_HPP
