#include <catch.hpp>

#include <chi/Context.hpp>
#include <chi/GraphFunction.hpp>
#include <chi/GraphModule.hpp>
#include <chi/GraphStruct.hpp>
#include <chi/LangModule.hpp>
#include <chi/NodeInstance.hpp>
#include <chi/NodeType.hpp>
#include <chi/Support/Result.hpp>

using namespace chi;
using namespace nlohmann;

TEST_CASE("Create and manipulate local varables in GraphFunctions", "") {
	Context c;
	Result  res;

	res += c.loadModule("lang");
	REQUIRE(!!res);

	auto& langMod = *c.langModule();

	auto mod  = c.newGraphModule("test/main");
	auto func = mod->getOrCreateFunction("main", {}, {}, {"In Exec"}, {"Out Exec"});

	REQUIRE(func->localVariables().size() == 0);
	REQUIRE_FALSE(func->localVariableFromName("var").valid());

	WHEN("We add a local variable everything should be OK") {
		bool inserted;
		auto ty = func->getOrCreateLocalVariable("var", langMod.typeFromName("i32"), &inserted);
		REQUIRE(ty.valid() == true);
		REQUIRE(inserted == true);
		REQUIRE(ty.name == "var");
		REQUIRE(ty.type == langMod.typeFromName("i32"));
		REQUIRE(func->localVariables().size() == 1);
		REQUIRE(func->localVariables()[0] == ty);
		REQUIRE(func->localVariableFromName("var") == ty);

		WHEN("We add some set and get nodes") {
			NodeInstance* setInst = nullptr;
			res = func->insertNode("test/main", "_set_var", "lang:i32", 0.f, 0.f, Uuid::random(),
			                       &setInst);
			REQUIRE(!!res);
			REQUIRE(setInst != nullptr);
			REQUIRE_FALSE(setInst->type().pure());
			REQUIRE(setInst->type().execInputs().size() == 1);
			REQUIRE(setInst->type().execOutputs().size() == 1);
			REQUIRE(setInst->type().dataInputs().size() == 1);
			REQUIRE(setInst->type().dataInputs()[0].type == langMod.typeFromName("i32"));
			REQUIRE(setInst->type().dataOutputs().size() == 0);

			NodeInstance* getInst = nullptr;
			res = func->insertNode("test/main", "_get_var", "lang:i32", 0.f, 0.f, Uuid::random(),
			                       &getInst);
			REQUIRE(!!res);
			REQUIRE(getInst != nullptr);
			REQUIRE(getInst->type().pure());
			REQUIRE(getInst->type().dataOutputs().size() == 1);
			REQUIRE(getInst->type().dataOutputs()[0].type == langMod.typeFromName("i32"));
			REQUIRE(getInst->type().dataInputs().size() == 0);

			REQUIRE(func->nodes().size() == 2);

			WHEN("We add another local var with the name var inserted should be false") {
				auto ty2 =
				    func->getOrCreateLocalVariable("var", langMod.typeFromName("i8*"), &inserted);
				REQUIRE(inserted == false);
				REQUIRE(ty2.name == "var");
				REQUIRE(ty2.type == langMod.typeFromName("i32"));
				REQUIRE(ty2 == ty);
			}

			WHEN("We remove that variable") {
				REQUIRE(func->removeLocalVariable("var"));
				REQUIRE(func->localVariables().size() == 0);
				REQUIRE_FALSE(func->localVariableFromName("var").valid());

				THEN("The get and set nodes should be deleted") {
					REQUIRE(func->nodes().size() == 0);
				}
			}

			WHEN("We we rename that variable") {
				func->renameLocalVariable("var", "thevar");

				// it should have retyped the nodes
				REQUIRE(func->nodes().size() == 2);

				REQUIRE(setInst->type().name() == "_set_thevar");
				REQUIRE(getInst->type().name() == "_get_thevar");
			}

			WHEN("We retype that variable") {
				func->retypeLocalVariable("var", langMod.typeFromName("i1"));

				// it should have retyped the nodes
				REQUIRE(func->nodes().size() == 2);

				REQUIRE(setInst->type().name() == "_set_var");
				REQUIRE(getInst->type().name() == "_get_var");

				REQUIRE_FALSE(setInst->type().pure());
				REQUIRE(setInst->type().execInputs().size() == 1);
				REQUIRE(setInst->type().execOutputs().size() == 1);
				REQUIRE(setInst->type().dataInputs().size() == 1);
				REQUIRE(setInst->type().dataInputs()[0].type == langMod.typeFromName("i1"));
				REQUIRE(setInst->type().dataOutputs().size() == 0);

				REQUIRE(getInst->type().pure());
				REQUIRE(getInst->type().dataOutputs().size() == 1);
				REQUIRE(getInst->type().dataOutputs()[0].type == langMod.typeFromName("i1"));
				REQUIRE(getInst->type().dataInputs().size() == 0);
			}

			WHEN("We remove the wrong name") {
				REQUIRE_FALSE(func->removeLocalVariable("notvar"));
				REQUIRE(func->localVariables().size() == 1);
				REQUIRE(func->localVariables()[0] == ty);
				REQUIRE(func->localVariableFromName("var") == ty);

				REQUIRE(func->nodes().size() == 2);
			}
		}
	}
}
