#include "catch.hpp"

#include <chi/Core/Context.hpp>
#include <chi/Core/GraphFunction.hpp>
#include <chi/Core/GraphModule.hpp>
#include <chi/Core/GraphStruct.hpp>
#include <chi/Core/LangModule.hpp>
#include <chi/Core/NodeInstance.hpp>
#include <chi/Core/NodeType.hpp>
#include <chi/Support/Result.hpp>

using namespace chi;
using namespace nlohmann;

TEST_CASE("Create and manipulate entry nodes in GraphFunctions", "") {
	Context c;
	Result  res;

	res = c.loadModule("lang");
	REQUIRE(!!res);

	auto& langMod = *c.langModule();

	auto mod      = c.newGraphModule("test/main");
	bool inserted = false;
	auto func     = mod->getOrCreateFunction("main", {{"Hello", langMod.typeFromName("i32")}},
	                                     {{"HelOut", langMod.typeFromName("i1")}}, {"In Exec"},
	                                     {"Out Exec"}, &inserted);
	REQUIRE(inserted == true);

	// it should be all good
	REQUIRE(func->name() == "main");
	REQUIRE(&func->context() == &c);
	REQUIRE(&func->module() == c.moduleByFullName("test/main"));
	REQUIRE(func->nodes().size() == 0);
	REQUIRE(func->entryNode() == nullptr);
	REQUIRE(func->dataInputs().size() == 1);
	REQUIRE((func->dataInputs()[0] == NamedDataType{"Hello", langMod.typeFromName("i32")}));
	REQUIRE(func->dataOutputs().size() == 1);
	REQUIRE((func->dataOutputs()[0] == NamedDataType{"HelOut", langMod.typeFromName("i1")}));
	REQUIRE(func->execInputs().size() == 1);
	REQUIRE(func->execInputs()[0] == "In Exec");
	REQUIRE(func->execOutputs().size() == 1);
	REQUIRE(func->execOutputs()[0] == "Out Exec");
	REQUIRE(func->localVariables().size() == 0);

	json correctEntryJson = R"({"data": [{"Hello": "lang:i32"}], "exec": ["In Exec"]})"_json;
	std::unique_ptr<NodeType> ty;
	res = func->createEntryNodeType(&ty);
	REQUIRE(!!res);
	REQUIRE(ty != nullptr);
	REQUIRE(ty->name() == "entry");
	REQUIRE(&ty->module() == &langMod);
	REQUIRE(ty->toJSON() == correctEntryJson);

	WHEN("We add a correct entry node it should be a-ok") {
		std::unique_ptr<NodeType> ty;

		res += langMod.nodeTypeFromName("entry", correctEntryJson, &ty);
		REQUIRE(!!res);
		REQUIRE(ty != nullptr);

		NodeInstance* inst;
		auto          entryUUID = boost::uuids::random_generator()();
		res += func->insertNode(std::move(ty), 213.f, 123.f, entryUUID, &inst);
		REQUIRE(func->nodeByID(entryUUID) == inst);
		REQUIRE(func->nodeByID(boost::uuids::random_generator()()) == nullptr);
		REQUIRE(!!res);
		REQUIRE(inst != nullptr);

		REQUIRE(inst->id() == entryUUID);
		REQUIRE(inst->x() == 213.f);
		REQUIRE(inst->y() == 123.f);
		REQUIRE(inst->type().qualifiedName() == "lang:entry");
		REQUIRE(inst->type().toJSON() == correctEntryJson);
		REQUIRE(&inst->function() == func);
		REQUIRE(&inst->module() == mod);

		REQUIRE(func->entryNode() == inst);
	}

	WHEN("We add a correct entry node with getOrInsertEntryNode it should be a-ok") {
		NodeInstance* inst;
		auto          entryUUID = boost::uuids::random_generator()();
		res                     = func->getOrInsertEntryNode(213.f, 123.f, entryUUID, &inst);
		REQUIRE(!!res);
		REQUIRE(inst != nullptr);

		REQUIRE(inst->id() == entryUUID);
		REQUIRE(inst->x() == 213.f);
		REQUIRE(inst->y() == 123.f);
		REQUIRE(inst->type().qualifiedName() == "lang:entry");
		REQUIRE(inst->type().toJSON() == correctEntryJson);

		REQUIRE(func->entryNode() == inst);
	}

	WHEN("We add a correct node with the other insertNode function it should be a-ok") {
		NodeInstance* inst;
		auto          entryUUID = boost::uuids::random_generator()();
		res = func->insertNode("lang", "entry", correctEntryJson, 213.f, 123.f, entryUUID, &inst);
		REQUIRE(!!res);
		REQUIRE(inst != nullptr);
		REQUIRE(func->nodes().size() == 1);
		REQUIRE(func->nodes()[entryUUID].get() == inst);

		REQUIRE(inst->id() == entryUUID);
		REQUIRE(inst->x() == 213.f);
		REQUIRE(inst->y() == 123.f);
		REQUIRE(inst->type().qualifiedName() == "lang:entry");
		REQUIRE(inst->type().toJSON() == correctEntryJson);

		REQUIRE(func->entryNode() == inst);

		WHEN("We delete that node the count should go back down") {
			res = func->removeNode(*inst);
			REQUIRE(!!res);

			REQUIRE(func->nodes().size() == 0);
			REQUIRE(func->entryNode() == nullptr);
		}

		WHEN("We add another entry node, entryNode should fail") {
			NodeInstance* inst2;
			auto          entryUUID = boost::uuids::random_generator()();
			res = func->insertNode("lang", "entry", correctEntryJson, 213.f, 123.f, entryUUID,
			                       &inst2);
			REQUIRE(!!res);

			REQUIRE(inst2 != inst);
			REQUIRE(func->entryNode() == nullptr);
		}

		WHEN("We try to add another entry node with getOrInsertEntryNode") {
			NodeInstance* inst2;
			auto          entryUUID = boost::uuids::random_generator()();
			res                     = func->getOrInsertEntryNode(0.f, 0.f, entryUUID, &inst2);

			REQUIRE(inst2 == inst);
		}

		WHEN("We add another entry node with the same ID, insertNode should fail") {
			NodeInstance* inst2;
			res = func->insertNode("lang", "entry", correctEntryJson, 213.f, 123.f, entryUUID,
			                       &inst2);
			REQUIRE(!res);
			REQUIRE(res.result_json[0]["errorcode"] == "E47");
		}
	}

	auto checkInvalidEntry = [&](json obj) {
		NodeInstance* inst;
		auto          entryUUID = boost::uuids::random_generator()();
		res = func->insertNode("lang", "entry", obj, 213.f, 123.f, entryUUID, &inst);

		REQUIRE(!!res);
		REQUIRE(inst != nullptr);
		REQUIRE(func->nodes().size() == 1);
		REQUIRE(func->nodes()[entryUUID].get() == inst);

		REQUIRE(inst->id() == entryUUID);
		REQUIRE(inst->x() == 213.f);
		REQUIRE(inst->y() == 123.f);
		REQUIRE(inst->type().qualifiedName() == "lang:entry");
		REQUIRE_FALSE(inst->type().toJSON() == correctEntryJson);

		REQUIRE(func->entryNode() == nullptr);
	};

	WHEN("We add an invalid entry node entryNode should fail") { checkInvalidEntry({}); }
	WHEN("We add an invalid entry with the wrong name for the data parameter") {
		checkInvalidEntry(R"({"data": [{"Helloo": "lang:i32"}], "exec": ["In Exec"]})"_json);
	}
	WHEN("We add an invalid entry with the wrong name for the exec paramater") {
		checkInvalidEntry(R"({"data": [{"Hello": "lang:i32"}], "exec": ["In Exec wrong"]})"_json);
	}
	WHEN("We add an invalid entry with the wrong amount of data inputs") {
		checkInvalidEntry(
		    R"({"data": [{"Hello": "lang:i32"}, {"Hello": "lang:i1"}], "exec": ["In Exec"]})"_json);
	}
	WHEN("We add an invalid entry with the wrong amount of exec inputs") {
		checkInvalidEntry(R"({"data": [{"Hello": "lang:i32"}], "exec": ["In Exec", "Tada"]})"_json);
	}
}
