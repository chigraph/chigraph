#include <catch.hpp>

#include <chig/Context.hpp>
#include <chig/LangModule.hpp>

using namespace chig;

TEST_CASE("LangModule", "[module]") {
	
	GIVEN("A context with LangModule in it") {
		Context c;
		c.addModule(std::make_unique<LangModule>(c));
		
		THEN("We try to get associated types with correct parameters, it works") {
			
			REQUIRE(c.getType("lang", "i32") == llvm::IntegerType::getInt32Ty(c.context));
			REQUIRE(c.getType("lang", "i32*") == llvm::IntegerType::getInt32PtrTy(c.context));
			REQUIRE(c.getType("lang", "i32**") == llvm::PointerType::get(llvm::IntegerType::getInt32PtrTy(c.context), 0));
			REQUIRE(c.getType("lang", "i8") == llvm::IntegerType::getInt8Ty(c.context));
			REQUIRE(c.getType("lang", "double") == llvm::Type::getDoubleTy(c.context));
			
		}
		
		THEN("We try to get associated types with incorrect parameters, it returns nullptr") {
			
			REQUIRE(c.getType("lang", "i32a") == nullptr);
			REQUIRE(c.getType("lang", "i32*a") == nullptr);
			REQUIRE(c.getType("lang", "*i32**") == nullptr);
			REQUIRE(c.getType("lang", "&i8") == nullptr);
			REQUIRE(c.getType("lang", "pq") == nullptr);
		}
		
		WHEN("We try to get if node") {
			std::unique_ptr<NodeType> ifNode = nullptr;
			REQUIRE_NOTHROW(ifNode = c.getNodeType("lang", "if", {}));
			
			THEN("It should be totally valid") {
				REQUIRE(ifNode != nullptr);
				REQUIRE(ifNode->execInputs.size() == 1);
				REQUIRE(ifNode->execOutputs.size() == 2);
				REQUIRE(ifNode->dataInputs.size() == 1);
				REQUIRE(ifNode->dataOutputs.size() == 0);
				
				// make sure it is actually a if
				REQUIRE(dynamic_cast<IfNodeType*>(ifNode.get()) != nullptr);
				
			}
			
			WHEN("We clone it") {
				std::unique_ptr<NodeType> clone = ifNode->clone();
				
				THEN("The clone will be valid") {
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
		
		WHEN("We try to get entry node") {
			std::unique_ptr<NodeType> entryNode = nullptr;
			REQUIRE_NOTHROW(entryNode = c.getNodeType("lang", "entry", nlohmann::json::parse(R"end(   {"hello": "i32", "hello2": "i32*"}   )end")));
			
			THEN("It should be totally valid") {
				REQUIRE(entryNode != nullptr);
				REQUIRE(entryNode->execInputs.size() == 0);
				REQUIRE(entryNode->execOutputs.size() == 1);
				REQUIRE(entryNode->dataInputs.size() == 0);
				REQUIRE(entryNode->dataOutputs.size() == 2);
				
				// make sure it is actually an entry
				REQUIRE(dynamic_cast<EntryNodeType*>(entryNode.get()) != nullptr);
				
			}
			
			WHEN("We clone it") {
				std::unique_ptr<NodeType> clone = entryNode->clone();
				
				THEN("The clone will be valid") {
					
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
