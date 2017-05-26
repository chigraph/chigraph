#include <catch.hpp>

#include <chi/Subprocess.hpp>
#include <chi/Result.hpp>

#include <llvm/Support/FileSystem.h>

#include <thread>

using namespace chi;

namespace fs = boost::filesystem;

using namespace std::chrono_literals;

TEST_CASE("Subprocess", "") {
	auto childPath = fs::path{ llvm::sys::fs::getMainExecutable(nullptr, nullptr) }.parent_path() / "subprocess_tester_child"
#ifdef WIN32
		".exe"
#endif
		;


	std::string stdOut, stdErr;

	auto attachToPipes = [&stdOut, &stdErr](Subprocess& child) {
		stdOut.clear();
		stdErr.clear();
		child.attachToStdErr([&stdErr](const char* data, size_t size) {
			stdErr.append(data, size);
		});
		child.attachToStdOut([&stdOut](const char* data, size_t size) {
			stdOut.append(data, size);
		});
	};

	Result res;

	WHEN("We create a process that just spits stuff out, then it works") {
		Subprocess child{ childPath };
		child.setArguments({ "helloworld" });
		attachToPipes(child);

		res += child.start();

		// wait
		auto code = child.exitCode();
		REQUIRE(code == 0);
		REQUIRE(res.dump() == "");
		REQUIRE(stdOut == "Hello World!");
		REQUIRE(stdErr == "");
		REQUIRE(!child.running());

	}


	WHEN("We create a process that just echos") {
		Subprocess child{ childPath };
		child.setArguments({ "echo" });
		attachToPipes(child);

		res += child.start();

		res += child.pushToStdIn("hello", 5);
		res += child.closeStdIn();

		// wait
		auto code = child.exitCode();
		REQUIRE(code == 0);
		REQUIRE(res.dump() == "");
		REQUIRE(stdOut == "hello");
		REQUIRE(stdErr == "");
		REQUIRE(!child.running());

	}

	WHEN("We create a process that just echos to stderr") {
		Subprocess child{ childPath };
		child.setArguments({ "echostderr" });
		attachToPipes(child);

		res += child.start();

		res += child.pushToStdIn("hello", 5);
		res += child.closeStdIn();

		// wait
		auto code = child.exitCode();
		REQUIRE(code == 0);
		REQUIRE(res.dump() == "");
		REQUIRE(stdOut == "");
		REQUIRE(stdErr == "hello");
		REQUIRE(!child.running());

	}

	WHEN("We create a process that just echos to both stderr and stdout") {
		Subprocess child{ childPath };
		child.setArguments({ "echoboth" });
		attachToPipes(child);

		res += child.start();

		res += child.pushToStdIn("hello", 5);
		res += child.closeStdIn();

		// wait
		auto code = child.exitCode();
		REQUIRE(code == 0);
		REQUIRE(res.dump() == "");
		REQUIRE(stdOut == "hello");
		REQUIRE(stdErr == "hello");
		REQUIRE(!child.running());

	}


	WHEN("We create a process that just exits with code 1") {
		Subprocess child{ childPath };
		child.setArguments({ "exitwitherr1" });
		attachToPipes(child);

		res += child.start();

		// wait
		auto code = child.exitCode();
		REQUIRE(code == 1);
		REQUIRE(res.dump() == "");
		REQUIRE(stdOut == "");
		REQUIRE(stdErr == "");
		REQUIRE(!child.running());

	}

	WHEN("We create a process that sleeps so we can test running()") {
		Subprocess child{ childPath };
		child.setArguments({ "wait1s" });
		attachToPipes(child);

		res += child.start();

		REQUIRE(child.running());

		std::this_thread::sleep_for(2s);

		REQUIRE(!child.running());

		// wait
		auto code = child.exitCode();
		REQUIRE(code == 0);
		REQUIRE(res.dump() == "");
		REQUIRE(stdOut == "");
		REQUIRE(stdErr == "");
		REQUIRE(!child.running());

	}
}