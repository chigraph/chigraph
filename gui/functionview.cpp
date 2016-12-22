#include "functionview.hpp"

#include <QHBoxLayout>

#include <KMessageBox>

#include "../src/Node.hpp"
#include "../src/NodeGraphicsObject.hpp"

#include <chig/JsonModule.hpp>
#include <chig/NodeInstance.hpp>

#include "chignodegui.hpp"

FunctionView::FunctionView(
	chig::GraphFunction* func_, std::shared_ptr<DataModelRegistry> reg, QWidget* parent)
	: QWidget(parent), func{func_}
{
	auto hlayout = new QHBoxLayout(this);

	hlayout->setMargin(0);
	hlayout->setSpacing(0);

	scene = new FlowScene(reg);
	connect(scene, &FlowScene::nodeCreated, this, &FunctionView::nodeAdded);
	connect(scene, &FlowScene::nodeDeleted, this, &FunctionView::nodeDeleted);

	connect(scene, &FlowScene::connectionCreated, this, &FunctionView::connectionAdded);
	connect(scene, &FlowScene::connectionDeleted, this, &FunctionView::connectionDeleted);

	view = new FlowView(scene);

	hlayout->addWidget(view);

	// create nodes
	for (auto& node : func->graph().nodes()) {
		std::shared_ptr<Node> guinode =
			scene->createNode(std::make_unique<ChigNodeGui>(node.second.get()));

		guinode->nodeGraphicsObject().setPos({node.second->x(), node.second->y()});

		assoc[node.second.get()] = guinode;
	}

	// create connections
	for (auto& node : func->graph().nodes()) {
		auto thisNode = assoc[node.second.get()].lock();

		size_t connId = 0;
		for (auto& conn : node.second->inputDataConnections) {
			if (conn.first == nullptr) {
				continue;
			}
			auto inData = assoc[conn.first].lock();

			auto guiconn =
				scene
					->createConnection(thisNode, connId + node.second->inputExecConnections.size(),
						inData, conn.second + conn.first->outputExecConnections.size())
					.get();

			conns[guiconn] = {{{conn.first, conn.second + conn.first->outputExecConnections.size()},
				{node.second.get(), connId + node.second->inputExecConnections.size()}}};

			++connId;
		}

		connId = 0;
		for (auto& conn : node.second->outputExecConnections) {
			auto outExecNode = assoc[conn.first].lock();

			if (outExecNode) {
				auto guiconn =
					scene->createConnection(outExecNode, conn.second, thisNode, connId).get();

				conns[guiconn] = {{{node.second.get(), connId}, {conn.first, conn.second}}};
			}
			++connId;
		}
	}

	creating = false;
}

void FunctionView::nodeAdded(Node& n)
{
	if (creating) {
		return;
	}

	auto ptr = dynamic_cast<ChigNodeGui*>(n.nodeDataModel());

	if (ptr == nullptr) {
		return;
	}

	func->graph().nodes()[ptr->inst->id()] = std::unique_ptr<chig::NodeInstance>(ptr->inst);
}
void FunctionView::nodeDeleted(Node& n)
{
	auto ptr = dynamic_cast<ChigNodeGui*>(n.nodeDataModel());

	if (ptr == nullptr) {
		return;
	}

	func->graph().nodes().erase(ptr->inst->id());
}

void FunctionView::connectionAdded(const Connection& c)
{
	if (creating) {
		return;
	}

	std::shared_ptr<Node> lguinode, rguinode;
	lguinode = c.getNode(PortType::Out).lock();
	rguinode = c.getNode(PortType::In).lock();

	connect(&c, &Connection::updated, this, &FunctionView::connectionUpdated);

	if (!lguinode || !rguinode) {
		return;
	}

	// here, in and out mean input and output to the connection (like in chigraph)
	auto outptr = dynamic_cast<ChigNodeGui*>(rguinode->nodeDataModel());
	auto inptr = dynamic_cast<ChigNodeGui*>(lguinode->nodeDataModel());

	if (outptr == nullptr || inptr == nullptr) {
		return;
	}

	size_t inconnid = c.getPortIndex(PortType::Out);
	size_t outconnid = c.getPortIndex(PortType::In);

	conns[&c] = std::array<std::pair<chig::NodeInstance*, size_t>, 2>{
		{std::make_pair(inptr->inst, inconnid), std::make_pair(outptr->inst, outconnid)}};

	bool isExec = inconnid < inptr->inst->type().execOutputs().size();

	if (isExec) {
		chig::connectExec(*inptr->inst, inconnid, *outptr->inst, outconnid);
	} else {
		chig::connectData(*inptr->inst, inconnid - inptr->inst->type().execOutputs().size(),
			*outptr->inst, outconnid - outptr->inst->type().execInputs().size());
	}
}
void FunctionView::connectionDeleted(Connection& c)
{
	auto conniter = conns.find(&c);
	if (conniter == conns.end()) {
		return;
	}

	auto conn = conniter->second;

	// see if it's data or exec
	bool isExec = conn[0].second < conn[0].first->outputExecConnections.size();

	chig::Result res;

	if (isExec) {
		res += chig::disconnectExec(*conn[0].first, conn[0].second);
	} else {
		res += chig::disconnectData(*conn[0].first,
			conn[0].second - conn[0].first->outputExecConnections.size(), *conn[1].first);
	}

	if (!res) {
		KMessageBox::detailedError(this, "Internal error deleting connection",
			QString::fromStdString(res.dump()));
	}

	conns.erase(&c);
}

void FunctionView::updatePositions()
{
	for (auto& inst : assoc) {
		auto sptr = inst.second.lock();
		if (sptr) {
			QPointF pos = sptr->nodeGraphicsObject().pos();
			inst.first->setX(pos.x());
			inst.first->setY(pos.y());
		}
	}
}

void FunctionView::connectionUpdated(const Connection& c)
{
	if (creating) return;

	// find in assoc
	auto iter = conns.find(&c);
	if (iter == conns.end()) {
		return connectionAdded(c);
	}

	// remove the existing connection
}
