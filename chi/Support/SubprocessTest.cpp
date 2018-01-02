#include "catch.hpp"

#include <chi/Support/Result.hpp>
#include <chi/Support/Subprocess.hpp>

#include <thread>

using namespace chi;

namespace fs = boost::filesystem;

TEST_CASE("Subprocess", "") {

	auto childPath = fs::current_path() / "chi" / "Support" / "subprocesstesterchild"
#if _WIN32
		".exe"
#endif
	;

	std::string stdOut, stdErr;

	auto attachToPipes = [&stdOut, &stdErr](Subprocess& child) {
		stdOut.clear();
		stdErr.clear();
		child.attachStringToStdErr(stdErr);
		child.attachStringToStdOut(stdOut);
	};

	Result res;

	WHEN("We create a process that just spits stuff out, then it works") {
		Subprocess child{childPath};
		child.setArguments({"helloworld"});
		attachToPipes(child);

		res += child.start();

		// wait
		auto code = child.exitCode();
		REQUIRE(res.dump() == "");
		REQUIRE(code == 0);
		REQUIRE(stdOut == "Hello World!");
		REQUIRE(stdErr == "");
		REQUIRE(!child.running());
	}

	WHEN("We create a process that just echos") {
		Subprocess child{childPath};
		child.setArguments({"echo"});
		attachToPipes(child);

		res += child.start();

		res += child.pushToStdIn("hello", 5);
		res += child.closeStdIn();

		// wait
		auto code = child.exitCode();
		REQUIRE(res.dump() == "");
		REQUIRE(code == 0);
		REQUIRE(stdOut == "hello");
		REQUIRE(stdErr == "");
		REQUIRE(!child.running());
	}

	WHEN("We create a process that just echos to stderr") {
		Subprocess child{childPath};
		child.setArguments({"echostderr"});
		attachToPipes(child);

		res += child.start();

		res += child.pushToStdIn("hello", 5);
		res += child.closeStdIn();

		// wait
		auto code = child.exitCode();
		REQUIRE(res.dump() == "");
		REQUIRE(code == 0);
		REQUIRE(stdOut == "");
		REQUIRE(stdErr == "hello");
		REQUIRE(!child.running());
	}

	WHEN("We create a process that just echos to both stderr and stdout") {
		Subprocess child{childPath};
		child.setArguments({"echoboth"});
		attachToPipes(child);

		res += child.start();

		res += child.pushToStdIn("hello", 5);
		res += child.closeStdIn();

		// wait
		auto code = child.exitCode();
		REQUIRE(res.dump() == "");
		REQUIRE(code == 0);
		REQUIRE(stdOut == "hello");
		REQUIRE(stdErr == "hello");
		REQUIRE(!child.running());
	}

	WHEN("We create a process that just exits with code 1") {
		Subprocess child{childPath};
		child.setArguments({"exitwitherr1"});
		attachToPipes(child);

		res += child.start();

		// wait
		auto code = child.exitCode();
		REQUIRE(res.dump() == "");
		REQUIRE(code == 1);
		REQUIRE(stdOut == "");
		REQUIRE(stdErr == "");
		REQUIRE(!child.running());
	}

	WHEN("We create a process that sleeps so we can test running()") {
		Subprocess child{childPath};
		child.setArguments({"wait1s"});
		attachToPipes(child);

		res += child.start();

		REQUIRE(child.running());

		std::this_thread::sleep_for(std::chrono::seconds(2));

		REQUIRE(!child.running());

		// wait
		auto code = child.exitCode();
		REQUIRE(res.dump() == "");
		REQUIRE(code == 0);
		REQUIRE(stdOut == "");
		REQUIRE(stdErr == "");
		REQUIRE(!child.running());
	}
}
