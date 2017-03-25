#include "catch.hpp"

#include <chi/Context.hpp>
#include <chi/GraphFunction.hpp>
#include <chi/GraphModule.hpp>
#include <chi/JsonSerializer.hpp>
#include <chi/LangModule.hpp>
#include <chi/NodeInstance.hpp>
#include <chi/Result.hpp>
#include <chi/DataType.hpp>
#include <chi/NodeType.hpp>

#include <boost/uuid/uuid_io.hpp>

using namespace chi;
using namespace nlohmann;

template <typename... Args>
std::string string_format(const std::string& format, Args... args) {
	size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1;  // Extra space for '\0'
	std::unique_ptr<char[]> buf(new char[size]);
	snprintf(buf.get(), size, format.c_str(), args...);
	return std::string(buf.get(), buf.get() + size - 1);  // We don't want the '\0' inside
}

TEST_CASE("JsonSerializer", "[json]") {
	GIVEN("A default constructed Context with a LangModule and GraphFunction named hello") {
		Result  res;
		Context c;
		REQUIRE(!!c.loadModule("lang"));
		LangModule* lmod = static_cast<LangModule*>(c.moduleByFullName("lang"));
		REQUIRE(lmod != nullptr);

		auto jmod = c.newGraphModule("main/main");
		jmod->addDependency("lang");
		REQUIRE(jmod != nullptr);

		bool created;
		auto func = jmod->getOrCreateFunction("hello", {}, {}, {""}, {""}, &created);
		func->setDescription("desc");
		REQUIRE(created == true);
		REQUIRE(func != nullptr);

		auto requireWorks = [&](nlohmann::json expected) {
			nlohmann::json ret = graphFunctionToJson(*func);

			REQUIRE(ret == expected);

		};

		THEN("The JSON should be correct") {
			auto correctJSON = R"ENDJSON(
				{
					"type": "function",
					"name": "hello",
					"description": "desc",
					"nodes": {},
					"connections": [],
                    "data_inputs": [],
                    "data_outputs": [],
					"exec_inputs": [""],
					"exec_outputs": [""],
					"local_variables": {}
				}
			)ENDJSON"_json;

			requireWorks(correctJSON);
		}

		WHEN("We create some nodes and try to dump json") {
			std::vector<std::pair<DataType, std::string>> inputs = {
			    {lmod->typeFromName("i1"), "in1"}};

			std::unique_ptr<NodeType> toFill;
			Result                    res = c.nodeTypeFromModule(
			    "lang", "entry", R"({"data": [{"in1": "lang:i1"}], "exec": [""]})"_json, &toFill);
			REQUIRE(!!res);
			NodeInstance* entry;
			res += func->insertNode(std::move(toFill), 32, 32, boost::uuids::random_generator()(),
			                        &entry);
			std::string entryUUID = boost::uuids::to_string(entry->id());
			REQUIRE(!!res);

			THEN("The JSON should be correct") {
				auto correctJSON = string_format(R"(
					{
						"type": "function",
						"name": "hello",
						"description": "desc",
						"nodes": {
							"%s": {
								"type": "lang:entry",
								"location": [32.0,32.0],
								"data": {
									"data": [{"in1": "lang:i1"}],
									"exec": [""]
								}
							}
						},
						"connections": [],
                    "data_inputs": [],
                    "data_outputs": [],
					"exec_inputs": [""],
					"exec_outputs": [""] ,
					"local_variables": {}
					}
					)",
				                                 entryUUID.c_str());

				requireWorks(json::parse(correctJSON));
			}

			WHEN("A lang:if is added") {
				std::unique_ptr<NodeType> ifType;
				res = c.nodeTypeFromModule("lang", "if", {}, &ifType);
				REQUIRE(!!res);
				NodeInstance* ifNode;
				res += func->insertNode(std::move(ifType), 44.f, 23.f,
				                        boost::uuids::random_generator()(), &ifNode);
				std::string ifUUID = boost::uuids::to_string(ifNode->id());
				REQUIRE(!!res);

				THEN("The JSON should be correct") {
					auto correctJSON = string_format(R"ENDJSON(
						{
							"type": "function",
							"name": "hello",
							"description": "desc",
							"nodes": {
								"%s": {
									"type": "lang:entry",
									"location": [32.0,32.0],
									"data": {
										"data": [{"in1": "lang:i1"}],
										"exec": [""]
									}
								},
								"%s": {
									"type": "lang:if",
									"location": [44.0, 23.0],
									"data": null
								}
							},
							"connections": [],
							"data_inputs": [],
							"data_outputs": [],
							"exec_inputs": [""],
							"exec_outputs": [""],
							"local_variables": {}
						}
						)ENDJSON",
					                                 entryUUID.c_str(), ifUUID.c_str());

					requireWorks(json::parse(correctJSON));
				}

				WHEN("We connect the entry to the ifNode exec") {
					connectExec(*entry, 0, *ifNode, 0);

					THEN("The JSON should be correct") {
						auto correctJSON = string_format(R"ENDJSON(
							{
								"type": "function",
								"name": "hello",
								"description": "desc",
								"nodes": {
									"%s": {
										"type": "lang:entry",
										"location": [32.0,32.0],
										"data": {
											"data": [{"in1": "lang:i1"}],
											"exec": [""]
										}
									},
									"%s": {
										"type": "lang:if",
										"location": [44.0, 23.0],
										"data": null
									}
								},
								"connections": [
									{
										"type": "exec",
										"input": ["%s",0],
										"output": ["%s",0]
									}
								],
								"data_inputs": [],
								"data_outputs": [],
								"exec_inputs": [""],
								"exec_outputs": [""],
								"local_variables": {}
							}
						)ENDJSON",
						                                 entryUUID.c_str(), ifUUID.c_str(),
						                                 entryUUID.c_str(), ifUUID.c_str());

						requireWorks(json::parse(correctJSON));
					}

					WHEN("Connect the data") {
						res = connectData(*entry, 0, *ifNode, 0);

						REQUIRE(res.result_json == json::array());

						THEN("The JSON should be correct") {
							auto correctJSON =
							    string_format(R"ENDJSON(
								{
								"type": "function",
								"name": "hello",
								"description": "desc",
								"nodes": {
									"%s": {
										"type": "lang:entry",
										"location": [32.0,32.0],
										"data": {
											"data": [{"in1": "lang:i1"}],
											"exec": [""]
										}
									},
									"%s": {
										"type": "lang:if",
										"location": [44.0, 23.0],
										"data": null
									}
								},
								"connections": [
									{
										"type": "data",
										"input": ["%s",0],
										"output": ["%s",0]
									},
									{
										"type": "exec",
										"input": ["%s",0],
										"output": ["%s",0]
									}
								],
								"data_inputs": [],
								"data_outputs": [],
								"exec_inputs": [""],
								"exec_outputs": [""],
								"local_variables": {}
								})ENDJSON",
							                  entryUUID.c_str(), ifUUID.c_str(), entryUUID.c_str(),
							                  ifUUID.c_str(), entryUUID.c_str(), ifUUID.c_str());

							requireWorks(json::parse(correctJSON));
						}
					}
				}
			}
		}

		WHEN("We create some local variables") {
			func->getOrCreateLocalVariable("hello", lmod->typeFromName("i32"));

			auto correctJSON = R"ENDJSON(
				{
					"type": "function",
					"name": "hello",
					"description": "desc",
					"nodes": {},
					"connections": [],
                    "data_inputs": [],
                    "data_outputs": [],
					"exec_inputs": [""],
					"exec_outputs": [""],
					"local_variables": {"hello": "lang:i32"}
				}
			)ENDJSON"_json;

			requireWorks(correctJSON);
		}
	}
}
