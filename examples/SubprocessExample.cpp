/// A simple example on how to use the Subprocess class
/// The explanation for this is in the chi::Subprocess documentation

#include <chi/Subprocess.hpp>
#include <chi/Result.hpp>

#include <iostream>

int main() {
	
	/// [Constructing]
	chi::Subprocess child{
#ifdef WIN32
		"C:\\Windows\\System32\\cmd.exe"
#else
		"/bin/sh"
#endif
	};
	/// [Constructing]
	
	/// [Setup]
	std::string stdOut;
	child.attachStringToStdOut(stdOut);
	/// [Setup]
	
	/// [Start]
	chi::Result res;
	res += child.start();
	if (!res) {
		std::cerr << res << std::endl;
		return 1;
	}
	/// [Start]
	
	/// [Write to stdin]
	std::string dataToSend = "echo hello";
	res += child.pushToStdIn(dataToSend.data(), dataToSend.size());
	if (!res) {
		std::cerr << res << std::endl;
		return 1;
	}
	res += child.closeStdIn();
	if (!res) {
		std::cerr << res << std::endl;
		return 1;
	}
	/// [Write to stdin]
	
	/// [Get Exit Code]
	int exitCode = child.exitCode();
	if (exitCode != 0) {
		std::cerr << "Failed run /bin/sh < 'echo hi' with exit code: " << exitCode << std::endl;
		return 1;
	}
	/// [Get Exit Code]
	
	/// [Use byproducts]
	std::cout << stdOut << std::endl;
	/// [Use byproducts]
}
