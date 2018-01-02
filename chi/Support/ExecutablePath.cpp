/// \file ExecutablePath.cpp
/// Much of this file is copied for LLVM's lib/Support/Unix/Path.inc and
/// lib/Support/Windows/Path.inc

#include "ExecutablePath.hpp"

#include <boost/filesystem/operations.hpp>

namespace fs = boost::filesystem;

// linux implementation
#ifdef __linux__
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>
#define NAMLEN(dirent) strlen((dirent)->d_name)

namespace chi {
fs::path executablePath() {
	char     exe_path[MAXPATHLEN];
	fs::path aPath("/proc/self/exe");
	if (fs::exists(aPath)) {
		// /proc is not always mounted under Linux (chroot for example).
		ssize_t len = readlink(aPath.string().c_str(), exe_path, sizeof(exe_path));
		if (len >= 0) return std::string(exe_path, len);
	}
	assert(false);  // aah noo
	return {};
}
}  // namespace chi

// osx
#elif defined __APPLE__
#include <mach-o/dyld.h>
#include <stdlib.h>
#include <sys/attr.h>

namespace chi {
fs::path executablePath() {
	// On OS X the executable path is saved to the stack by dyld. Reading it
	// from there is much faster than calling dladdr, especially for large
	// binaries with symbols.
	char     exe_path[MAXPATHLEN];
	uint32_t size = sizeof(exe_path);
	if (_NSGetExecutablePath(exe_path, &size) == 0) {
		char link_path[MAXPATHLEN];
		if (realpath(exe_path, link_path)) return link_path;
	}
	assert(false);
	return {};
}
}  // namespace chi

#elif defined WIN32
#define WIN32_MEAN_AND_LEAN
#include <Windows.h>

namespace chi {

boost::filesystem::path executablePath() {
	std::vector<wchar_t> PathName(MAX_PATH);
	DWORD                Size = ::GetModuleFileNameW(NULL, PathName.data(), PathName.size());

	// A zero return value indicates a failure other than insufficient space.
	if (Size == 0) return "";

	// Insufficient space is determined by a return value equal to the size of
	// the buffer passed in.
	if (Size == PathName.size()) return "";

	// On success, GetModuleFileNameW returns the number of characters written to
	// the buffer not including the NULL terminator.
	PathName.resize(Size);

	// Convert the result from UTF-16 to UTF-8.
	std::string PathNameUTF8(MAX_PATH, '\0');

	int numWritten = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, PathName.data(), PathName.size(), &PathNameUTF8[0], PathNameUTF8.size(), nullptr, nullptr);
	if (numWritten == 0) return "";

	PathNameUTF8.resize(numWritten);
	return PathNameUTF8;
}

} // namespace chi
#else
#error GetMainExecutable is not implemented on this host yet.
#endif
