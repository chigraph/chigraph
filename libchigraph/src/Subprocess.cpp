#include "chi/Subprocess.hpp"

#include "chi/Result.hpp"

#include <cassert>

#include <boost/algorithm/string/replace.hpp>



namespace chi {

// Win32 Implementation
#ifdef WIN32

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

namespace {
// http://stackoverflow.com/questions/1387064/how-to-get-the-error-message-from-the-error-code-returned-by-getlasterror
// Returns the last Win32 error, in string format. Returns an empty string if there is no error.
std::string GetLastErrorAsString()
{
	//Get the error message, if any.
	DWORD errorMessageID = GetLastError();
	if (errorMessageID == 0)
		return std::string(); //No error message has been recorded

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;
}
} // namespace 

struct Subprocess::Implementation {
	HANDLE StdIn_Read = 0;
	HANDLE StdIn_Write = 0;

	HANDLE StdOut_Read = 0;
	HANDLE StdOut_Write = 0;
	
	HANDLE StdErr_Read = 0;
	HANDLE StdErr_Write = 0;

	PROCESS_INFORMATION procInfo = {};
};

Result Subprocess::pushToStdIn(const char* data, size_t size)
{

	Result res;

	if (size == 0) {
		return{};
	}

	DWORD bytesWritten;
	if (!WriteFile(mPimpl->StdIn_Write, data, size, &bytesWritten, nullptr)) {
		res.addEntry("EUKN", "Failed to write to stdin for process", { {"Error Message", GetLastErrorAsString()}});
		return res;
	}
	if (bytesWritten == 0) {
		res.addEntry("EUKN", "No bytes were written to stdin", { { "Error Message", GetLastErrorAsString() } });
	}

	return res;
}

Result Subprocess::closeStdIn()
{
	Result res;
	
	if (!CloseHandle(mPimpl->StdIn_Write)) {
		res.addEntry("EUKN", "Failed to close process stdin", { { "Error Message", GetLastErrorAsString() } });
	}
	mPimpl->StdIn_Write = 0;

	return res;
}

Result Subprocess::handleInput()
{
	Result res;

	std::array<char, 8192> buffer;

	// read stdout
	while (true) {
		DWORD bytesRead;

		if (!ReadFile(mPimpl->StdOut_Read, buffer.data(), buffer.size(), &bytesRead, nullptr)) {
			if (GetLastError() != ERROR_MORE_DATA) {


				res.addEntry("EUKN", "Failed to read from stdout pipe", { { "Error Message", GetLastErrorAsString() } });
				return res;
			}
		}

		// send it
		if (mStdOutHandler) {
			mStdOutHandler(buffer.data(), bytesRead);
		}

		if (bytesRead == 0) {
			break;
		}
	}
	// read stderr
	while (true) {
		DWORD bytesRead;

		if (!ReadFile(mPimpl->StdErr_Read, buffer.data(), buffer.size(), &bytesRead, nullptr)) {
			res.addEntry("EUKN", "Failed to read from stderr pipe", { { "Error Message", GetLastErrorAsString() } });
			return res;
		}

		// send it
		if (mStdErrHandler) {
			mStdErrHandler(buffer.data(), bytesRead);
		}

		if (bytesRead == 0) {
			break;
		}
	}

	return res;
}

Result Subprocess::start()
{
	Result res;

	// Create SECURITY_ATTRIBUTES struct
	SECURITY_ATTRIBUTES secAttributes;
	secAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
	secAttributes.bInheritHandle = true;
	secAttributes.lpSecurityDescriptor = nullptr;

	// create stdout pipe
	if (!CreatePipe(&mPimpl->StdOut_Read, &mPimpl->StdOut_Write, &secAttributes, 0)) {
		res.addEntry("EUKN", "Failed to create stdout pipe", { { "Error Message", GetLastErrorAsString() } });
		return res;
	}
	// make sure the read handle isn't inhereited
	if (!SetHandleInformation(mPimpl->StdOut_Read, HANDLE_FLAG_INHERIT, 0)) {
		res.addEntry("EUKN", "Failed to SetHandleInformation", { { "Error Message", GetLastErrorAsString() } });
		return res;
	}


	// create stderr pipe
	if (!CreatePipe(&mPimpl->StdErr_Read, &mPimpl->StdErr_Write, &secAttributes, 0)) {
		res.addEntry("EUKN", "Failed to create stderr pipe", { { "Error Message", GetLastErrorAsString() } });
		return res;
	}
	// make sure the read handle isn't inhereited
	if (!SetHandleInformation(mPimpl->StdErr_Read, HANDLE_FLAG_INHERIT, 0)) {
		res.addEntry("EUKN", "Failed to SetHandleInformation", { { "Error Message", GetLastErrorAsString() } });
		return res;
	}


	// create stdin pipe
	if (!CreatePipe(&mPimpl->StdIn_Read, &mPimpl->StdIn_Write, &secAttributes, 0)) {
		res.addEntry("EUKN", "Failed to create stdin pipe", { { "Error Message", GetLastErrorAsString() } });
		return res;
	}
	// make sure the read handle isn't inhereited
	if (!SetHandleInformation(mPimpl->StdIn_Read, HANDLE_FLAG_INHERIT, 0)) {
		res.addEntry("EUKN", "Failed to SetHandleInformation", { { "Error Message", GetLastErrorAsString() } });
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

	// setup startupInfo
	STARTUPINFO startupInfo = {};
	startupInfo.cb = sizeof(STARTUPINFO);
	startupInfo.hStdError = mPimpl->StdErr_Write;
	startupInfo.hStdOutput = mPimpl->StdOut_Write;
	startupInfo.hStdInput = mPimpl->StdIn_Read;
	startupInfo.dwFlags = STARTF_USESTDHANDLES;
	
	// create the process
	if (!CreateProcessW(nullptr, &commandLine[0], nullptr, nullptr, TRUE, 0, nullptr, mWorkingDir.c_str(), &startupInfo, &mPimpl->procInfo)) {
		res.addEntry("EUKN", "Failed to CreateProcess", { { "Error Message", GetLastErrorAsString() } });
		return res;
	}


	// write cached stdin
	if (!mCachedStdIn.empty()) {
		pushToStdIn(mCachedStdIn.data(), mCachedStdIn.size());
		mCachedStdIn.clear();
	}

	return res;
}

void Subprocess::kill()
{
	if (running()) {

		TerminateProcess(mPimpl->procInfo.hProcess, 9);
	}
}

void Subprocess::wait()
{
	// wait for it to complete
	while (running()) {

		WaitForSingleObject(mPimpl->procInfo.hProcess, 100);

		handleInput();
	}
}

int chi::Subprocess::exitCode()
{
	wait();

	DWORD exitCode = 0;
	if (!GetExitCodeProcess(mPimpl->procInfo.hProcess, &exitCode)) {
		return -1; // what should this be?
	}

	return exitCode;
}

bool Subprocess::running()
{
	if (mPimpl->procInfo.hProcess == 0) {
		return false;
	}
	DWORD exitCode = 0;
	if (!GetExitCodeProcess(mPimpl->procInfo.hProcess, &exitCode)) {
		return false;
	}

	return exitCode == STILL_ACTIVE;
}

void Subprocess::wait_for(std::chrono::milliseconds duration)
{

	WaitForSingleObject(mPimpl->procInfo.hProcess, duration.count());

	handleInput();
}

// POSIX implementation
#else 

#include <unistd.h>
#include <bits/signum.h>

struct Subprocess::Implementation {
	std::array<int, 2> stdinPipe = {{-1, -1}};
	
	std::array<int, 2> stdoutPipe = {{-1, -1}};
	std::array<int, 2> stderrPipe = {{-1, -1}};
	
	int childPID = -1;
};

Result Subprocess::pushToStdIn(const char* data, size_t size)
{
	Result res;
	
	if (write(mPimpl->stdinPipe[0], data, size) == -1) {
		res.addEntry("EUKN", "Failed to write to stdin pipe", {{"Error message", strerror(errno)}});
		return res;
	}
	
	return res;
}

Result Subprocess::closeStdIn()
{
	if (close(mPimpl->stdinPipe[0]) == -1) {
		res.addEntry("EUKN", "Failed to close stdin pipe", {{"Error Message", strerror(errno)}});
		return res;
	}
}

Result Subprocess::handleInput()
{
	Result res;
	
	std::array<char, 4096> buffer;
	
	// read from stdout
	ssize_t bytesRead = read(mPimpl->stdoutPipe[1], buffer.data(), buffer.size());
	
	// error
	if (bytesRead == -1) {
		res.addEntry("EUKN", "Failed to read from stdout pipe", {{"Error Message", strerror(errno)}});
		return res;
	}
	
	// 0 is EOF
	if (bytesRead != 0 && mStdOutHandler) {
		mStdOutHandler(buffer.data(), bytesRead);
	}
	
	// read from stderr
	
}

Result Subprocess::start()
{
	Result res;
	
	// create pipes
	if (pipe(mPimpl->stdinPipe.data()) != 0) {
		res.addEntry("EUKN", "Failed to create stdin pipe", {{"Error message", strerror(errno)}});
		return res;
	}
	if (pipe (mPimpl->stdoutPipe.data()) != 0) {
		res.addEntry("EUKN", "Failed to create stdout pipe", {{"Error message", strerror(errno)}});
		return res;
	}
	if (pipe (mPimpl->stderrPipe.data()) != 0) {
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
		boost::filesystem::current_path(mWorkingDir);
		
		// make read end of stdin pipe the stdin stream, and same for the other pipes
		dup2(mPimpl->stdinPipe[0], 0);
		dup2(mPimpl->stdoutPipe[1], 1);
		dup2(mPimpl->stdoutPipe[1], 2);
		
		// close the other fds, we don't need them
		close(mPimpl->stdinPipe[0]);
		close(mPimpl->stdinPipe[1]);
		close(mPimpl->stdoutPipe[0]);
		close(mPimpl->stdinPipe[1]);
		close(mPimpl->stderrPipe[0]);
		close(mPimpl->stdinPipe[1]);

		// close open fds for the process (other than 0, 1, and 2 which are the std streams)
		// https://stackoverflow.com/questions/899038/getting-the-highest-allocated-file-descriptor/899533#899533
		int fd_max=static_cast<int>(sysconf(_SC_OPEN_MAX)); // truncation is safe
		for(int fd=3;fd<fd_max;fd++)
			close(fd);
		
		// set group
		setpgid(0, 0);
		
		
		// make argv
		std::vector<char*> argv;
		std::string exePathStr = mExePath.string();
		argv.push_back(&exePathStr[0]);
		for (auto& arg : mArguments) {
			argv.push_back(&arg[0]);
		}
		
		// run the process
		execvp(mExePath.c_str(), argv.data());
		
		_exit(EXIT_FAILURE);

	}
	
	// parent process
	
	// close the ends of the pipes we don't use
	close(mPimpl->stdinPipe[1]);
	close(mPimpl->stdoutPipe[0]);
	close(mPimpl->stderrPipe[0]);
	
	return {};
}

void Subprocess::kill()
{
	kill(mPimpl->childPID, SIGINT);
}

void Subprocess::wait()
{
	waitpid()
}

int chi::Subprocess::exitCode()
{
	
}

bool Subprocess::running()
{
	
}

void Subprocess::wait_for(std::chrono::milliseconds duration)
{
	
}

#endif



// Common functions
Subprocess::Subprocess(const boost::filesystem::path& path) : mPimpl{ std::make_unique<Subprocess::Implementation>() }, mExePath{ path }  {}
Subprocess::~Subprocess() = default;

}
