#include <catch.hpp>

#include <chig/Context.hpp>
#include <chig/DataType.hpp>
#include <chig/LangModule.hpp>
#include <chig/NodeType.hpp>

using namespace chig;

TEST_CASE("LangModule", "[module]") {
	GIVEN("A context with LangModule in it") {
		Context c;
		c.loadModule("lang");
		ChigModule* mod = c.moduleByName("lang");

		THEN("We try to get associated types with correct parameters, it works") {
			DataType test;
			Result   res;

			res = c.typeFromModule("lang", "i32", &test);
			REQUIRE(!!res);
			REQUIRE(test.llvmType() == llvm::IntegerType::getInt32Ty(c.llvmContext()));
			REQUIRE(&test.module() == mod);
			REQUIRE(test.unqualifiedName() == "i32");
			REQUIRE(test.qualifiedName() == "lang:i32");

			res = c.typeFromModule("lang", "i8*", &test);
			REQUIRE(!!res);
			REQUIRE(test.llvmType() == llvm::IntegerType::getInt8PtrTy(c.llvmContext()));
			REQUIRE(&test.module() == mod);
			REQUIRE(test.unqualifiedName() == "i8*");
			REQUIRE(test.qualifiedName() == "lang:i8*");

			res = c.typeFromModule("lang", "float", &test);
			REQUIRE(!!res);
			REQUIRE(test.llvmType() == llvm::IntegerType::getFloatTy(c.llvmContext()));
			REQUIRE(&test.module() == mod);
			REQUIRE(test.unqualifiedName() == "float");
			REQUIRE(test.qualifiedName() == "lang:float");
		}

		THEN(
		    "We try to get associated types with incorrect parameters, it returns the correct "
		    "errors") {
			DataType test;
			Result   res;

			res = c.typeFromModule("lang", "i32a", &test);
			REQUIRE(!res);
			REQUIRE(res.result_json[0]["errorcode"] == "E37");

			res = c.typeFromModule("lang", "i32*a", &test);
			REQUIRE(!res);
			REQUIRE(res.result_json[0]["errorcode"] == "E37");

			res = c.typeFromModule("lang", "*i32**", &test);
			REQUIRE(!res);
			REQUIRE(res.result_json[0]["errorcode"] == "E37");

			res = c.typeFromModule("lang", "&i8", &test);
			REQUIRE(!res);
			REQUIRE(res.result_json[0]["errorcode"] == "E37");

			res = c.typeFromModule("lang", "pq", &test);
			REQUIRE(!res);
			REQUIRE(res.result_json[0]["errorcode"] == "E37");
		}

		WHEN("We try to get if node") {
			Result                    res;
			std::unique_ptr<NodeType> ifNode = nullptr;
			res                              = c.nodeTypeFromModule("lang", "if", {}, &ifNode);
			REQUIRE(!!res);

			THEN("It should be totally valid") {
				REQUIRE(ifNode != nullptr);
				REQUIRE(ifNode->execInputs().size() == 1);
				REQUIRE(ifNode->execOutputs().size() == 2);
				REQUIRE(ifNode->dataInputs().size() == 1);
				REQUIRE(ifNode->dataOutputs().size() == 0);

				// make sure it is actually a if
				REQUIRE(ifNode->name() == "if");
			}

			WHEN("We clone it") {
				std::unique_ptr<NodeType> clone = ifNode->clone();

				THEN("The clone will be valid") {
					REQUIRE(ifNode != nullptr);
					REQUIRE(ifNode->execInputs().size() == 1);
					REQUIRE(ifNode->execOutputs().size() == 2);
					REQUIRE(ifNode->dataInputs().size() == 1);
					REQUIRE(ifNode->dataOutputs().size() == 0);

					// make sure it is actually a if
					REQUIRE(ifNode->name() == "if");
				}
			}
		}

		WHEN("We try to get entry node") {
			Result res;

			std::unique_ptr<NodeType> entryNode = nullptr;

			res = c.nodeTypeFromModule(
			    "lang", "entry",
			    nlohmann::json::parse(
			        R"end( { "data": [{"hello": "lang:i32"}, {"hello2": "lang:i8*"}], "exec": [""]  } )end"),
			    &entryNode);
			REQUIRE(!!res);

			THEN("It should be totally valid") {
				REQUIRE(entryNode != nullptr);
				REQUIRE(entryNode->execInputs().size() == 0);
				REQUIRE(entryNode->execOutputs().size() == 1);
				REQUIRE(entryNode->dataInputs().size() == 0);
				REQUIRE(entryNode->dataOutputs().size() == 2);

				// make sure it is actually an entry
				REQUIRE(entryNode->name() == "entry");
			}

			WHEN("We clone it") {
				std::unique_ptr<NodeType> clone = entryNode->clone();

				THEN("The clone will be valid") {
					REQUIRE(clone != nullptr);
					REQUIRE(clone->execInputs().size() == 0);
					REQUIRE(clone->execOutputs().size() == 1);
					REQUIRE(clone->dataInputs().size() == 0);
					REQUIRE(clone->dataOutputs().size() == 2);

					// make sure it is actually an entry
					REQUIRE(clone->name() == "entry");
				}
			}
		}
	}
}
