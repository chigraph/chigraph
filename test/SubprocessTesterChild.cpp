#include <iostream>
#include <thread>
#include <chrono>
#include <string>

using namespace std::chrono_literals;


std::string readAllStdin() {
	std::string ret{ std::istreambuf_iterator<char>{std::cin}, std::istreambuf_iterator<char>{} };

	return ret;
}

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cout << "usage: subprocess_tester_child <mode> where mode is either echo, echostderr, echoboth, helloworld, wait1s, or exitwitherr1";
	}

	auto mode = std::string{ argv[1] };

	if (mode == "echo") {
		std::cout << readAllStdin();
		return 0;
	}
	if (mode == "echostderr") {
		std::cerr << readAllStdin();
		return 0;
	}
	if (mode == "echoboth") {
		auto stdIn = readAllStdin();
		std::cout << stdIn;
		std::cerr << stdIn;
		return 0;
	}
	if (mode == "helloworld") {
		std::cout << "Hello World!";
		return 0;
	}
	if (mode == "wait1s") {
		std::this_thread::sleep_for(1s);
		return 0;
	}
	if (mode == "exitwitherr1") {
		return 1;
	}



}