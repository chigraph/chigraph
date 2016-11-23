#include <catch.hpp>

#include <chig/Context.hpp>
#include <chig/LangModule.hpp>

using namespace chig;

TEST_CASE("LangModule", "[module]")
{
	GIVEN("A context with LangModule in it")
	{
		Context c;
		c.addModule(std::make_unique<LangModule>(c));

		THEN("We try to get associated types with correct parameters, it works")
		{
			llvm::Type* test;
			Result res;

			res = c.getType("lang", "i32", &test);
			REQUIRE(!!res);
			REQUIRE(test == llvm::IntegerType::getInt32Ty(c.llcontext));

			res = c.getType("lang", "i32*", &test);
			REQUIRE(!!res);
			REQUIRE(test == llvm::IntegerType::getInt32PtrTy(c.llcontext));

			res = c.getType("lang", "i32**", &test);
			REQUIRE(!!res);
			REQUIRE(test == llvm::PointerType::get(llvm::IntegerType::getInt32PtrTy(c.llcontext), 0));

			res = c.getType("lang", "i8", &test);
			REQUIRE(!!res);
			REQUIRE(test == llvm::IntegerType::getInt8Ty(c.llcontext));

			res = c.getType("lang", "double", &test);
			REQUIRE(!!res);
			REQUIRE(test == llvm::Type::getDoubleTy(c.llcontext));
		}

		THEN(
			"We try to get associated types with incorrect parameters, it returns the correct "
			"errors")
		{
			llvm::Type* test;
			Result res;

			res = c.getType("lang", "i32a", &test);
			REQUIRE(!res);
			REQUIRE(res.result_json[0]["errorcode"] == "E37");

			res = c.getType("lang", "i32*a", &test);
			REQUIRE(!res);
			REQUIRE(res.result_json[0]["errorcode"] == "E37");

			res = c.getType("lang", "*i32**", &test);
			REQUIRE(!res);
			REQUIRE(res.result_json[0]["errorcode"] == "E37");

			res = c.getType("lang", "&i8", &test);
			REQUIRE(!res);
			REQUIRE(res.result_json[0]["errorcode"] == "E37");

			res = c.getType("lang", "pq", &test);
			REQUIRE(!res);
			REQUIRE(res.result_json[0]["errorcode"] == "E37");
		}

		WHEN("We try to get if node")
		{
			Result res;
			std::unique_ptr<NodeType> ifNode = nullptr;
			res = c.getNodeType("lang", "if", {}, &ifNode);
			REQUIRE(!!res);

			THEN("It should be totally valid")
			{
				REQUIRE(ifNode != nullptr);
				REQUIRE(ifNode->execInputs.size() == 1);
				REQUIRE(ifNode->execOutputs.size() == 2);
				REQUIRE(ifNode->dataInputs.size() == 1);
				REQUIRE(ifNode->dataOutputs.size() == 0);

				// make sure it is actually a if
				REQUIRE(dynamic_cast<IfNodeType*>(ifNode.get()) != nullptr);
			}

			WHEN("We clone it")
			{
				std::unique_ptr<NodeType> clone = ifNode->clone();

				THEN("The clone will be valid")
				{
					REQUIRE(ifNode != nullptr);
					REQUIRE(ifNode->execInputs.size() == 1);
					REQUIRE(ifNode->execOutputs.size() == 2);
					REQUIRE(ifNode->dataInputs.size() == 1);
					REQUIRE(ifNode->dataOutputs.size() == 0);

					// make sure it is actually a if
					REQUIRE(dynamic_cast<IfNodeType*>(ifNode.get()) != nullptr);
				}
			}
		}

		WHEN("We try to get entry node")
		{
			Result res;

			std::unique_ptr<NodeType> entryNode = nullptr;

			res = c.getNodeType("lang", "entry",
				nlohmann::json::parse(R"end(   {"hello": "i32", "hello2": "i32*"}   )end"),
				&entryNode);
			REQUIRE(!!res);

			THEN("It should be totally valid")
			{
				REQUIRE(entryNode != nullptr);
				REQUIRE(entryNode->execInputs.size() == 0);
				REQUIRE(entryNode->execOutputs.size() == 1);
				REQUIRE(entryNode->dataInputs.size() == 0);
				REQUIRE(entryNode->dataOutputs.size() == 2);

				// make sure it is actually an entry
				REQUIRE(dynamic_cast<EntryNodeType*>(entryNode.get()) != nullptr);
			}

			WHEN("We clone it")
			{
				std::unique_ptr<NodeType> clone = entryNode->clone();

				THEN("The clone will be valid")
				{
					REQUIRE(entryNode != nullptr);
					REQUIRE(entryNode->execInputs.size() == 0);
					REQUIRE(entryNode->execOutputs.size() == 1);
					REQUIRE(entryNode->dataInputs.size() == 0);
					REQUIRE(entryNode->dataOutputs.size() == 2);

					// make sure it is actually an entry
					REQUIRE(dynamic_cast<EntryNodeType*>(entryNode.get()) != nullptr);
				}
			}
		}
	}
}
