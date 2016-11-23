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
	for(auto& node : func->nodes) {
		
		std::shared_ptr<Node> guinode = scene->createNode(std::make_unique<ChigNodeGui>(module->context->getModuleByName(node.second->type->module.c_str()), node.second->type->name, node.second->type->toJSON()));
		
		guinode->nodeGraphicsObject()->setPos({node.second->x, node.second->y});
		
		assoc[node.second.get()] = guinode;
		
	}
	
	// create connections
	for(auto& node : func->nodes) {
		
		auto guinode = assoc[node.second.get()].lock();
		
		size_t connId = 0;
		for(auto& conn : node.second->inputDataConnections) {
			
			auto guiother = assoc[conn.first].lock();
			
			auto guiconn = scene->createConnection(PortType::Out, guiother, conn.first->outputDataConnections.size() + conn.second);
			guiconn->setNodeToPort(guinode, PortType::In, node.second->outputDataConnections.size() + connId);
			
			guinode->nodeState().setConnection(PortType::In,
				connId,
				guiconn);
			
			guiother->nodeState().setConnection(PortType::Out, conn.second, guiconn);
			
			++connId;
		}
		
		connId = 0;
		for(auto& conn : node.second->outputExecConnections) {
			
			auto guiother = assoc[conn.first].lock();
			
			auto guiconn = scene->createConnection(PortType::In, guiother, conn.second);
			guiconn->setNodeToPort(guinode, PortType::Out, connId);
			
			guinode->nodeState().setConnection(PortType::Out,
				connId,
				guiconn);
			
			guiother->nodeState().setConnection(PortType::In, conn.second, guiconn);
			
			++connId;
		}
		
	}
}
