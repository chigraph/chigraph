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


#endif

// Common functions
Subprocess::Subprocess(const boost::filesystem::path& path) : mPimpl{ std::make_unique<Subprocess::Implementation>() }, mExePath{ path }  {}
Subprocess::~Subprocess() = default;

}
