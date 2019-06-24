/// \file Subprocess.cpp

#include "chi/Support/Subprocess.hpp"
#include "chi/Support/Result.hpp"

#include <cassert>
#include <thread>

#include <boost/algorithm/string/replace.hpp>

// Win32 Implementation
#ifdef WIN32

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

namespace chi {
namespace {
// http://stackoverflow.com/questions/1387064/how-to-get-the-error-message-from-the-error-code-returned-by-getlasterror
// Returns the last Win32 error, in string format. Returns an empty string if there is no error.
std::string GetLastErrorAsString() {
	// Get the error message, if any.
	DWORD errorMessageID = GetLastError();
	if (errorMessageID == 0) return std::string();  // No error message has been recorded

	LPSTR  messageBuffer = nullptr;
	size_t size          = FormatMessageA(
	    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	    NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0,
	    NULL);

	std::string message(messageBuffer, size);

	// Free the buffer.
	LocalFree(messageBuffer);

	return message;
}
// taken from
// https://docs.microsoft.com/en-us/visualstudio/debugger/how-to-set-a-thread-name-in-native-code
#pragma pack(push, 8)
typedef struct tagTHREADNAME_INFO {
	DWORD  dwType;      // Must be 0x1000.
	LPCSTR szName;      // Pointer to name (in user addr space).
	DWORD  dwThreadID;  // Thread ID (-1=caller thread).
	DWORD  dwFlags;     // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void SetThreadName(HANDLE thread, const char* name) {
	THREADNAME_INFO info;
	info.dwType     = 0x1000;
	info.szName     = name;
	info.dwThreadID = GetThreadId(thread);
	info.dwFlags    = 0;
#pragma warning(push)
#pragma warning(disable : 6320 6322)
	__try {
		RaiseException(0x406D1388, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
	} __except (EXCEPTION_EXECUTE_HANDLER) {}
#pragma warning(pop)
}
}  // namespace

struct Subprocess::Implementation {
	HANDLE StdIn_Write = 0;

	HANDLE StdOut_Read = 0;

	HANDLE StdErr_Read = 0;

	PROCESS_INFORMATION procInfo = {};

	std::thread stdoutThread;
	std::thread stderrThread;
};

Subprocess::~Subprocess() {
	wait();

	// close the FDs

	CloseHandle(mPimpl->StdOut_Read);
	CloseHandle(mPimpl->StdErr_Read);

	mPimpl->StdOut_Read = nullptr;
	mPimpl->StdErr_Read = nullptr;

	if (mPimpl->StdIn_Write != nullptr) {
		CloseHandle(mPimpl->StdIn_Write);
		mPimpl->StdIn_Write = nullptr;
	}

	if (mPimpl->stdoutThread.joinable()) { mPimpl->stdoutThread.join(); }

	if (mPimpl->stderrThread.joinable()) { mPimpl->stderrThread.join(); }
}

Result Subprocess::pushToStdIn(const char* data, size_t size) {
	assert(started() && "Process must be started to push to stdin");
	assert(!isStdInClosed() && "Cannot push to stdin after closing stdin");

	Result res;

	if (size == 0) { return {}; }

	DWORD bytesWritten;
	if (!WriteFile(mPimpl->StdIn_Write, data, size, &bytesWritten, nullptr)) {
		res.addEntry("EUKN", "Failed to write to stdin for process",
		             {{"Error Message", GetLastErrorAsString()}});
		return res;
	}
	if (bytesWritten == 0) {
		res.addEntry("EUKN", "No bytes were written to stdin",
		             {{"Error Message", GetLastErrorAsString()}});
	}

	return res;
}

Result Subprocess::closeStdIn() {
	assert(running() && "You cannot close stdin for a process you never started");
	assert(!isStdInClosed() && "You cannot close stdin for a process you never started");

	Result res;

	if (!CloseHandle(mPimpl->StdIn_Write)) {
		res.addEntry("EUKN", "Failed to close process stdin",
		             {{"Error Message", GetLastErrorAsString()}});
	}
	mPimpl->StdIn_Write = nullptr;

	mStdInClosed = true;

	return res;
}

Result Subprocess::start() {
	assert(!started() && "Cannot start a Subprocess twice");

	Result res;

	// Create SECURITY_ATTRIBUTES struct
	SECURITY_ATTRIBUTES secAttributes;
	secAttributes.nLength              = sizeof(SECURITY_ATTRIBUTES);
	secAttributes.bInheritHandle       = true;
	secAttributes.lpSecurityDescriptor = nullptr;

	HANDLE stdoutWrite;
	HANDLE stderrWrite;
	HANDLE stdinRead;

	// create stdout pipe
	if (!CreatePipe(&mPimpl->StdOut_Read, &stdoutWrite, &secAttributes, 0)) {
		res.addEntry("EUKN", "Failed to create stdout pipe",
		             {{"Error Message", GetLastErrorAsString()}});
		return res;
	}
	// make sure the read handle isn't inhereited
	if (!SetHandleInformation(mPimpl->StdOut_Read, HANDLE_FLAG_INHERIT, 0)) {
		res.addEntry("EUKN", "Failed to SetHandleInformation",
		             {{"Error Message", GetLastErrorAsString()}});
		return res;
	}

	// create stderr pipe
	if (!CreatePipe(&mPimpl->StdErr_Read, &stderrWrite, &secAttributes, 0)) {
		res.addEntry("EUKN", "Failed to create stderr pipe",
		             {{"Error Message", GetLastErrorAsString()}});
		return res;
	}
	// make sure the read handle isn't inhereited
	if (!SetHandleInformation(mPimpl->StdErr_Read, HANDLE_FLAG_INHERIT, 0)) {
		res.addEntry("EUKN", "Failed to SetHandleInformation",
		             {{"Error Message", GetLastErrorAsString()}});
		return res;
	}

	// create stdin pipe
	if (!CreatePipe(&stdinRead, &mPimpl->StdIn_Write, &secAttributes, 0)) {
		res.addEntry("EUKN", "Failed to create stdin pipe",
		             {{"Error Message", GetLastErrorAsString()}});
		return res;
	}
	// make sure the read handle isn't inhereited
	if (!SetHandleInformation(mPimpl->StdIn_Write, HANDLE_FLAG_INHERIT, 0)) {
		res.addEntry("EUKN", "Failed to SetHandleInformation",
		             {{"Error Message", GetLastErrorAsString()}});
		return res;
	}

	// finally start the process
	////////////////////////////

	// create commandline
	std::wstring commandLine = L'"' + mExePath.wstring() + L'"';

	for (const auto& cmd : mArguments) {
		std::wstring wCmd;
		std::copy(cmd.begin(), cmd.end(), std::back_inserter(wCmd));

		boost::algorithm::replace_all(wCmd, L"\"", L"\\\"");

		commandLine += L" \"" + wCmd + L'"';
	}

	ZeroMemory(&mPimpl->procInfo, sizeof(PROCESS_INFORMATION));

	// setup startupInfo
	STARTUPINFOW startupInfo;
	ZeroMemory(&startupInfo, sizeof(startupInfo));

	startupInfo.cb         = sizeof(startupInfo);
	startupInfo.hStdError  = stderrWrite;
	startupInfo.hStdOutput = stdoutWrite;
	startupInfo.hStdInput  = stdinRead;
	startupInfo.dwFlags |= STARTF_USESTDHANDLES;

	// create the process
	if (!CreateProcessW(nullptr, &commandLine[0], nullptr, nullptr, TRUE, 0, nullptr,
	                    mWorkingDir.c_str(), &startupInfo, &mPimpl->procInfo)) {
		res.addEntry("EUKN", "Failed to CreateProcess",
		             {{"Error Message", GetLastErrorAsString()}});
		return res;
	}

	// Close the thread handle, we don't need it
	CloseHandle(mPimpl->procInfo.hThread);

	// close the handles to the ends of the pipe we don't use
	CloseHandle(stderrWrite);
	CloseHandle(stdoutWrite);
	CloseHandle(stdinRead);

	// start threads for listening for input
	mPimpl->stdoutThread = std::thread([this]() {
		while (true) {
			DWORD bytesRead;

			std::vector<char> buffer;
			buffer.resize(8192);

			if (!ReadFile(mPimpl->StdOut_Read, buffer.data(), buffer.size(), &bytesRead, nullptr)) {
				return;
			}

			if (bytesRead == 0) { return; }

			// send it
			if (mStdOutHandler) { mStdOutHandler(buffer.data(), bytesRead); }
		}
	});

#ifndef NDEBUG
	{
		auto threadName = mExePath.filename().string() + " stdout reader";

		// name the thread for debugging
		SetThreadName(mPimpl->stdoutThread.native_handle(), threadName.c_str());
	}
#endif

	// start threads for listening for input
	mPimpl->stderrThread = std::thread([this]() {

		while (true) {
			DWORD bytesRead;

			std::vector<char> buffer;
			buffer.resize(8192);

			if (!ReadFile(mPimpl->StdErr_Read, buffer.data(), buffer.size(), &bytesRead, nullptr)) {
				return;
			}

			if (bytesRead == 0) { return; }

			// send it
			if (mStdErrHandler) { mStdErrHandler(buffer.data(), bytesRead); }
		}

	});

#ifndef NDEBUG
	{
		auto threadName = mExePath.filename().string() + " stderr reader";

		// name the thread for debugging
		SetThreadName(mPimpl->stderrThread.native_handle(), threadName.c_str());
	}
#endif

	mStarted = true;

	return res;
}

void Subprocess::kill() {
	if (running()) { TerminateProcess(mPimpl->procInfo.hProcess, 9); }
}

void Subprocess::wait() {
	assert(started() && "Cannot wait for a process before it's started");

	// wait for it to complete
	WaitForSingleObject(mPimpl->procInfo.hProcess, INFINITE);
}

int chi::Subprocess::exitCode() {
	wait();

	DWORD exitCode = 0;
	if (!GetExitCodeProcess(mPimpl->procInfo.hProcess, &exitCode)) {
		return -1;  // what should this be?
	}

	return exitCode;
}

bool Subprocess::running() {
	assert(started() && "Must start a process before checking if it's running");

	if (mPimpl->procInfo.hProcess == 0) { return false; }
	DWORD exitCode = 0;
	if (!GetExitCodeProcess(mPimpl->procInfo.hProcess, &exitCode)) { return false; }

	return exitCode == STILL_ACTIVE;
}

}  // namespace chi

// POSIX implementation
#else

#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

namespace chi {

struct Subprocess::Implementation {
	std::array<int, 2> stdinPipe = {{-1, -1}};

	std::array<int, 2> stdoutPipe = {{-1, -1}};
	std::array<int, 2> stderrPipe = {{-1, -1}};

	int childPID = -1;

	std::thread stdoutThread;
	std::thread stderrThread;
};

Subprocess::~Subprocess() {
	wait();

	// close the FDs

	if (isStdInClosed()) {
		close(mPimpl->stdinPipe[1]);
		mPimpl->stdinPipe[1] = -1;
	}

	close(mPimpl->stdoutPipe[0]);
	mPimpl->stdoutPipe[0] = -1;

	close(mPimpl->stderrPipe[0]);
	mPimpl->stderrPipe[0] = -1;

	if (mPimpl->stdoutThread.joinable()) { mPimpl->stdoutThread.join(); }

	if (mPimpl->stderrThread.joinable()) { mPimpl->stderrThread.join(); }
}

Result Subprocess::pushToStdIn(const char* data, size_t size) {
	assert(started() && "Process must be started to push to stdin");
	assert(!isStdInClosed() && "Cannot push to stdin after closing stdin");

	Result res;

	if (write(mPimpl->stdinPipe[1], data, size) == -1) {
		res.addEntry("EUKN", "Failed to write to stdin pipe", {{"Error message", strerror(errno)}});
		return res;
	}

	return res;
}

Result Subprocess::closeStdIn() {
	assert(running() && "You cannot close stdin for a process you never started");
	assert(!isStdInClosed() && "You cannot close stdin for a process you never started");

	Result res;

	if (close(mPimpl->stdinPipe[1]) == -1) {
		res.addEntry("EUKN", "Failed to close stdin pipe", {{"Error Message", strerror(errno)}});
		return res;
	}
	mPimpl->stdinPipe[1] = -1;

	mStdInClosed = true;

	return res;
}

Result Subprocess::start() {
	assert(!started() && "Cannot start a Subprocess twice");

	Result res;

	// create pipes
	if (pipe(mPimpl->stdinPipe.data()) != 0) {
		res.addEntry("EUKN", "Failed to create stdin pipe", {{"Error message", strerror(errno)}});
		return res;
	}
	if (pipe(mPimpl->stdoutPipe.data()) != 0) {
		res.addEntry("EUKN", "Failed to create stdout pipe", {{"Error message", strerror(errno)}});
		return res;
	}
	if (pipe(mPimpl->stderrPipe.data()) != 0) {
		res.addEntry("EUKN", "Failed to create stderr pipe", {{"Error message", strerror(errno)}});
		return res;
	}

	// fork!
	mPimpl->childPID = fork();

	if (mPimpl->childPID == -1) {
		res.addEntry("EUKN", "Failed to fork", {{"Error message", strerror(errno)}});
		return res;
	}

	// child process
	if (mPimpl->childPID == 0) {
		std::filesystem::current_path(mWorkingDir);

		// make read end of stdin pipe the stdin stream, and same for the other pipes
		dup2(mPimpl->stdinPipe[0], 0);
		dup2(mPimpl->stdoutPipe[1], 1);
		dup2(mPimpl->stderrPipe[1], 2);

		// close the other fds, we don't need them
		close(mPimpl->stdinPipe[0]);
		close(mPimpl->stdinPipe[1]);
		close(mPimpl->stdoutPipe[0]);
		close(mPimpl->stdoutPipe[1]);
		close(mPimpl->stderrPipe[0]);
		close(mPimpl->stderrPipe[1]);

		// close open fds for the process (other than 0, 1, and 2 which are the std streams)
		// https://stackoverflow.com/questions/899038/getting-the-highest-allocated-file-descriptor/899533#899533
		int fd_max = static_cast<int>(sysconf(_SC_OPEN_MAX));  // truncation is safe
		for (int fd = 3; fd < fd_max; fd++) close(fd);

		// set group
		setpgid(0, 0);

		// make argv
		std::vector<char*> argv;
		std::string        exePathStr = mExePath.string();
		argv.push_back(&exePathStr[0]);
		for (auto& arg : mArguments) { argv.push_back(&arg[0]); }
		argv.push_back(nullptr);

		// run the process
		execvp(mExePath.c_str(), argv.data());

		_exit(EXIT_FAILURE);
	}

	// parent process

	// close the ends of the pipes we don't use
	close(mPimpl->stdinPipe[0]);
	mPimpl->stdinPipe[0] = -1;
	close(mPimpl->stdoutPipe[1]);
	mPimpl->stdoutPipe[1] = -1;
	close(mPimpl->stderrPipe[1]);
	mPimpl->stderrPipe[1] = -1;

	// start threads
	mPimpl->stdoutThread = std::thread([this] {

		std::array<char, 4096> buffer;
		while (true) {
			// read from stdout
			ssize_t bytesRead = read(mPimpl->stdoutPipe[0], buffer.data(), buffer.size());

			// error
			if (bytesRead == -1) { return; }

			// 0 is EOF
			if (bytesRead == 0) { return; }

			if (mStdOutHandler) { mStdOutHandler(buffer.data(), bytesRead); }
		}
	});

	mPimpl->stderrThread = std::thread([this] {

		std::array<char, 4096> buffer;
		while (true) {
			// read from stdout
			ssize_t bytesRead = read(mPimpl->stderrPipe[0], buffer.data(), buffer.size());

			// error
			if (bytesRead == -1) { return; }

			// 0 is EOF
			if (bytesRead == 0) { return; }

			if (mStdErrHandler) { mStdErrHandler(buffer.data(), bytesRead); }
		}
	});

	mStarted = true;

	return {};
}

void Subprocess::kill() {
	assert(started() && "Cannot kill a process if it never started");
	::kill(mPimpl->childPID, SIGINT);
}

void Subprocess::wait() {
	assert(started() && "Cannot wait for a process before it's started");

	int exit_status;
	waitpid(mPimpl->childPID, &exit_status, 0);

	if (WIFEXITED(exit_status)) { mExitCode = WEXITSTATUS(exit_status); }
}

int chi::Subprocess::exitCode() {
	assert(started() && "Cannot get the exit code of a process before it's started");

	if (mExitCode) { return *mExitCode; }

	int exit_status;
	if (waitpid(mPimpl->childPID, &exit_status, 0) == -1) { return -1; }

	if (WIFEXITED(exit_status)) {
		mExitCode = WEXITSTATUS(exit_status);
		return *mExitCode;
	}

	return -1;
}

bool Subprocess::running() {
	assert(started() && "Must start a process before checking if it's running");

	int exit_status;

	// WNOHANG makes sure it doesn't wait until the process is done
	int status = waitpid(mPimpl->childPID, &exit_status, WNOHANG);
	if (status == -1) {
		return false;  // error
	}
	if (status == 0) { return true; }
	if (WIFEXITED(exit_status)) { mExitCode = int(WEXITSTATUS(exit_status)); }
	return false;
}

}  // namespace chi

#endif

namespace chi {

// Common functions
Subprocess::Subprocess(const std::filesystem::path& path)
    : mPimpl{std::make_unique<Subprocess::Implementation>()}, mExePath{path} {
	assert(std::filesystem::is_regular_file(path) &&
	       "the path passed to subprocess is not a regular file");
}

}  // namespace chi
