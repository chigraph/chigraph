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

#include "chignodegui.hpp"

class FunctionView : public QWidget
{
	Q_OBJECT
public:

	FunctionView(chig::GraphFunction* func_, QWidget* parent = nullptr);

    Node* guiNodeFromChigNode(chig::NodeInstance* inst);    
    chig::NodeInstance* chigNodeFromGuiNode(Node* node);
    
	void updatePositions();

    // refresh I/O for the node
	void refreshGuiForNode(Node* node);
    
    chig::GraphFunction* function() const { return mFunction; }

private slots:
	void nodeAdded(Node& n);
	void nodeDeleted(Node& n);

	void connectionAdded(Connection& c);
	void connectionDeleted(Connection& c);

	void connectionUpdated(Connection& c);
    
private:
	FlowScene* mScene;
	FlowView* mView;
    
	chig::GraphFunction* mFunction;
    
	std::unordered_map<chig::NodeInstance*, Node*> mNodeMap;
    
    
	// this contains absolute port ids
	std::unordered_map<const Connection*, std::array<std::pair<chig::NodeInstance*, size_t>, 2>>
		conns;

};

#endif  // CHIGGUI_FUNCTIONVIEW_HPP
