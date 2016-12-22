#include "catch.hpp"

#include <chig/Context.hpp>
#include <chig/GraphFunction.hpp>
#include <chig/JsonModule.hpp>
#include <chig/LangModule.hpp>
#include <chig/NodeInstance.hpp>

using namespace chig;
using namespace nlohmann;

TEST_CASE("JsonSerializer", "[json]")
{
	GIVEN("A default constructed Context with a LangModule and GraphFunction named hello")
	{
		Result res;
		Context c;
		REQUIRE(!!c.loadModule("lang"));
		LangModule* lmod = static_cast<LangModule*>(c.moduleByName("lang"));
        REQUIRE(lmod != nullptr);

		auto deps = std::vector<std::string>{"lang"};
		auto jmod = std::make_unique<JsonModule>(
			c, "main", gsl::span<std::string>{deps.data(),
						   static_cast<gsl::span<std::string>::index_type>(deps.size())});
		GraphFunction func(*jmod, "hello", {}, {});

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
					"nodes": {},
					"connections": [],
                    "inputs": [],
                    "outputs": []
				}
			)ENDJSON"_json;

			requireWorks(correctJSON);
		}

		WHEN("We create some nodes and try to dump json")
		{
			std::vector<std::pair<DataType, std::string>> inputs = {
				{lmod->typeFromName("i1"), "in1"}};

			std::unique_ptr<NodeType> toFill;
			Result res =
				c.nodeTypeFromModule("lang", "entry", R"([{"in1": "lang:i1"}])"_json, &toFill);
			REQUIRE(!!res);
			NodeInstance* entry;
			res += func.insertNode(std::move(toFill), 32, 32, "entry", &entry);
			REQUIRE(!!res);

			THEN("The JSON should be correct")
			{
				auto correctJSON = R"ENDJSON(
					{
						"type": "function",
						"name": "hello",
						"nodes": {
							"entry": {
								"type": "lang:entry",
								"location": [32.0,32.0],
								"data": [
									{"in1": "lang:i1"}
								]
							}
						},
						"connections": [],
                        "inputs": [],
                        "outputs": []
					}
					)ENDJSON"_json;

				requireWorks(correctJSON);
			}

			WHEN("A lang:if is added")
			{
				std::unique_ptr<NodeType> ifType;
				res = c.nodeTypeFromModule("lang", "if", {}, &ifType);
				REQUIRE(!!res);
				NodeInstance* ifNode;
				res += func.insertNode(std::move(ifType), 44.f, 23.f, "if", &ifNode);
				REQUIRE(!!res);

				THEN("The JSON should be correct")
				{
					auto correctJSON = R"ENDJSON(
						{
							"type": "function",
							"name": "hello",
							"nodes": {
								"entry": {
									"type": "lang:entry",
									"location": [32.0,32.0],
									"data": [
										{"in1": "lang:i1"}
									]
								},
								"if": {
									"type": "lang:if",
									"location": [44.0, 23.0],
									"data": null
								}
							},
							"connections": [],
                            "inputs": [],
                            "outputs": []
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
								"nodes": {
									"entry": {
										"type": "lang:entry",
										"location": [32.0,32.0],
										"data": [
											{"in1": "lang:i1"}
										]
									},
									"if": {
										"type": "lang:if",
										"location": [44.0, 23.0],
										"data": null
									}
								},
								"connections": [
									{
										"type": "exec",
										"input": ["entry",0],
										"output": ["if",0]
									}
								],
                                "inputs": [],
                                "outputs": []
							}
						)ENDJSON"_json;

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
								"nodes": {
									"entry": {
										"type": "lang:entry",
										"location": [32.0,32.0],
										"data": [
											{"in1": "lang:i1"}
										]
									},
									"if": {
										"type": "lang:if",
										"location": [44.0, 23.0],
										"data": null
									}
								},
								"connections": [
									{
										"type": "data",
										"input": ["entry",0],
										"output": ["if",0]
									},
									{
										"type": "exec",
										"input": ["entry",0],
										"output": ["if",0]
									}
								],
								
                                "inputs": [],
                                "outputs": []
								})ENDJSON"_json;

							requireWorks(correctJSON);
						}
					}
				}
			}
		}
	}
}
