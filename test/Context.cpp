#include <catch.hpp>

#include <chig/Context.hpp>
#include <chig/LangModule.hpp>

#include <llvm/IR/DerivedTypes.h>

using namespace chig;

TEST_CASE("Contexts can be created and modules can be added to them", "[Context]") {
	
	GIVEN("A default constructed context") {
		Context c;
	
		THEN("Adding an empty module throws") {
			REQUIRE_THROWS(c.addModule(nullptr));
		}
		
		THEN("There will be no modules in it") {
			REQUIRE(c.modules.size() == 0);
		}
		
		THEN("stringifyType return proper strings") {
			REQUIRE(c.stringifyType(llvm::IntegerType::getInt32Ty(c.context)) == "i32");
			REQUIRE(c.stringifyType(llvm::IntegerType::getInt1Ty(c.context)) == "i1");
			REQUIRE(c.stringifyType(llvm::IntegerType::getInt32PtrTy(c.context)) == "i32*");
			REQUIRE(c.stringifyType(llvm::IntegerType::getInt8Ty(c.context)) == "i8");
		}
		
		WHEN("A LangModule is created and added") {
			c.addModule(std::make_unique<LangModule>(c));
			
			THEN("There should be 1 module in c") {
				REQUIRE(c.modules.size() == 1);
			}
			
			THEN("Getting the lang module from c should work") {
				REQUIRE(c.getModuleByName("lang") != nullptr);
				REQUIRE(c.getModuleByName("lang")->name == "lang");
			}
			
			THEN("Passing nullptr to getModuleByName should fail with nullptr") {
				REQUIRE_THROWS(c.getModuleByName(nullptr));
			}
			
			THEN("getNodeType should work for basic types") {
				REQUIRE(c.getNodeType("lang", "if", {}));
			}
			
			THEN("getNodeType should fail for unknown modules and types") {
				REQUIRE(c.getNodeType("lan", "if") == nullptr);
				REQUIRE(c.getNodeType("lang", "eef") == nullptr);
			}
			
			THEN("getType should work for basic types") {
				CHECK(c.stringifyType(c.getType("lang", "i32")) == "i32");
				CHECK(c.stringifyType(c.getType("lang", "i1")) == "i1");
				CHECK(c.stringifyType(c.getType("lang", "i32*")) == "i32*");
				CHECK(c.stringifyType(c.getType("lang", "i32**")) == "i32**");
				REQUIRE(c.stringifyType(c.getType("lang", "i8")) == "i8");
			}
			
			THEN("getType should fail for incorrect modules and types") {
				REQUIRE(c.getType("lan", "i32") == nullptr);
				REQUIRE(c.getType("lang", "ii32") == nullptr);
			}
		}
	}
	

	
}
