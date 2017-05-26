#include <catch.hpp>

#include <chi/Result.hpp>

using namespace nlohmann;

TEST_CASE("Result", "") {
	chi::Result res;
	REQUIRE(res.result_json.is_array());

	WHEN("We add stuff, the size of result_json goes up") {
		REQUIRE(res.result_json.size() == 0);

		res.addEntry("I123", "Message", {});

		REQUIRE(res.result_json.size() == 1);

		res.addEntry("I123", "Message", {});

		REQUIRE(res.result_json.size() == 2);
	}

	WHEN("We add stuff, the data gets recorderd correctly") {
		res.addEntry("I12", "She's a keeper", {{"Key 1", "Data 1"}});
		res.addEntry("I13", "Kaali", {{"Key 2", "Data 2"}, {"Woah", "Cool"}});

		REQUIRE(res.result_json.size() == 2);

		REQUIRE(res.result_json[0]["overview"] == "She's a keeper");
		REQUIRE(res.result_json[0]["errorcode"] == "I12");

		REQUIRE(res.result_json[0]["data"].size() == 1);

		REQUIRE(res.result_json[0]["data"].find("Key 1") != res.result_json[0]["data"].end());
		REQUIRE(res.result_json[0]["data"]["Key 1"] == "Data 1");

		REQUIRE(res.result_json[1]["overview"] == "Kaali");
		REQUIRE(res.result_json[1]["errorcode"] == "I13");

		REQUIRE(res.result_json[1]["data"].size() == 2);

		REQUIRE(res.result_json[1]["data"].find("Key 2") != res.result_json[1]["data"].end());
		REQUIRE(res.result_json[1]["data"]["Key 2"] == "Data 2");

		REQUIRE(res.result_json[1]["data"].find("Woah") != res.result_json[1]["data"].end());
		REQUIRE(res.result_json[1]["data"]["Woah"] == "Cool");
	}

	WHEN("We add an error, success turns to false") {
		REQUIRE(res.success() == true);

		res.addEntry("W111", "Message", {});

		REQUIRE(res.success() == true);

		res.addEntry("I113", "Message", {});

		REQUIRE(res.success() == true);

		res.addEntry("E124", "Message", {});

		REQUIRE(res.success() == false);
	}

	WHEN("We use contexts, they are merged") {
		{
			auto helloCtx = res.addScopedContext({{"Hello", "Double oh seven"}});

			REQUIRE(res.contextJson() == R"({"Hello": "Double oh seven"})"_json);

			res.addEntry("I231", "Kool.", {{"Goodbye", "Young man ;-)"}});

			REQUIRE(res.result_json[0]["data"].size() == 2);

			REQUIRE(res.result_json[0]["data"].find("Hello") != res.result_json[0]["data"].end());
			REQUIRE(res.result_json[0]["data"]["Hello"] == "Double oh seven");

			REQUIRE(res.result_json[0]["data"].find("Goodbye") != res.result_json[0]["data"].end());
			REQUIRE(res.result_json[0]["data"]["Goodbye"] == "Young man ;-)");

			WHEN("You add another, it should still merge") {
				{
					auto ctxv2 = res.addScopedContext(
					    {{"What makes you better than your brother?", "Nada."}});

					REQUIRE(
					    res.contextJson() ==
					    R"({"Hello": "Double oh seven", "What makes you better than your brother?": "Nada."})"_json);

					res.addEntry("Ikyo", "D", {{"Hey", "stfu"}});

					REQUIRE(res.result_json[1]["data"].size() == 3);

					REQUIRE(res.result_json[1]["data"].find("Hello") !=
					        res.result_json[1]["data"].end());
					REQUIRE(res.result_json[1]["data"]["Hello"] == "Double oh seven");

					REQUIRE(res.result_json[1]["data"].find(
					            "What makes you better than your brother?") !=
					        res.result_json[1]["data"].end());
					REQUIRE(
					    res.result_json[1]["data"]["What makes you better than your brother?"] ==
					    "Nada.");

					REQUIRE(res.result_json[1]["data"].find("Hey") !=
					        res.result_json[1]["data"].end());
					REQUIRE(res.result_json[1]["data"]["Hey"] == "stfu");
				}

				THEN("After the second context is destroyed, it should only merge the first") {
					REQUIRE(res.contextJson() == R"({"Hello": "Double oh seven"})"_json);

					res.addEntry("Iaaah", "sup", {{"bruh", "these tests are dooooope"}});

					REQUIRE(res.result_json[2]["data"].size() == 2);

					REQUIRE(res.result_json[2]["data"].find("Hello") !=
					        res.result_json[2]["data"].end());
					REQUIRE(res.result_json[2]["data"]["Hello"] == "Double oh seven");

					REQUIRE(res.result_json[2]["data"].find("bruh") !=
					        res.result_json[2]["data"].end());
					REQUIRE(res.result_json[2]["data"]["bruh"] == "these tests are dooooope");
				}
			}

			WHEN(
			    "The the two contexts have conflicting keys, the most recently added one takes "
			    "priority") {
				auto zCtx = res.addScopedContext({{"Hello", "Max Zorin"}});

				res.addEntry("Ekk", "1", {{"2", 3}});

				REQUIRE(res.result_json[1]["data"].size() == 2);

				REQUIRE(res.result_json[1]["data"].find("Hello") !=
				        res.result_json[1]["data"].end());
				REQUIRE(res.result_json[1]["data"]["Hello"] == "Max Zorin");

				REQUIRE(res.result_json[1]["data"].find("2") != res.result_json[1]["data"].end());
				REQUIRE(res.result_json[1]["data"]["2"] == 3);
			}

			WHEN("The entry conflicts with a context, the entry take priority") {
				res.addEntry("Ipp", "adf", {{"Hello", "Le Chiffre"}});

				REQUIRE(res.result_json[1]["data"].size() == 1);

				REQUIRE(res.result_json[1]["data"].find("Hello") !=
				        res.result_json[1]["data"].end());
				REQUIRE(res.result_json[1]["data"]["Hello"] == "Le Chiffre");
			}
		}
	}
}
