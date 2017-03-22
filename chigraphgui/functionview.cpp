#include "functionview.hpp"
#include "mainwindow.hpp"

#include <QHBoxLayout>

#include <KMessageBox>

#include <chi/GraphModule.hpp>
#include <chi/FunctionValidator.hpp>
#include <chi/NodeInstance.hpp>

#include <../src/Node.hpp>
#include <nodes/ConnectionStyle>

#include "chigraphnodemodel.hpp"

using namespace QtNodes;

FunctionView::FunctionView(MainWindow* mainWindow, chi::GraphFunction* func_, QWidget* parent)
    : QWidget(parent), mFunction{func_}, mMainWindow{mainWindow} {
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

	mScene = new FlowScene(createRegistry());

	mView = new FlowView(mScene);

	hlayout->addWidget(mView);

	// create nodes
	for (auto& node : mFunction->nodes()) {
		auto& guinode =
		    mScene->createNode(std::make_unique<ChigraphNodeModel>(node.second.get(), this));

		mScene->setNodePosition(guinode, {node.second->x(), node.second->y()});

		mNodeMap[node.second.get()] = &guinode;
	}

	// create connections
	for (auto& node : mFunction->nodes()) {
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

			if (outExecNode != nullptr) {
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

	connect(mScene, &FlowScene::nodeMoved, this, &FunctionView::nodeMoved);
	connect(mScene, &FlowScene::nodeDoubleClicked, this, &FunctionView::nodeDoubleClicked);
}

void FunctionView::nodeAdded(Node& n) {
	auto ptr = dynamic_cast<ChigraphNodeModel*>(n.nodeDataModel());

	if (ptr == nullptr) { return; }
	// if it already exists then don't
	if (mNodeMap.find(&ptr->instance()) != mNodeMap.end()) { return; }

	mFunction->nodes()[ptr->instance().id()] =
	    std::unique_ptr<chi::NodeInstance>(&ptr->instance());

	mNodeMap[&ptr->instance()] = &n;

	dirtied();
	updateValidationStates();
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
	dirtied();
	updateValidationStates();
}

void FunctionView::connectionAdded(Connection& c) {
	Node *lguinode, *rguinode;
	lguinode = c.getNode(PortType::Out);
	rguinode = c.getNode(PortType::In);

	connect(&c, &Connection::updated, this, &FunctionView::connectionUpdated);

	if (lguinode == nullptr || rguinode == nullptr) { return; }

	// here, in and out mean input and output to the connection (like in chigraph)
	auto outptr = dynamic_cast<ChigraphNodeModel*>(rguinode->nodeDataModel());
	auto inptr  = dynamic_cast<ChigraphNodeModel*>(lguinode->nodeDataModel());

	if (outptr == nullptr || inptr == nullptr) { return; }

	auto inconnid  = c.getPortIndex(PortType::Out);
	auto outconnid = c.getPortIndex(PortType::In);

	bool isExec = inconnid < inptr->instance().type().execOutputs().size();

	chi::Result res;
	if (isExec) {
		res += chi::connectExec(inptr->instance(), inconnid, outptr->instance(), outconnid);
	} else {
		res += chi::connectData(
		    inptr->instance(), inconnid - inptr->instance().type().execOutputs().size(),
		    outptr->instance(), outconnid - outptr->instance().type().execInputs().size());
	}
	if (!res) {
		// actually delete that connection
		mScene->deleteConnection(c);

		KMessageBox::detailedError(this, "Failed to connect!", QString::fromStdString(res.dump()));

		return;  // don't add to the array
	}

	conns[&c] = std::array<std::pair<chi::NodeInstance*, size_t>, 2>{
	    {std::make_pair(&inptr->instance(), inconnid),
	     std::make_pair(&outptr->instance(), outconnid)}};
	dirtied();
	updateValidationStates();
}
void FunctionView::connectionDeleted(Connection& c) {
	auto conniter = conns.find(&c);
	if (conniter == conns.end()) { return; }

	// don't do anything if

	auto conn = conniter->second;

	// see if it's data or exec
	bool isExec = conn[0].second < conn[0].first->outputExecConnections.size();

	chi::Result res;

	if (isExec) {
		res += chi::disconnectExec(*conn[0].first, conn[0].second);
	} else {
		res += chi::disconnectData(*conn[0].first,
		                            conn[0].second - conn[0].first->outputExecConnections.size(),
		                            *conn[1].first);
	}

	if (!res) {
		KMessageBox::detailedError(this, "Internal error deleting connection",
		                           QString::fromStdString(res.dump()));
	}

	conns.erase(&c);
	dirtied();
	updateValidationStates();
}

void FunctionView::connectionUpdated(Connection& c) {
	// find in assoc
	auto iter = conns.find(&c);
	if (iter == conns.end()) { return connectionAdded(c); }

	dirtied();
	// remove the existing connection
}

void FunctionView::nodeMoved(Node& n, QPointF newLoc) {
	auto model = dynamic_cast<ChigraphNodeModel*>(n.nodeDataModel());

	model->instance().setX(newLoc.x());
	model->instance().setY(newLoc.y());
	dirtied();
}

void FunctionView::nodeDoubleClicked(QtNodes::Node& n) {
	// see if we can get the implementation
	auto model = dynamic_cast<ChigraphNodeModel*>(n.nodeDataModel());

	auto* graphMod = dynamic_cast<chi::GraphModule*>(&model->instance().type().module());
	if (graphMod == nullptr) { return; }

	auto func = graphMod->functionFromName(model->instance().type().name());
	if (func == nullptr) { return; }
	// load the right module
	mMainWindow->openModule(QString::fromStdString(graphMod->fullName()));

	// load the right function
	mMainWindow->newFunctionSelected(func);
}

void FunctionView::addBreakpoint(QtNodes::Node& n) {
  // TODO: implement
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

	QPointF pos = mScene->getNodePosition(*node);

	mNodeMap.erase(inst);  // so the signal doesn't do stuff
	mScene->removeNode(*node);

	mNodeMap.emplace(inst, nullptr);  // just so the signal doesn't do stuff

	auto& thisNode = mScene->createNode(std::make_unique<ChigraphNodeModel>(inst, this));
	mNodeMap[inst] = &thisNode;
	mScene->setNodePosition(thisNode, pos);

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
		if (conn.first != nullptr) {
			conns[mScene->createConnection(*mNodeMap[conn.first], conn.second, thisNode, id)
			          .get()] = {
			    {std::make_pair(inst, id), std::make_pair(conn.first, conn.second)}};
		}
		++id;
	}
	id = 0;
	for (const auto& conn : inst->inputDataConnections) {
		if (conn.first != nullptr) {
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
	dirtied();
}

void FunctionView::refreshRegistry() { mScene->setRegistry(createRegistry()); }

Node* FunctionView::guiNodeFromChigNode(chi::NodeInstance* inst) {
	auto iter = mNodeMap.find(inst);
	if (iter != mNodeMap.end()) { return iter->second; }
	return nullptr;
}

chi::NodeInstance* FunctionView::chigNodeFromGuiNode(Node* node) {
	auto nodeGui = dynamic_cast<ChigraphNodeModel*>(node->nodeDataModel());
	if (nodeGui == nullptr) { return nullptr; }
	return &nodeGui->instance();
}

std::shared_ptr<DataModelRegistry> FunctionView::createRegistry() {
	auto reg = std::make_shared<DataModelRegistry>();

	// register dependencies + our own mod
	auto deps = mFunction->module().dependencies();

	// register functions in this module
	deps.insert(mFunction->module().fullName());
	for (auto modName : deps) {
		auto module = mFunction->context().moduleByFullName(modName);
		Expects(module != nullptr);

		for (auto typeName : module->nodeTypeNames()) {
			// create that node type unless it's entry or exit
			if (modName == "lang" && (typeName == "entry" || typeName == "exit")) { continue; }

			std::unique_ptr<chi::NodeType> ty;
			module->nodeTypeFromName(typeName, {}, &ty);

			auto name = ty->qualifiedName();  // cache the name because ty is moved from
			reg->registerModel(std::make_unique<ChigraphNodeModel>(
			    new chi::NodeInstance(mFunction, std::move(ty), 0, 0,
			                           boost::uuids::random_generator()()),
			    this), QString::fromStdString(modName.generic_path().string()));  // TODO: this is a memory leak
		}
	}
	// register exit -- it has to be the speical kind of exit for this function
	std::unique_ptr<chi::NodeType> ty;
	mFunction->createExitNodeType(&ty);

	reg->registerModel(std::make_unique<ChigraphNodeModel>(
	    new chi::NodeInstance(mFunction, std::move(ty), 0, 0), this), QString::fromStdString(mFunction->module().fullName()));

	// register local variable setters and getters
	for (const auto& local : mFunction->localVariables()) {
		mFunction->module().nodeTypeFromName("_set_" + local.name, local.type.qualifiedName(), &ty);

		reg->registerModel(std::make_unique<ChigraphNodeModel>(
		    new chi::NodeInstance(mFunction, std::move(ty), 0, 0), this), i18n("Local Variables"));

		mFunction->module().nodeTypeFromName("_get_" + local.name, local.type.qualifiedName(), &ty);

		reg->registerModel(std::make_unique<ChigraphNodeModel>(
		    new chi::NodeInstance(mFunction, std::move(ty), 0, 0), this), i18n("Local Variables"));
	}

	return reg;
}

void FunctionView::updateValidationStates() {
	// validate the function
	auto result = chi::validateFunction(*mFunction);
	
	// clear validation states of the other nodes
	for (auto n : mInvalidNodes) {
		auto castedModel = dynamic_cast<ChigraphNodeModel*>(n->nodeDataModel());
		castedModel->setErrorState(QtNodes::NodeValidationState::Valid, {});
	}
	mInvalidNodes.clear();
	
	// reset those which which are invaid now
	for (auto errJson : result.result_json) {
		auto& data = errJson["data"];
		
		// parse out Node ID
		if (data.find("Node ID") != data.end()) {
			std::string s = data["Node ID"];
			auto id =  boost::uuids::string_generator()(s);
			
			std::string errCode = errJson["errorcode"];
			bool isError = errCode[0] == 'E';
			
			std::string overview = errJson["overview"];
			
			auto node = mFunction->nodeByID(id);
			if (node != nullptr) {
				auto guiNode = guiNodeFromChigNode(node);
				if (guiNode != nullptr) {
					auto castedModel = dynamic_cast<ChigraphNodeModel*>(guiNode->nodeDataModel());
					castedModel->setErrorState(isError ? QtNodes::NodeValidationState::Error : 
						QtNodes::NodeValidationState::Warning, QString::fromStdString(overview));
				}
				mInvalidNodes.push_back(guiNode);
			}
		}
	}
	
}
