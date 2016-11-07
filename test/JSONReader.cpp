#include <catch.hpp>

#include <chig/Context.hpp>
#include <chig/GraphFunction.hpp>
#include <chig/LangModule.hpp>

#include <fstream>

using namespace chig;
using namespace nlohmann;

TEST_CASE("Read json", "[json]")
{
	GIVEN("A default constructed Context object and a lang module") {
		
		Context c;
		c.addModule(std::make_unique<LangModule>(c));

		WHEN("We load some empty JSON then it should throw") {
			
			REQUIRE_THROWS(GraphFunction::fromJSON(c, json::parse("null")));
			
		}		
		
		WHEN("We load an empty JSON object then it should throw") {
			
			REQUIRE_THROWS(GraphFunction::fromJSON(c, json::parse("{}")));
			
		}
		
		WHEN("We load a non-function type") {
			
			auto inputJSON = R"ENDJSON(
				{
					"type": "notfunction"
				})ENDJSON"_json;
				
			REQUIRE_THROWS(GraphFunction::fromJSON(c, inputJSON)); 
			
		}
		
		WHEN("We have a function type but no nodes") {
			auto inputJSON = R"ENDJSON(
				{
					"type": "function"
				})ENDJSON"_json;
			
			REQUIRE_THROWS(GraphFunction::fromJSON(c, inputJSON)); 

		}
		WHEN("We have a function with nodes but nodes is null") {
			auto inputJSON = R"ENDJSON(
				{
					"type": "function",
					"nodes": null
				})ENDJSON"_json;
			
			REQUIRE_THROWS(GraphFunction::fromJSON(c, inputJSON)); 

		}
        WHEN("We have a function with nodes but no name") {
            auto inputJSON = R"ENDJSON(
                {
                    "type": "function",
                    "nodes": []
                })ENDJSON"_json;

            REQUIRE_THROWS(GraphFunction::fromJSON(c, inputJSON));

        }
        WHEN("We have a function with nodes and a name--this should work!") {
            auto inputJSON = R"ENDJSON(
                {
                    "type": "function",
                    "nodes": []
                })ENDJSON"_json;

            REQUIRE_THROWS(GraphFunction::fromJSON(c, inputJSON));

        }
		
	}

}
