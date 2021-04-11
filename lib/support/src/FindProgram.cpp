#include "chi/Support/FindProgram.hpp"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <cstdlib>

namespace fs = std::filesystem;

namespace chi {
fs::path findProgram(const char* name) {
	// get path environment variable
	std::string pathEnv = std::getenv("PATH");

	const char* sep =
#ifdef _WIN32
	    ";"
#else
	    ":"
#endif
	    ;

	std::vector<std::string> paths;
	boost::algorithm::split(paths, pathEnv, boost::algorithm::is_any_of(sep));
	for (const auto& pathStr : paths) {
		auto path = fs::path(pathStr) / name;
		if ((fs::is_regular_file(path) || fs::is_symlink(path)) &&
		    (fs::status(path).permissions() & fs::perms::others_exec) == fs::perms::others_exec) {
			return path;
		}
	}

	return {};
}

}  // namespace chi