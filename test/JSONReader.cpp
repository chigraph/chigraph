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
			REQUIRE(res.result_json[0]["errorcode"] == ec);
		};

		auto requireWorks = [&](nlohmann::json obj) {
			std::unique_ptr<GraphFunction> func;
			res = GraphFunction::fromJSON(c, obj, &func);

			REQUIRE(!!res);

			return func;
		};


		WHEN("We have a function with nodes and a name, but no connections")
		{
			auto inputJSON = R"ENDJSON(
				{
					"type": "function",
					"name": "Hello",
					"nodes": []
				})ENDJSON"_json;

			requireErrs(inputJSON, "E35");
		}
	}
}
