#include "catch.hpp"

#include <fstream>

#include <chi/Support/Result.hpp>

#include <chi/Fetcher/Fetcher.hpp>

#include <boost/filesystem.hpp>

using namespace chi;
namespace fs = boost::filesystem;

TEST_CASE("Can fetch remote modules", "[Context]") {
	// create a temporary directory for a workspace
	fs::path workspaceDir =
	    boost::filesystem::temp_directory_path() / fs::unique_path();  // this is a tmp file
	fs::create_directories(workspaceDir);

	// create the .chigraphworkspace file
	{ fs::ofstream stream{workspaceDir / ".chigraphworkspace"}; }

	auto res = fetchModule(workspaceDir, "github.com/chigraph/hellochigraph/hello/main", true);
	REQUIRE(res.dump() == "");

	REQUIRE(fs::is_directory(workspaceDir / "src" / "github.com" / "chigraph" / "hellochigraph"));
}
