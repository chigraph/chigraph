#include <catch.hpp>

#include <chig/Context.hpp>
#include <chig/GraphFunction.hpp>
#include <chig/LangModule.hpp>

#include <fstream>

using namespace chig;
using namespace nlohmann;

TEST_CASE("Read json", "[json]")
{
	GIVEN("A default constructed Context object and a lang module")
	{
		Result res;
		Context c;
		c.addModule(std::make_unique<LangModule>(c));

		auto requireErrs = [&](nlohmann::json obj, const char* ec) {
			std::unique_ptr<GraphFunction> func;
			res = GraphFunction::fromJSON(c, obj, &func);

			REQUIRE(!res);
			REQUIRE(res.result_json["errorcode"] == ec);
		};

		auto requireWorks = [&](nlohmann::json obj) {
			std::unique_ptr<GraphFunction> func;
			res = GraphFunction::fromJSON(c, obj, &func);

			REQUIRE(!!res);

			return func;
		};

		WHEN("We load some empty JSON then it should throw")
		{
			requireErrs(json::parse("null"), "E1");
		}

		WHEN("We load an empty JSON object then it should throw")
		{
			requireErrs(json::parse("{}"), "E2");
		}

		WHEN("We load a non-function type")
		{
			auto inputJSON = R"ENDJSON(
				{
					"type": "notfunction"
				})ENDJSON"_json;

			requireErrs(inputJSON, "E3");
		}

		WHEN("We have a function type but no nodes")
		{
			auto inputJSON = R"ENDJSON(
				{
					"type": "function"
				})ENDJSON"_json;

			requireErrs(inputJSON, "E4");
		}
		WHEN("We have a function with nodes but nodes is null")
		{
			auto inputJSON = R"ENDJSON(
				{
					"type": "function",
					"nodes": null
				})ENDJSON"_json;

			requireErrs(inputJSON, "E4");
		}
		WHEN("We have a function with nodes but no name")
		{
			auto inputJSON = R"ENDJSON(
				{
					"type": "function",
					"nodes": []
				})ENDJSON"_json;

			requireErrs(inputJSON, "E4");
		}

		WHEN("We have a function with nodes and a name it should work")
		{
			auto inputJSON = R"ENDJSON(
				{
					"type": "function",
					"name": "Hello",
					"nodes": []
				})ENDJSON"_json;

			auto obj = requireWorks(inputJSON);
			REQUIRE(obj->graphName == "Hello");
			REQUIRE(obj->nodes.size() == 0);
		}
	}
}
