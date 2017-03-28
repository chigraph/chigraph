#pragma once

#ifndef CHIGGUI_FUNCTIONVIEW_HPP
#define CHIGGUI_FUNCTIONVIEW_HPP

#include <nodes/FlowScene>
#include <nodes/FlowView>

#include <chi/GraphFunction.hpp>
#include <chi/GraphModule.hpp>
#include <chi/NodeInstance.hpp>

#include <memory>
#include <unordered_map>

#include "chigraphnodemodel.hpp"

class FunctionView : public QWidget {
	Q_OBJECT
public:
	FunctionView(chi::GraphFunction& func_, QWidget* parent = nullptr);

	QtNodes::Node* guiNodeFromChigNode(chi::NodeInstance* inst);
	chi::NodeInstance* chigNodeFromGuiNode(QtNodes::Node* node);

	// refresh I/O for the node
	void refreshGuiForNode(QtNodes::Node* node);
	
	void centerOnNode(chi::NodeInstance& inst);

	void refreshRegistry();

	chi::GraphFunction* function() const { return mFunction; }

	QtNodes::FlowScene& scene() const { return *mScene; }
	
	std::vector<chi::NodeInstance*> selectedNodes();
	
	void selectNode(chi::NodeInstance& node);
	
private slots:
	void nodeAdded(QtNodes::Node& n);
	void nodeDeleted(QtNodes::Node& n);

	void connectionAdded(QtNodes::Connection& c);
	void connectionDeleted(QtNodes::Connection& c);

	void connectionUpdated(QtNodes::Connection& c);

	void nodeMoved(QtNodes::Node& n, QPointF newLoc);

	void nodeDoubleClicked(QtNodes::Node& n);

signals:
	void dirtied();
	void functionDoubleClicked(chi::GraphFunction& func);

private:
	void updateValidationStates();

	QtNodes::FlowScene* mScene;
	QtNodes::FlowView*  mView;

	chi::GraphFunction* mFunction;

	std::unordered_map<chi::NodeInstance*, QtNodes::Node*> mNodeMap;

	// this contains absolute port ids
	std::unordered_map<const QtNodes::Connection*,
	                   std::array<std::pair<chi::NodeInstance*, size_t>, 2>>
	    conns;

	std::shared_ptr<QtNodes::DataModelRegistry> createRegistry();

	// stores invalid nodes so we can clear their state when they become valid again
	std::vector<QtNodes::Node*> mInvalidNodes;
};

#endif  // CHIGGUI_FUNCTIONVIEW_HPP
