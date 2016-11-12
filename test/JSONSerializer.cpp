#include "catch.hpp"

#include <chig/Context.hpp>
#include <chig/GraphFunction.hpp>
#include <chig/LangModule.hpp>

using namespace chig;
using namespace nlohmann;

TEST_CASE("JsonSerializer", "[json]")
{
	GIVEN("A default constructed Context with a LangModule and GraphFunction named hello")
	{
		Result res;
		Context c;
		c.addModule(std::make_unique<LangModule>(c));

		GraphFunction func(c, "hello");

		auto requireWorks = [&](nlohmann::json expected) {
			nlohmann::json ret;

			res = func.toJSON(&ret);

			REQUIRE(!!res);
			REQUIRE(ret == expected);

		};

		THEN("The JSON should be correct")
		{
			auto correctJSON = R"ENDJSON(
				{
					"type": "function",
					"name": "hello",
					"nodes": [],
					"connections": []
				}
			)ENDJSON"_json;

			requireWorks(correctJSON);
		}

		WHEN("We create some nodes and try to dump json")
		{
			std::vector<std::pair<llvm::Type*, std::string>> inputs = {
				{llvm::Type::getInt1Ty(c.context), "in1"}};

			auto entry = func.insertNode(std::make_unique<EntryNodeType>(c, inputs), 32, 32);

			THEN("The JSON should be correct")
			{
				auto correctJSON = R"ENDJSON(
					{
						"type": "function",
						"name": "hello",
						"nodes": [
							{
								"type": "lang:entry",
								"location": [32.0,32.0],
								"data": {
									"in1": "lang:i1"
								}
							}
						],
						"connections": []
					}
					)ENDJSON"_json;

				requireWorks(correctJSON);
			}

			WHEN("A lang:if is added")
			{
				std::unique_ptr<NodeType> ifType;
				res = c.getNodeType("lang", "if", {}, &ifType);
				REQUIRE(!!res);
				auto ifNode = func.insertNode(std::move(ifType), 44.f, 23.f);

				THEN("The JSON should be correct")
				{
					auto correctJSON = R"ENDJSON(
						{
							"type": "function",
							"name": "hello",
							"nodes": [
								{
									"type": "lang:entry",
									"location": [32.0,32.0],
									"data": {
										"in1": "lang:i1"
									}
								},
								{
									"type": "lang:if",
									"location": [44.0, 23.0],
									"data": null
								}
							],
							"connections": []
						}
						)ENDJSON"_json;

					requireWorks(correctJSON);
				}

				WHEN("We connect the entry to the ifNode exec")
				{
					connectExec(*entry, 0, *ifNode, 0);

					THEN("The JSON should be correct")
					{
						auto correctJSON = R"ENDJSON(
							{
								"type": "function",
								"name": "hello",
								"nodes": [
									{
										"type": "lang:entry",
										"location": [32.0,32.0],
										"data": {
											"in1": "lang:i1"
										}
									},
									{
										"type": "lang:if",
										"location": [44.0, 23.0],
										"data": null
									}
								],
								"connections": [
									{
										"type": "exec",
										"input": [0, 0],
										"output": [1, 0]
									}
								]
							})ENDJSON"_json;

						requireWorks(correctJSON);
					}

					WHEN("Connect the data")
					{
						res = connectData(*entry, 0, *ifNode, 0);
						
						REQUIRE(res.result_json == json::array());

						THEN("The JSON should be correct")
						{
							auto correctJSON = R"ENDJSON(
								{
								"type": "function",
								"name": "hello",
								"nodes": [
									{
									"type": "lang:entry",
									"location": [32.0,32.0],
									"data": {
										"in1": "lang:i1"
									}
									},
									{
										"type": "lang:if",
										"location": [44.0,23.0],
										"data": null
									}
								],
								"connections": [
									{
										"type": "exec",
										"input": [0,0],
										"output": [1,0]
									},
									{
										"type": "data",
										"input": [0,0],
										"output": [1,0]
									}
								]
								})ENDJSON"_json;

							requireWorks(correctJSON);
						}
					}
				}
			}
		}
	}
}
