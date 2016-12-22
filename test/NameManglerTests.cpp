#include "catch.hpp"

#include <chig/NameMangler.hpp>

using namespace chig;

TEST_CASE("Name Mangler", "")
{
	using namespace std::string_literals;

	REQUIRE(mangleFunctionName("github.com/russelltg/test/lib", "docall") ==
			"github_dcom_srusselltg_stest_slib_mdocall");

	REQUIRE(mangleFunctionName("github.com/russelltg/test/main", "main") == "main");

	REQUIRE(mangleFunctionName("github.com/russelltg/test/main", "maina") ==
			"github_dcom_srusselltg_stest_smain_mmaina");

	REQUIRE(mangleFunctionName("github.com/rus__selltg/test/lib", "docall") ==
			"github_dcom_srus____selltg_stest_slib_mdocall");

	REQUIRE(mangleFunctionName("", "docall") == "_mdocall");

	REQUIRE(mangleFunctionName("github.com/russelltg/test/lib", "") ==
			"github_dcom_srusselltg_stest_slib_m");

	REQUIRE(mangleFunctionName("_github.com/russelltg/test/lib_", "doca_ll") ==
			"__github_dcom_srusselltg_stest_slib___mdoca_ll");

	// test demangling
	auto pair = unmangleFunctionName("github_dcom_srusselltg_stest_slib_mdocall");
	REQUIRE(pair.first == "github.com/russelltg/test/lib"s);
	REQUIRE(pair.second == "docall"s);

	pair = unmangleFunctionName("main");
	REQUIRE(pair.first == "main"s);
	REQUIRE(pair.second == "main"s);

	pair = unmangleFunctionName("github_dcom_srus____selltg_stest_slib_mdocall");
	REQUIRE(pair.first == "github.com/rus__selltg/test/lib"s);
	REQUIRE(pair.second == "docall"s);

	pair = unmangleFunctionName("_mdocall");
	REQUIRE(pair.first == ""s);
	REQUIRE(pair.second == "docall"s);

	pair = unmangleFunctionName("github_dcom_srusselltg_stest_slib_m");
	REQUIRE(pair.first == "github.com/russelltg/test/lib"s);
	REQUIRE(pair.second == ""s);

	pair = unmangleFunctionName("__github_dcom_srusselltg_stest_slib___mdoca_ll");
	REQUIRE(pair.first == "_github.com/russelltg/test/lib_"s);
	REQUIRE(pair.second == "doca_ll"s);
}
