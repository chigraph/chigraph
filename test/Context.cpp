#include <catch.hpp>

#include <chig/Context.hpp>
#include <chig/LangModule.hpp>

#include <llvm/IR/DerivedTypes.h>

using namespace chig;

TEST_CASE("Contexts can be created and modules can be added to them", "[Context]")
{
	GIVEN("A default constructed context")
	{
		Context c;
		Result res;

		THEN("There will be no modules in it") { REQUIRE(c.modules.size() == 0); }
		THEN("stringifyType return proper strings")
		{
			REQUIRE(c.stringifyType(llvm::IntegerType::getInt32Ty(c.context)) == "i32");
			REQUIRE(c.stringifyType(llvm::IntegerType::getInt1Ty(c.context)) == "i1");
			REQUIRE(c.stringifyType(llvm::IntegerType::getInt32PtrTy(c.context)) == "i32*");
			REQUIRE(c.stringifyType(llvm::IntegerType::getInt8Ty(c.context)) == "i8");
		}

		WHEN("A LangModule is created and added")
		{
			c.addModule(std::make_unique<LangModule>(c));

			THEN("There should be 1 module in c") { REQUIRE(c.modules.size() == 1); }
			THEN("Getting the lang module from c should work")
			{
				REQUIRE(c.getModuleByName("lang") != nullptr);
				REQUIRE(c.getModuleByName("lang")->name == "lang");
			}

			THEN("getNodeType should work for basic types")
			{
				std::unique_ptr<NodeType> ty;
				res = c.getNodeType("lang", "if", {}, &ty);
				REQUIRE(!!res);
			}

			THEN("getNodeType should fail for unknown modules and types with correct ec")
			{
				std::unique_ptr<NodeType> ty;
				res = c.getNodeType("lan", "if", {}, &ty);
				REQUIRE(!res);
				REQUIRE(res.result_json["errorcode"] == "E20");

				res = c.getNodeType("lang", "eef", {}, &ty);
				REQUIRE(!res);
				REQUIRE(res.result_json["errorcode"] == "E21");
			}

			THEN("getType should work for basic types")
			{
				auto checkTy = [&](const char* ty) {
					llvm::Type* llty;
					res = c.getType("lang", ty, &llty);
					REQUIRE(c.stringifyType(llty) == ty);
				};

				checkTy("i32");
				checkTy("i1");
				checkTy("i32*");
				checkTy("i32**");
				checkTy("double");
			}

			THEN("getType should fail for incorrect modules and types")
			{
				llvm::Type* ty;
				res = c.getType("lang", "iiint", &ty);
				REQUIRE(!res);
				REQUIRE(res.result_json["errorcode"] == "E21");

				res = c.getType("lag", "i8", &ty);
				REQUIRE(!res);
				REQUIRE(res.result_json["errorcode"] == "E20");
			}
		}
	}
}
