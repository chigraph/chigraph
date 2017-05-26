#include "chi/Subprocess.hpp"

#include "chi/Result.hpp"

#include <cassert>
#include <thread>

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

	if (mPimpl->stdoutThread.joinable()) {
		mPimpl->stdoutThread.join();
	}

	if (mPimpl->stderrThread.joinable()) {
		mPimpl->stderrThread.join();
	}
}


Result Subprocess::pushToStdIn(const char* data, size_t size)
{
	assert(running() && "Process must be running to push to stdin");

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
	mPimpl->StdIn_Write = nullptr;

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

	HANDLE stdoutWrite;
	HANDLE stderrWrite;
	HANDLE stdinRead;

	// create stdout pipe
	if (!CreatePipe(&mPimpl->StdOut_Read, &stdoutWrite, &secAttributes, 0)) {
		res.addEntry("EUKN", "Failed to create stdout pipe", { { "Error Message", GetLastErrorAsString() } });
		return res;
	}
	// make sure the read handle isn't inhereited
	if (!SetHandleInformation(mPimpl->StdOut_Read, HANDLE_FLAG_INHERIT, 0)) {
		res.addEntry("EUKN", "Failed to SetHandleInformation", { { "Error Message", GetLastErrorAsString() } });
		return res;
	}


	// create stderr pipe
	if (!CreatePipe(&mPimpl->StdErr_Read, &stderrWrite, &secAttributes, 0)) {
		res.addEntry("EUKN", "Failed to create stderr pipe", { { "Error Message", GetLastErrorAsString() } });
		return res;
	}
	// make sure the read handle isn't inhereited
	if (!SetHandleInformation(mPimpl->StdErr_Read, HANDLE_FLAG_INHERIT, 0)) {
		res.addEntry("EUKN", "Failed to SetHandleInformation", { { "Error Message", GetLastErrorAsString() } });
		return res;
	}


	// create stdin pipe
	if (!CreatePipe(&stdinRead, &mPimpl->StdIn_Write, &secAttributes, 0)) {
		res.addEntry("EUKN", "Failed to create stdin pipe", { { "Error Message", GetLastErrorAsString() } });
		return res;
	}
	// make sure the read handle isn't inhereited
	if (!SetHandleInformation(mPimpl->StdIn_Write, HANDLE_FLAG_INHERIT, 0)) {
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

	ZeroMemory(&mPimpl->procInfo, sizeof(PROCESS_INFORMATION));

	// setup startupInfo
	STARTUPINFO startupInfo;
	ZeroMemory(&startupInfo, sizeof(STARTUPINFO));

	startupInfo.cb = sizeof(STARTUPINFO);
	startupInfo.hStdError = stderrWrite;
	startupInfo.hStdOutput = stdoutWrite;
	startupInfo.hStdInput = stdinRead;
	startupInfo.dwFlags |= STARTF_USESTDHANDLES;
	
	// create the process
	if (!CreateProcessW(nullptr, &commandLine[0], nullptr, nullptr, TRUE, 0, nullptr, mWorkingDir.c_str(), &startupInfo, &mPimpl->procInfo)) {
		res.addEntry("EUKN", "Failed to CreateProcess", { { "Error Message", GetLastErrorAsString() } });
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

			if (bytesRead == 0) {
				return;
			}

			// send it
			if (mStdOutHandler) {
				mStdOutHandler(buffer.data(), bytesRead);
			}
		}
	});

	// start threads for listening for input
	mPimpl->stderrThread = std::thread([this]() {

		while (true) {

			DWORD bytesRead;

			std::vector<char> buffer;
			buffer.resize(8192);

			if (!ReadFile(mPimpl->StdErr_Read, buffer.data(), buffer.size(), &bytesRead, nullptr)) {
				return;
			}

			if (bytesRead == 0) {
				return;
			}

			// send it
			if (mStdErrHandler) {
				mStdErrHandler(buffer.data(), bytesRead);
			}
		}


	});


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
	WaitForSingleObject(mPimpl->procInfo.hProcess, INFINITE);


	
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
}

// POSIX implementation
#else 


#endif

// Common functions
Subprocess::Subprocess(const boost::filesystem::path& path) : mPimpl{ std::make_unique<Subprocess::Implementation>() }, mExePath{ path }  {}

}
