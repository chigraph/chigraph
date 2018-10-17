#include <catch.hpp>

#include <chi/Context.hpp>
#include <chi/DataType.hpp>
#include <chi/LangModule.hpp>
#include <chi/NodeType.hpp>
#include <chi/Support/Result.hpp>
#include <chi/Fetcher/Fetcher.hpp>

#include <llvm/IR/DerivedTypes.h>

using namespace chi;
namespace fs = boost::filesystem;

TEST_CASE("Contexts can fetch remote modules", "[Context]") {
	// create a temporary directory for a workspace
	fs::path workspaceDir =
	    boost::filesystem::temp_directory_path() / fs::unique_path();  // this is a tmp file
	fs::create_directories(workspaceDir);

	// create the .chigraphworkspace file
	{ fs::ofstream stream{workspaceDir / ".chigraphworkspace"}; }

	auto res = fetchModule(workspaceDir, "github.com/chigraph/hellochigraph/hello/main", true);
	REQUIRE(res.dump() == "");

	REQUIRE(fs::is_directory(workspaceDir / "src" / "github.com" / "chigraph" / "hellochigraph"));
	REQUIRE(fs::is_directory(workspaceDir / "src" / "github.com" / "chigraph" / "std")); // it should have fetched the dependency as well
}
