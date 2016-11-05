#include <catch.hpp>

#include <chig/Context.hpp>
#include <chig/GraphFunction.hpp>
#include <chig/LangModule.hpp>

#include <fstream>

using namespace chig;

TEST_CASE("Read json", "[json]")
{
	GIVEN("A default constructed Context object and a lang module") {
		
		Context c;
		c.addModule(std::make_unique<LangModule>(c));

		WHEN("Helloworld.chig is loaded from file and we load a graph function from it") {
			
			std::ifstream t("helloworld.chig");
			assert(t);
			std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
			
			
			auto data = nlohmann::json::parse(str);

			auto graph = GraphFunction::fromJSON(c, data["graphs"][0]);
			
			THEN("All the stuff defined in helloworld will work") {
				REQUIRE(graph.nodes.size() == 2);
				REQUIRE(graph.nodes[0]->type->module == "lang");
				REQUIRE(graph.nodes[0]->type->name == "entry");
				REQUIRE(graph.nodes[0]->type->dataOutputs.size() == 1);
				REQUIRE(c.stringifyType(graph.nodes[0]->type->dataOutputs[0].first) == "i32");
				REQUIRE(graph.nodes[0]->type->dataOutputs[0].second == "input");
                
				REQUIRE(graph.nodes[0]->outputExecConnections.size() == 1);
				REQUIRE(graph.nodes[0]->outputExecConnections[0].first == graph.nodes[1].get());
				REQUIRE(graph.nodes[0]->outputExecConnections[0].second == 0);
                
				REQUIRE(graph.nodes[0]->outputDataConnections.size() == 1);
				REQUIRE(graph.nodes[0]->outputDataConnections[0].first == graph.nodes[1].get());
				REQUIRE(graph.nodes[0]->outputDataConnections[0].second == 0);
                
				REQUIRE(graph.nodes[1]->type->module == "lang");
				REQUIRE(graph.nodes[1]->type->name == "exit");
                
				REQUIRE(graph.nodes[1]->inputDataConnections.size() == 1);
				REQUIRE(graph.nodes[1]->inputDataConnections[0].first == graph.nodes[0].get());
				REQUIRE(graph.nodes[1]->inputDataConnections[0].second == 0);
                
				REQUIRE(graph.nodes[1]->inputExecConnections.size() == 1);
				REQUIRE(graph.nodes[1]->inputExecConnections[0].first == graph.nodes[0].get());
				REQUIRE(graph.nodes[1]->inputExecConnections[0].second == 0);
			}
			
		}
		
	}

}
