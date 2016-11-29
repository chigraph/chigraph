#include "functionview.hpp"

#include <QHBoxLayout>

#include <KMessageBox>

#include <nodes/Node>
#include <nodes/NodeGraphicsObject>

#include <chig/NodeInstance.hpp>
#include <chig/JsonModule.hpp>

#include "chignodegui.hpp"

FunctionView::FunctionView(chig::JsonModule* module, chig::GraphFunction* func_, std::shared_ptr<DataModelRegistry> reg, QWidget* parent) : QWidget(parent), func{func_} {
	
	QHBoxLayout* hlayout = new QHBoxLayout(this);
	
	hlayout->setMargin(0);
	hlayout->setSpacing(0);
	
	scene = new FlowScene(reg);
	view = new FlowView(scene);
	
	hlayout->addWidget(view);
	
	// create nodes
	for(auto& node : func->graph.nodes) {
		
		std::shared_ptr<Node> guinode = scene->createNode(std::make_unique<ChigNodeGui>(module->context->getModuleByName(node.second->type->module.c_str()), node.second->type->name, node.second->type->toJSON()));
		
		guinode->nodeGraphicsObject()->setPos({node.second->x, node.second->y});
		
		assoc[node.second.get()] = guinode;
		
	}
	
	// create connections
	for(auto& node : func->graph.nodes) {
		
		auto thisNode = assoc[node.second.get()].lock();
		
		size_t connId = 0;
		for(auto& conn : node.second->inputDataConnections) {
			
			auto inData = assoc[conn.first].lock();
			
            scene->createConnection(thisNode, connId + node.second->inputExecConnections.size(), inData, conn.second + conn.first->outputExecConnections.size());
            
			++connId;
		}
		
		connId = 0;
		for(auto& conn : node.second->outputExecConnections) {
			
			auto outExecNode = assoc[conn.first].lock();
			
			scene->createConnection(outExecNode, conn.second, thisNode, connId);
            
			++connId;
		}
		
	}
}
