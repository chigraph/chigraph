#include <catch.hpp>

#include <chi/Context.hpp>
#include <chi/GraphFunction.hpp>
#include <chi/GraphModule.hpp>
#include <chi/GraphStruct.hpp>
#include <chi/LangModule.hpp>
#include <chi/NodeInstance.hpp>
#include <chi/Result.hpp>

using namespace chi;
using namespace nlohmann;

TEST_CASE("Create and manipulate exit nodes in GraphFunctions", "") {
	Context c;
	Result  res;

	res = c.loadModule("lang");
	REQUIRE(!!res);

	auto& langMod = *c.langModule();

	auto mod      = c.newGraphModule("test/main");
	bool inserted = false;

	WHEN("We create a function with a few arguments") {
		auto func = mod->getOrCreateFunction("main", {{"Hello", langMod.typeFromName("i32")}},
		                                     {{"HelOut", langMod.typeFromName("i1")}}, {"In Exec"},
		                                     {"Out Exec"}, &inserted);
		REQUIRE(inserted == true);

		std::unique_ptr<NodeType> ty;
		res = func->createExitNodeType(&ty);
		REQUIRE(ty != nullptr);
		REQUIRE(ty->execInputs().size() == 1);
		REQUIRE(ty->execInputs()[0] == "Out Exec");
		REQUIRE(ty->execOutputs().size() == 0);
		REQUIRE(ty->dataInputs().size() == 1);
		REQUIRE((ty->dataInputs()[0] == NamedDataType{"HelOut", langMod.typeFromName("i1")}));
		REQUIRE(ty->dataOutputs().size() == 0);
		REQUIRE(ty->toJSON() == R"({"data": [{"HelOut": "lang:i1"}], "exec": ["Out Exec"]})"_json);
	}
	WHEN("We create an entirely default function") {
		auto func = mod->getOrCreateFunction("main", {}, {}, {}, {}, &inserted);
		REQUIRE(inserted == true);

		std::unique_ptr<NodeType> ty;
		res = func->createExitNodeType(&ty);
		REQUIRE(ty != nullptr);
		REQUIRE(ty->execInputs().size() == 0);
		REQUIRE(ty->execOutputs().size() == 0);
		REQUIRE(ty->dataInputs().size() == 0);
		REQUIRE(ty->dataOutputs().size() == 0);
		REQUIRE(ty->toJSON() == R"({"data": [], "exec": []})"_json);
	}
}
