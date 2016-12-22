#include <catch.hpp>

#include <chig/Context.hpp>
#include <chig/LangModule.hpp>

#include <llvm/IR/DerivedTypes.h>

#include <chig/Config.hpp>

using namespace chig;
namespace fs = boost::filesystem;

TEST_CASE("Contexts can be created and modules can be added to them", "[Context]")
{
	GIVEN("A default constructed context")
	{
		Context c{};
		Result res;

		THEN("It resolves workspace paths correctly")
		{
			fs::path workspaceDir = fs::path(CHIG_TEST_DIR) / "codegen" / "workspace";

			REQUIRE(workspaceFromChildPath(workspaceDir) == workspaceDir);
			REQUIRE(workspaceFromChildPath(workspaceDir / "src") == workspaceDir);
			REQUIRE(workspaceFromChildPath(workspaceDir / "src" / "github.com") == workspaceDir);
			REQUIRE(workspaceFromChildPath("/").empty());
		}

		THEN("There will be no modules in it") { REQUIRE(c.numModules() == 0); }
		THEN("stringifyType return proper strings")
		{
			REQUIRE(stringifyLLVMType(llvm::IntegerType::getInt32Ty(c.llvmContext())) == "i32");
			REQUIRE(stringifyLLVMType(llvm::IntegerType::getInt1Ty(c.llvmContext())) == "i1");
			REQUIRE(stringifyLLVMType(llvm::IntegerType::getInt32PtrTy(c.llvmContext())) == "i32*");
			REQUIRE(stringifyLLVMType(llvm::IntegerType::getInt8Ty(c.llvmContext())) == "i8");
		}

		WHEN("A LangModule is created and added")
		{
			c.addModule(std::make_unique<LangModule>(c));

			WHEN("Lang module is added again")
			{
				bool added = c.addModule(std::make_unique<LangModule>(c));

				THEN("It have not been added")
				{
					// TODO: different E number
					REQUIRE(added == false);
				}
			}

			THEN("There should be 1 module in c") { REQUIRE(c.numModules() == 1); }
			THEN("Getting the lang module from c should work")
			{
				REQUIRE(c.moduleByName("lang") != nullptr);
				REQUIRE(c.moduleByName("lang")->name() == "lang");
			}

			THEN("Getting other modules should fail")
			{
				REQUIRE(c.moduleByName("qwerty") == nullptr);
				REQUIRE(c.moduleByName("") == nullptr);
				REQUIRE(c.moduleByName("lang/hello") == nullptr);
			}

			THEN("Getting the lang module from c should work via moduleByFullName")
			{
				REQUIRE(c.moduleByFullName("lang") != nullptr);
				REQUIRE(c.moduleByFullName("lang")->name() == "lang");
			}

			THEN("Getting other modules should fail via moduleByFullName")
			{
				REQUIRE(c.moduleByFullName("qwerty") == nullptr);
				REQUIRE(c.moduleByFullName("") == nullptr);
				REQUIRE(c.moduleByFullName("lang/hello") == nullptr);
			}

			THEN("getNodeType should work for basic types")
			{
				std::unique_ptr<NodeType> ty;
				res = c.nodeTypeFromModule("lang", "if", {}, &ty);
				REQUIRE(!!res);
			}

			THEN("getNodeType should fail for unknown modules and types with correct ec")
			{
				std::unique_ptr<NodeType> ty;
				res = c.nodeTypeFromModule("lan", "if", {}, &ty);
				REQUIRE(!res);
				REQUIRE(res.result_json[0]["errorcode"] == "E36");

				res = c.nodeTypeFromModule("lang", "eef", {}, &ty);
				REQUIRE(!res);
				REQUIRE(res.result_json[0]["errorcode"] == "E37");
			}

			THEN("getType should work for basic types")
			{
				auto checkTy = [&](gsl::cstring_span<> ty) {
					DataType chigty;
					res = c.typeFromModule("lang", ty, &chigty);
					REQUIRE(stringifyLLVMType(chigty.llvmType()) == ty);
				};

				checkTy("i32");
				checkTy("i1");
				checkTy("i32*");
				checkTy("i32**");
				checkTy("double");
			}

			THEN("getType should fail for incorrect modules and types")
			{
				DataType ty;
				res = c.typeFromModule("lang", "iiint", &ty);
				REQUIRE(!res);
				REQUIRE(res.result_json[0]["errorcode"] == "E37");

				res = c.typeFromModule("lag", "i8", &ty);
				REQUIRE(!res);
				REQUIRE(res.result_json[0]["errorcode"] == "E36");
			}
		}

		THEN("Load module should fail") { 
            REQUIRE(!c.loadModule("github.com/hello")); 
            REQUIRE(!c.loadModule(""));
        }
	}

	GIVEN("A context constructed with a workspace") {}
}
