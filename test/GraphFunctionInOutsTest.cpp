#include <catch.hpp>
#include "TestCommon.hpp"

#include <chig/Context.hpp>
#include <chig/GraphFunction.hpp>
#include <chig/GraphModule.hpp>
#include <chig/GraphStruct.hpp>
#include <chig/LangModule.hpp>
#include <chig/NodeInstance.hpp>
#include <chig/Result.hpp>

using namespace chig;
using namespace nlohmann;

TEST_CASE("Create and manipulate inputs and outputs in GraphFunctions", "") {
	Context c;
	Result  res;

	res = c.loadModule("lang");
	REQUIRE(!!res);

	auto& langMod = *c.langModule();

	auto mod      = c.newGraphModule("test/main");
	bool inserted = false;

	// create an empty function
	auto func = mod->getOrCreateFunction("main", {}, {}, {}, {}, &inserted);
	REQUIRE(func != nullptr);

	// create some nodes
	NodeInstance* entryNode;
	res = func->getOrInsertEntryNode(0.f, 0.f, "entry", &entryNode);
	REQUIRE(!!res);

	std::unique_ptr<NodeType> exitType;
	res = func->createExitNodeType(&exitType);
	REQUIRE(!!res);

	std::unique_ptr<NodeType> exitType2 = exitType->clone();
	NodeInstance*             exitNode1;
	res = func->insertNode(std::move(exitType), 0.f, 0.f, "exit1", &exitNode1);
	REQUIRE(!!res);

	NodeInstance* exitNode2;
	res = func->insertNode(std::move(exitType2), 0.f, 0.f, "exit2", &exitNode2);

	// add a local so we can have get and set nodes to test if they disconnected correctly
	func->getOrCreateLocalVariable("var", langMod.typeFromName("i32"));

	NodeInstance* setNode;
	func->insertNode("test/main", "_set_var", "lang:i32", 0, 0, "set", &setNode);

	NodeInstance* setNode2;
	func->insertNode("test/main", "_set_var", "lang:i32", 0, 0, "set2", &setNode2);

	NodeInstance* getNode;
	func->insertNode("test/main", "_get_var", "lang:i32", 0, 0, "get", &getNode);

	WHEN("We add a data input") {
		NamedDataType toAdd{"param1", langMod.typeFromName("i32")};

		func->addDataInput(toAdd.type, toAdd.name);

		REQUIRE(func->dataInputs().size() == 1);
		REQUIRE(func->dataInputs()[0] == toAdd);

		// check entry node
		REQUIRE(entryNode->type().dataInputs().size() == 0);
		REQUIRE(entryNode->type().dataOutputs().size() == 1);
		REQUIRE(entryNode->type().dataOutputs()[0] == toAdd);

		// create another node to test disconnecting
		connectData(*entryNode, 0, *setNode, 0);

		REQUIRE(entryNode->outputDataConnections[0].size() == 1);
		REQUIRE(entryNode->outputDataConnections[0][0].first == setNode);
		REQUIRE(entryNode->outputDataConnections[0][0].second == 0);

		WHEN("We add another after that") {
			NamedDataType toAdd2{"param2", langMod.typeFromName("i1")};

			func->addDataInput(toAdd2.type, toAdd2.name, 1);

			REQUIRE(func->dataInputs().size() == 2);
			REQUIRE(func->dataInputs()[0] == toAdd);
			REQUIRE(func->dataInputs()[1] == toAdd2);

			// check entry node
			REQUIRE(entryNode->type().dataInputs().size() == 0);
			REQUIRE(entryNode->type().dataOutputs().size() == 2);
			REQUIRE(entryNode->type().dataOutputs()[0] == toAdd);
			REQUIRE(entryNode->type().dataOutputs()[1] == toAdd2);
		}

		WHEN("We add another before that") {
			NamedDataType toAdd2{"param2", langMod.typeFromName("i1")};

			func->addDataInput(toAdd2.type, toAdd2.name, 0);

			REQUIRE(func->dataInputs().size() == 2);
			REQUIRE(func->dataInputs()[0] == toAdd2);
			REQUIRE(func->dataInputs()[1] == toAdd);

			// check entry node
			REQUIRE(entryNode->type().dataInputs().size() == 0);
			REQUIRE(entryNode->type().dataOutputs().size() == 2);
			REQUIRE(entryNode->type().dataOutputs()[0] == toAdd2);
			REQUIRE(entryNode->type().dataOutputs()[1] == toAdd);
		}

		WHEN("We rename it") {
			func->renameDataInput(0, "param1renamed");

			REQUIRE(func->dataInputs().size() == 1);
			REQUIRE((func->dataInputs()[0] ==
			         NamedDataType{"param1renamed", langMod.typeFromName("i32")}));

			// check entry node
			REQUIRE(entryNode->type().dataInputs().size() == 0);
			REQUIRE(entryNode->type().dataOutputs().size() == 1);
			REQUIRE((entryNode->type().dataOutputs()[0] ==
			         NamedDataType{"param1renamed", langMod.typeFromName("i32")}));

			// should still be connected
			REQUIRE(entryNode->outputDataConnections[0].size() == 1);
			REQUIRE(entryNode->outputDataConnections[0][0].first == setNode);
			REQUIRE(entryNode->outputDataConnections[0][0].second == 0);
		}

		WHEN("We rename it using the wrong index") {
			func->renameDataInput(1, "param1renamed");

			// it should be exactly the same
			REQUIRE(func->dataInputs().size() == 1);
			REQUIRE(func->dataInputs()[0] == toAdd);

			// check entry node
			REQUIRE(entryNode->type().dataInputs().size() == 0);
			REQUIRE(entryNode->type().dataOutputs().size() == 1);
			REQUIRE(entryNode->type().dataOutputs()[0] == toAdd);
		}

		WHEN("We retype it") {
			func->retypeDataInput(0, langMod.typeFromName("i1"));

			REQUIRE(func->dataInputs().size() == 1);
			REQUIRE((func->dataInputs()[0] == NamedDataType{"param1", langMod.typeFromName("i1")}));

			// check entry node
			REQUIRE(entryNode->type().dataInputs().size() == 0);
			REQUIRE(entryNode->type().dataOutputs().size() == 1);
			REQUIRE((entryNode->type().dataOutputs()[0] ==
			         NamedDataType{"param1", langMod.typeFromName("i1")}));

			// should't be connected
			REQUIRE(entryNode->outputDataConnections[0].size() == 0);
		}

		WHEN("We retype it with the wrong index") {
			func->retypeDataInput(1, langMod.typeFromName("i1"));

			// it should be exactly the same
			REQUIRE(func->dataInputs().size() == 1);
			REQUIRE(func->dataInputs()[0] == toAdd);

			// check entry node
			REQUIRE(entryNode->type().dataInputs().size() == 0);
			REQUIRE(entryNode->type().dataOutputs().size() == 1);
			REQUIRE(entryNode->type().dataOutputs()[0] == toAdd);

			// should still be connected
			REQUIRE(entryNode->outputDataConnections[0].size() == 1);
			REQUIRE(entryNode->outputDataConnections[0][0].first == setNode);
			REQUIRE(entryNode->outputDataConnections[0][0].second == 0);
		}
	}

	WHEN("We add a data output") {
		NamedDataType toAdd{"outparam1", langMod.typeFromName("i32")};

		func->addDataOutput(toAdd.type, toAdd.name);

		REQUIRE(func->dataOutputs().size() == 1);
		REQUIRE(func->dataOutputs()[0] == toAdd);

		// check exit nodes
		REQUIRE(exitNode1->type().dataOutputs().size() == 0);
		REQUIRE(exitNode1->type().dataInputs().size() == 1);
		REQUIRE(exitNode1->type().dataInputs()[0] == toAdd);

		REQUIRE(exitNode2->type().dataOutputs().size() == 0);
		REQUIRE(exitNode2->type().dataInputs().size() == 1);
		REQUIRE(exitNode2->type().dataInputs()[0] == toAdd);

		connectData(*getNode, 0, *exitNode1, 0);
		connectData(*getNode, 0, *exitNode2, 0);

		// check connection
		REQUIRE(exitNode1->inputDataConnections[0].first == getNode);
		REQUIRE(exitNode1->inputDataConnections[0].second == 0);
		REQUIRE(exitNode2->inputDataConnections[0].first == getNode);
		REQUIRE(exitNode2->inputDataConnections[0].second == 0);

		WHEN("We add another after that") {
			NamedDataType toAdd2{"outparam2", langMod.typeFromName("i1")};

			func->addDataOutput(toAdd2.type, toAdd2.name, 1);

			REQUIRE(func->dataOutputs().size() == 2);
			REQUIRE(func->dataOutputs()[0] == toAdd);
			REQUIRE(func->dataOutputs()[1] == toAdd2);

			// check exit nodes
			REQUIRE(exitNode1->type().dataOutputs().size() == 0);
			REQUIRE(exitNode1->type().dataInputs().size() == 2);
			REQUIRE(exitNode1->type().dataInputs()[0] == toAdd);
			REQUIRE(exitNode1->type().dataInputs()[1] == toAdd2);

			REQUIRE(exitNode2->type().dataOutputs().size() == 0);
			REQUIRE(exitNode2->type().dataInputs().size() == 2);
			REQUIRE(exitNode2->type().dataInputs()[0] == toAdd);
			REQUIRE(exitNode2->type().dataInputs()[1] == toAdd2);
		}

		WHEN("We add another before that") {
			NamedDataType toAdd2{"outparam2", langMod.typeFromName("i1")};

			func->addDataOutput(toAdd2.type, toAdd2.name, 0);

			REQUIRE(func->dataOutputs().size() == 2);
			REQUIRE(func->dataOutputs()[0] == toAdd2);
			REQUIRE(func->dataOutputs()[1] == toAdd);

			// check exit nodes
			REQUIRE(exitNode1->type().dataOutputs().size() == 0);
			REQUIRE(exitNode1->type().dataInputs().size() == 2);
			REQUIRE(exitNode1->type().dataInputs()[0] == toAdd2);
			REQUIRE(exitNode1->type().dataInputs()[1] == toAdd);

			REQUIRE(exitNode2->type().dataOutputs().size() == 0);
			REQUIRE(exitNode2->type().dataInputs().size() == 2);
			REQUIRE(exitNode2->type().dataInputs()[0] == toAdd2);
			REQUIRE(exitNode2->type().dataInputs()[1] == toAdd);
		}

		WHEN("We rename it") {
			func->renameDataOutput(0, "param1renamed");
			NamedDataType newTy{"param1renamed", langMod.typeFromName("i32")};

			REQUIRE(func->dataOutputs().size() == 1);
			REQUIRE(func->dataOutputs()[0] == newTy);

			// check entry node
			REQUIRE(exitNode1->type().dataOutputs().size() == 0);
			REQUIRE(exitNode1->type().dataInputs().size() == 1);
			REQUIRE(exitNode1->type().dataInputs()[0] == newTy);

			// should still be connected
			REQUIRE(exitNode1->inputDataConnections[0].first == getNode);
			REQUIRE(exitNode1->inputDataConnections[0].second == 0);
			REQUIRE(exitNode2->inputDataConnections[0].first == getNode);
			REQUIRE(exitNode2->inputDataConnections[0].second == 0);
		}

		WHEN("We rename it using the wrong index") {
			func->renameDataOutput(1, "param1renamed");

			// it should be exactly the same
			REQUIRE(func->dataOutputs().size() == 1);
			REQUIRE(func->dataOutputs()[0] == toAdd);

			// check exit nodes
			REQUIRE(exitNode1->type().dataOutputs().size() == 0);
			REQUIRE(exitNode1->type().dataInputs().size() == 1);
			REQUIRE(exitNode1->type().dataInputs()[0] == toAdd);

			REQUIRE(exitNode2->type().dataOutputs().size() == 0);
			REQUIRE(exitNode2->type().dataInputs().size() == 1);
			REQUIRE(exitNode2->type().dataInputs()[0] == toAdd);

			// should still be connected
			REQUIRE(exitNode1->inputDataConnections[0].first == getNode);
			REQUIRE(exitNode1->inputDataConnections[0].second == 0);
			REQUIRE(exitNode2->inputDataConnections[0].first == getNode);
			REQUIRE(exitNode2->inputDataConnections[0].second == 0);
		}

		WHEN("We retype it") {
			func->retypeDataOutput(0, langMod.typeFromName("i1"));
			NamedDataType newTy{"outparam1", langMod.typeFromName("i1")};

			REQUIRE(func->dataOutputs().size() == 1);
			REQUIRE(func->dataOutputs()[0] == newTy);

			REQUIRE(func->dataOutputs().size() == 1);
			REQUIRE(func->dataOutputs()[0] == newTy);

			// check entry node
			REQUIRE(exitNode1->type().dataOutputs().size() == 0);
			REQUIRE(exitNode1->type().dataInputs().size() == 1);
			REQUIRE(exitNode1->type().dataInputs()[0] == newTy);

			// should't be connected
			REQUIRE(exitNode1->inputDataConnections[0].first == nullptr);
			REQUIRE(exitNode1->inputDataConnections[0].second == ~0ull);
			REQUIRE(exitNode2->inputDataConnections[0].first == nullptr);
			REQUIRE(exitNode2->inputDataConnections[0].second == ~0ull);
		}

		WHEN("We retype it with the wrong index") {
			func->retypeDataOutput(1, langMod.typeFromName("i1"));

			// it should be exactly the same
			REQUIRE(func->dataOutputs().size() == 1);
			REQUIRE(func->dataOutputs()[0] == toAdd);

			// check exit nodes
			REQUIRE(exitNode1->type().dataOutputs().size() == 0);
			REQUIRE(exitNode1->type().dataInputs().size() == 1);
			REQUIRE(exitNode1->type().dataInputs()[0] == toAdd);

			REQUIRE(exitNode2->type().dataOutputs().size() == 0);
			REQUIRE(exitNode2->type().dataInputs().size() == 1);
			REQUIRE(exitNode2->type().dataInputs()[0] == toAdd);

			// should still be connected
			REQUIRE(exitNode1->inputDataConnections[0].first == getNode);
			REQUIRE(exitNode1->inputDataConnections[0].second == 0);
			REQUIRE(exitNode2->inputDataConnections[0].first == getNode);
			REQUIRE(exitNode2->inputDataConnections[0].second == 0);
		}
	}

	WHEN("We add a exec input") {
		func->addExecInput("ein");

		REQUIRE(func->execInputs().size() == 1);
		REQUIRE(func->execInputs()[0] == "ein");

		// check entry nodes
		REQUIRE(entryNode->type().execInputs().size() == 0);
		REQUIRE(entryNode->type().execOutputs().size() == 1);
		REQUIRE(entryNode->type().execOutputs()[0] == "ein");

		connectExec(*entryNode, 0, *setNode, 0);

		// check connection
		REQUIRE(entryNode->outputExecConnections[0].first == setNode);
		REQUIRE(entryNode->outputExecConnections[0].second == 0);

		WHEN("We add another after that") {
			func->addExecInput("ein2", 1);

			REQUIRE(func->execInputs().size() == 2);
			REQUIRE(func->execInputs()[0] == "ein");
			REQUIRE(func->execInputs()[1] == "ein2");

			// check entry nodes
			REQUIRE(entryNode->type().execInputs().size() == 0);
			REQUIRE(entryNode->type().execOutputs().size() == 2);
			REQUIRE(entryNode->type().execOutputs()[0] == "ein");
			REQUIRE(entryNode->type().execOutputs()[1] == "ein2");
		}

		WHEN("We add another before that") {
			func->addExecInput("ein2", 0);

			REQUIRE(func->execInputs().size() == 2);
			REQUIRE(func->execInputs()[0] == "ein2");
			REQUIRE(func->execInputs()[1] == "ein");

			// check entry nodes
			REQUIRE(entryNode->type().execInputs().size() == 0);
			REQUIRE(entryNode->type().execOutputs().size() == 2);
			REQUIRE(entryNode->type().execOutputs()[0] == "ein2");
			REQUIRE(entryNode->type().execOutputs()[1] == "ein");
		}

		WHEN("We rename it") {
			func->renameExecInput(0, "einrenamed");

			REQUIRE(func->execInputs().size() == 1);
			REQUIRE(func->execInputs()[0] == "einrenamed");

			// check entry nodes
			REQUIRE(entryNode->type().execInputs().size() == 0);
			REQUIRE(entryNode->type().execOutputs().size() == 1);
			REQUIRE(entryNode->type().execOutputs()[0] == "einrenamed");
		}

		WHEN("We rename it using the wrong index") {
			func->renameExecInput(1, "param1renamed");

			REQUIRE(func->execInputs().size() == 1);
			REQUIRE(func->execInputs()[0] == "ein");

			// check entry nodes
			REQUIRE(entryNode->type().execInputs().size() == 0);
			REQUIRE(entryNode->type().execOutputs().size() == 1);
			REQUIRE(entryNode->type().execOutputs()[0] == "ein");

			// check connection
			REQUIRE(entryNode->outputExecConnections[0].first == setNode);
			REQUIRE(entryNode->outputExecConnections[0].second == 0);
		}
	}

	WHEN("We add a exec output") {
		func->addExecOutput("eout");

		REQUIRE(func->execOutputs().size() == 1);
		REQUIRE(func->execOutputs()[0] == "eout");

		// check exit nodes
		REQUIRE(exitNode1->type().execOutputs().size() == 0);
		REQUIRE(exitNode1->type().execInputs().size() == 1);
		REQUIRE(exitNode1->type().execInputs()[0] == "eout");

		REQUIRE(exitNode2->type().execOutputs().size() == 0);
		REQUIRE(exitNode2->type().execInputs().size() == 1);
		REQUIRE(exitNode2->type().execInputs()[0] == "eout");

		res = connectExec(*setNode, 0, *exitNode1, 0);
		REQUIRE(!!res);

		res = connectExec(*setNode2, 0, *exitNode2, 0);
		REQUIRE(!!res);

		// check connection
		REQUIRE(exitNode1->inputExecConnections[0].size() == 1);
		REQUIRE(exitNode1->inputExecConnections[0][0].first == setNode);
		REQUIRE(exitNode1->inputExecConnections[0][0].second == 0);

		REQUIRE(exitNode2->inputExecConnections[0].size() == 1);
		REQUIRE(exitNode2->inputExecConnections[0][0].first == setNode2);
		REQUIRE(exitNode2->inputExecConnections[0][0].second == 0);

		WHEN("We add another after that") {
			func->addExecOutput("eout2", 1);

			REQUIRE(func->execOutputs().size() == 2);
			REQUIRE(func->execOutputs()[0] == "eout");
			REQUIRE(func->execOutputs()[1] == "eout2");

			// check exit nodes
			REQUIRE(exitNode1->type().execOutputs().size() == 0);
			REQUIRE(exitNode1->type().execInputs().size() == 2);
			REQUIRE(exitNode1->type().execInputs()[0] == "eout");
			REQUIRE(exitNode1->type().execInputs()[1] == "eout2");

			REQUIRE(exitNode2->type().execOutputs().size() == 0);
			REQUIRE(exitNode2->type().execInputs().size() == 2);
			REQUIRE(exitNode2->type().execInputs()[0] == "eout");
			REQUIRE(exitNode2->type().execInputs()[1] == "eout2");
		}

		WHEN("We add another before that") {
			func->addExecOutput("eout2", 0);

			REQUIRE(func->execOutputs().size() == 2);
			REQUIRE(func->execOutputs()[0] == "eout2");
			REQUIRE(func->execOutputs()[1] == "eout");

			// check exit nodes
			REQUIRE(exitNode1->type().execOutputs().size() == 0);
			REQUIRE(exitNode1->type().execInputs().size() == 2);
			REQUIRE(exitNode1->type().execInputs()[0] == "eout2");
			REQUIRE(exitNode1->type().execInputs()[1] == "eout");

			REQUIRE(exitNode2->type().execOutputs().size() == 0);
			REQUIRE(exitNode2->type().execInputs().size() == 2);
			REQUIRE(exitNode2->type().execInputs()[0] == "eout2");
			REQUIRE(exitNode2->type().execInputs()[1] == "eout");
		}

		WHEN("We rename it") {
			func->renameExecOutput(0, "einrenamed");

			REQUIRE(func->execOutputs().size() == 1);
			REQUIRE(func->execOutputs()[0] == "einrenamed");

			// check exit nodes
			REQUIRE(exitNode1->type().execOutputs().size() == 0);
			REQUIRE(exitNode1->type().execInputs().size() == 1);
			REQUIRE(exitNode1->type().execInputs()[0] == "einrenamed");

			REQUIRE(exitNode2->type().execOutputs().size() == 0);
			REQUIRE(exitNode2->type().execInputs().size() == 1);
			REQUIRE(exitNode2->type().execInputs()[0] == "einrenamed");
		}

		WHEN("We rename it using the wrong index") {
			func->renameExecOutput(1, "param1renamed");

			// everything should remain the same
			REQUIRE(func->execOutputs().size() == 1);
			REQUIRE(func->execOutputs()[0] == "eout");

			// check exit nodes
			REQUIRE(exitNode1->type().execOutputs().size() == 0);
			REQUIRE(exitNode1->type().execInputs().size() == 1);
			REQUIRE(exitNode1->type().execInputs()[0] == "eout");

			REQUIRE(exitNode2->type().execOutputs().size() == 0);
			REQUIRE(exitNode2->type().execInputs().size() == 1);
			REQUIRE(exitNode2->type().execInputs()[0] == "eout");

			// check connection
			REQUIRE(exitNode1->inputExecConnections[0].size() == 1);
			REQUIRE(exitNode1->inputExecConnections[0][0].first == setNode);
			REQUIRE(exitNode1->inputExecConnections[0][0].second == 0);

			REQUIRE(exitNode2->inputExecConnections[0].size() == 1);
			REQUIRE(exitNode2->inputExecConnections[0][0].first == setNode2);
			REQUIRE(exitNode2->inputExecConnections[0][0].second == 0);
		}
	}
}
