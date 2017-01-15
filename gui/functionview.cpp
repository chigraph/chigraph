#include "functionview.hpp"

#include <QHBoxLayout>

#include <KMessageBox>

#include "../src/Node.hpp"
#include "../src/NodeGraphicsObject.hpp"

#include <chig/JsonModule.hpp>
#include <chig/NodeInstance.hpp>

#include <nodes/ConnectionStyle>

#include "chigraphnodemodel.hpp"

FunctionView::FunctionView(chig::GraphFunction* func_, QWidget* parent)
	: QWidget(parent), mFunction{func_} {
	auto hlayout = new QHBoxLayout(this);

	// TODO: see how to actually set the colors
	ConnectionStyle::setConnectionStyle(R"(
    {
      "ConnectionStyle": {
        "UseDataDefinedColors": true
      }
    }
    )");

	hlayout->setMargin(0);
	hlayout->setSpacing(0);

	// create the registry
	//////////////////////

	auto reg = std::make_shared<DataModelRegistry>();

	// register dependencies + our own mod
	auto deps = mFunction->module().dependencies();
	deps.insert(mFunction->module().fullName());
	for (auto modName : deps) {
		auto module = mFunction->context().moduleByFullName(modName);
		Expects(module != nullptr);

		for (auto typeName : module->nodeTypeNames()) {
			// create that node type unless it's entry or exit
			if (modName == "lang" && (typeName == "entry" || typeName == "exit")) { continue; }

			std::unique_ptr<chig::NodeType> ty;
			module->nodeTypeFromName(typeName, {}, &ty);

			auto name = ty->qualifiedName();  // cache the name because ty is moved from
			reg->registerModel(std::make_unique<ChigraphNodeModel>(
				new chig::NodeInstance(nullptr, std::move(ty), 0, 0, name),
				this));  // TODO: this is a memory leak
		}
	}
	// register functions in this module
	// register exit -- it has to be the speical kind of exit for this function
	std::unique_ptr<chig::NodeType> ty;
	mFunction->createExitNodeType(&ty);

	reg->registerModel(std::make_unique<ChigraphNodeModel>(
		new chig::NodeInstance(nullptr, std::move(ty), 0, 0, "lang:exit"), this));

	mScene = new FlowScene(reg);

	mView = new FlowView(mScene);

	hlayout->addWidget(mView);

	// create nodes
	for (auto& node : mFunction->graph().nodes()) {
		auto& guinode =
			mScene->createNode(std::make_unique<ChigraphNodeModel>(node.second.get(), this));

		guinode.nodeGraphicsObject().setPos({node.second->x(), node.second->y()});

		mNodeMap[node.second.get()] = &guinode;
	}

	// create connections
	for (auto& node : mFunction->graph().nodes()) {
		auto thisNode = mNodeMap[node.second.get()];

		size_t connId = 0;
		for (auto& conn : node.second->inputDataConnections) {
			if (conn.first == nullptr) { continue; }
			auto inData = mNodeMap[conn.first];

			auto guiconn = mScene
							   ->createConnection(
								   *thisNode, connId + node.second->inputExecConnections.size(),
								   *inData, conn.second + conn.first->outputExecConnections.size())
							   .get();

			conns[guiconn] = {
				{{conn.first, conn.second + conn.first->outputExecConnections.size()},
				 {node.second.get(), connId + node.second->inputExecConnections.size()}}};

			++connId;
		}

		connId = 0;
		for (auto& conn : node.second->outputExecConnections) {
			auto outExecNode = mNodeMap[conn.first];

			if (outExecNode) {
				auto guiconn =
					mScene->createConnection(*outExecNode, conn.second, *thisNode, connId).get();

				conns[guiconn] = {{{node.second.get(), connId}, {conn.first, conn.second}}};
			}
			++connId;
		}
	}

	// finally connect to know when new stuff is made
	connect(mScene, &FlowScene::nodeCreated, this, &FunctionView::nodeAdded);
	connect(mScene, &FlowScene::nodeDeleted, this, &FunctionView::nodeDeleted);

	connect(mScene, &FlowScene::connectionCreated, this, &FunctionView::connectionAdded);
	connect(mScene, &FlowScene::connectionDeleted, this, &FunctionView::connectionDeleted);
}

void FunctionView::nodeAdded(Node& n) {
	auto ptr = dynamic_cast<ChigraphNodeModel*>(n.nodeDataModel());

	if (ptr == nullptr) { return; }
	// if it already exists then don't
	if (mNodeMap.find(&ptr->instance()) != mNodeMap.end()) { return; }

	mFunction->graph().nodes()[ptr->instance().id()] =
		std::unique_ptr<chig::NodeInstance>(&ptr->instance());
        
	mNodeMap[&ptr->instance()] = &n;
}

void FunctionView::nodeDeleted(Node& n) {
	auto ptr = dynamic_cast<ChigraphNodeModel*>(n.nodeDataModel());

	if (ptr == nullptr) { return; }

	// don't do it if it isn't in nodes
	if (mNodeMap.find(&ptr->instance()) == mNodeMap.end()) { return; }

	// find connections to this in conns and delete them -- removeNode does this in the chigraph
	// model now do that in the nodes model

	for (auto iter = conns.begin(); iter != conns.end();) {
		auto& pair = *iter;
		if (pair.second[0].first == &ptr->instance() || pair.second[1].first == &ptr->instance()) {
			// this doesn't invalidate iterators don't worry
			// http://en.cppreference.com/w/cpp/container/unordered_map#Iterator_invalidation
			conns.erase(iter);

			iter = conns.begin();  // reset our search so we don't miss anything, because iter was
								   // invalidated
		} else {
			++iter;  // only go to the next one if we don't go back to the beginning
		}
	}

	mFunction->removeNode(&ptr->instance());

	mNodeMap.erase(&ptr->instance());
}

void FunctionView::connectionAdded(Connection& c) {
	Node *lguinode, *rguinode;
	lguinode = c.getNode(PortType::Out);
	rguinode = c.getNode(PortType::In);

	connect(&c, &Connection::updated, this, &FunctionView::connectionUpdated);

	if (!lguinode || !rguinode) { return; }

	// here, in and out mean input and output to the connection (like in chigraph)
	auto outptr = dynamic_cast<ChigraphNodeModel*>(rguinode->nodeDataModel());
	auto inptr  = dynamic_cast<ChigraphNodeModel*>(lguinode->nodeDataModel());

	if (outptr == nullptr || inptr == nullptr) { return; }

	auto inconnid  = c.getPortIndex(PortType::Out);
	auto outconnid = c.getPortIndex(PortType::In);

	bool isExec = inconnid < inptr->instance().type().execOutputs().size();

	chig::Result res;
	if (isExec) {
		res += chig::connectExec(inptr->instance(), inconnid, outptr->instance(), outconnid);
	} else {
		res += chig::connectData(
			inptr->instance(), inconnid - inptr->instance().type().execOutputs().size(),
			outptr->instance(), outconnid - outptr->instance().type().execInputs().size());
	}
	if (!res) {
		// actually delete that connection
		mScene->deleteConnection(c);

		KMessageBox::detailedError(this, "Failed to connect!", QString::fromStdString(res.dump()));

		return;  // don't add to the array
	}

	conns[&c] = std::array<std::pair<chig::NodeInstance*, size_t>, 2>{
		{std::make_pair(&inptr->instance(), inconnid),
		 std::make_pair(&outptr->instance(), outconnid)}};
}
void FunctionView::connectionDeleted(Connection& c) {
	auto conniter = conns.find(&c);
	if (conniter == conns.end()) { return; }

	// don't do anything if

	auto conn = conniter->second;

	// see if it's data or exec
	bool isExec = conn[0].second < conn[0].first->outputExecConnections.size();

	chig::Result res;

	if (isExec) {
		res += chig::disconnectExec(*conn[0].first, conn[0].second);
	} else {
		res += chig::disconnectData(*conn[0].first,
									conn[0].second - conn[0].first->outputExecConnections.size(),
									*conn[1].first);
	}

	if (!res) {
		KMessageBox::detailedError(this, "Internal error deleting connection",
								   QString::fromStdString(res.dump()));
	}

	conns.erase(&c);
}

void FunctionView::updatePositions() {
	for (auto& inst : mNodeMap) {
		auto ptr = inst.second;
		if (ptr) {
			QPointF pos = ptr->nodeGraphicsObject().pos();
			inst.first->setX(pos.x());
			inst.first->setY(pos.y());
		}
	}
}

void FunctionView::connectionUpdated(Connection& c) {
	// find in assoc
	auto iter = conns.find(&c);
	if (iter == conns.end()) { return connectionAdded(c); }

	// remove the existing connection
}

void FunctionView::refreshGuiForNode(Node* node) {
	auto model = dynamic_cast<ChigraphNodeModel*>(node->nodeDataModel());

	auto inst = &model->instance();

	// find connections with this node
	for (auto iter = conns.begin(); iter != conns.end();) {
		auto& conn = *iter;
		if (conn.second[0].first == inst || conn.second[1].first == inst) {
			conns.erase(iter);
			iter = conns.begin();
		} else {
			++iter;
		}
	}

	QPointF pos = node->nodeGraphicsObject().pos();

	mNodeMap.erase(inst);  // so the signal doesn't do stuff
	mScene->removeNode(*node);

	mNodeMap.emplace(inst, nullptr);  // just so the signal doesn't do stuff

	auto& thisNode = mScene->createNode(std::make_unique<ChigraphNodeModel>(inst, this));
	mNodeMap[inst] = &thisNode;
	thisNode.nodeGraphicsObject().setPos(pos);

	// recreate connections
	auto id = 0ull;
	for (const auto& connSlot : inst->inputExecConnections) {
		for (const auto& conn : connSlot) {
			conns[mScene->createConnection(thisNode, id, *mNodeMap[conn.first], conn.second)
					  .get()] = {
				{std::make_pair(conn.first, conn.second), std::make_pair(inst, id)}};
		}
		++id;
	}
	id = 0;
	for (const auto& conn : inst->outputExecConnections) {
		if (conn.first) {
			conns[mScene->createConnection(*mNodeMap[conn.first], conn.second, thisNode, id)
					  .get()] = {
				{std::make_pair(inst, id), std::make_pair(conn.first, conn.second)}};
		}
		++id;
	}
	id = 0;
	for (const auto& conn : inst->inputDataConnections) {
		if (conn.first) {
			auto remoteID = conn.second + conn.first->outputExecConnections.size();
			auto localID  = id + inst->inputExecConnections.size();
			conns[mScene->createConnection(thisNode, localID, *mNodeMap[conn.first], remoteID)
					  .get()] = {{{conn.first, remoteID}, {inst, localID}}};
		}
		++id;
	}
	id = 0;
	for (const auto& connSlot : inst->outputDataConnections) {
		for (const auto& conn : connSlot) {
			auto remoteID = conn.second + conn.first->inputExecConnections.size();
			auto localID  = id + inst->outputExecConnections.size();
			conns[mScene->createConnection(*mNodeMap[conn.first], remoteID, thisNode, localID)
					  .get()] = {{{inst, localID}, {conn.first, remoteID}}};
		}
	}
}

Node* FunctionView::guiNodeFromChigNode(chig::NodeInstance* inst) {
	auto iter = mNodeMap.find(inst);
	if (iter != mNodeMap.end()) { return iter->second; }
	return nullptr;
}

chig::NodeInstance* FunctionView::chigNodeFromGuiNode(Node* node) {
	auto nodeGui = dynamic_cast<ChigraphNodeModel*>(node->nodeDataModel());
	if (nodeGui == nullptr) { return nullptr; }
	return &nodeGui->instance();
}
