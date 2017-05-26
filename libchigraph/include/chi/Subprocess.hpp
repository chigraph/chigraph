/// \file Subprocess.hpp

#pragma once

#ifndef CHI_SUBPROCESS_HPP
#define CHI_SUBPROCESS_HPP

#include <chi/Fwd.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>

#include <chrono>
#include <string>
#include <vector>

namespace chi {

/// Provides an platform-independent abstraction for creating subprocesses.
/// On OSX and Linux, this uses the POSIX api (`pipe()`, `fork()`, `exec()`, `write()`, `read()`, etc)
/// and on windows it uses the win32 API (`CreatePipe()`, `CreateProcess()`, `ReadFile()`, etc)
///
/// Usage is you create a Subprocess class:
/// \snippet SubprocessExample.cpp Constructing
/// 
/// Now you can use any of the Setup Functions (see above).
/// The string we pass to `attachStringToStdOut` is written from a differnt thread, so it's 
/// not safe to acces it until the program has exited.
/// 
/// \snippet SubprocessExample.cpp Setup
/// 
/// Once you're sure all youre setup variables are correctly set, start the process. 
/// You cannot change any of the setup values once you have called start()
/// 
/// \snippet SubprocessExample.cpp Start
/// 
/// Now that it's started, we can write data (if we wish) to stdin:
/// 
/// \snippet SubprocessExample.cpp Write to stdin
/// 
/// Now, you can use `running()` to see if the process has exited yet. We'll just use `exitCode()` 
/// to wait for the process to complete and get its exit code:
/// 
/// \snippet SubprocessExample.cpp Get Exit Code
/// 
/// Now that the process has exited, it's safe to use the `stdOut` string:
/// 
/// \snippet SubprocessExample.cpp Use byproducts
///
struct Subprocess {

	/// The function type for recieving data from pipes
	using pipeHandler = std::function<void(const char* data, size_t size)>;

	/// Construct a Subprocess with a path to an executable
	/// \pre `boost::filesystem::is_regular_file(pathToExecutable)`
	Subprocess(const boost::filesystem::path& pathToExecutable);

	/// Wait for the process to exit and close all open handles
	~Subprocess();

	/// \name Setup Functions
	/// Setup functions are called after the cosntructor, but before `start()`.
	/// Most of them cannot be called after `start()`.
	/// They are used to set parameters for the program's execution
	/// \{

	/// Set the arguments for the program with a begin and end iterator
	/// \param begin The start iterator of arguments. Should be an iterator of something
	/// converatable to std::string
	/// \param end The end iterator. Should be an iterator of something converatable to std::string
	/// \pre `!started()`
	template <typename ForwardIterator>
	void setArguments(const ForwardIterator& begin, const ForwardIterator& end) {
		// make sure the iterator type is good to go
	static_assert(std::is_convertible<typename std::iterator_traits<ForwardIterator>::value_type,
	                                  std::string>::value,
	              "Cannot run setArguments with iterators not convertable to std::string");

	assert(!started() && "Cannot set arguments after start() has been called");

	mArguments = std::vector<std::string>(begin, end);
	}

	/// Set the arguments for the program with a range
	/// \param range The range. Must have `begin()` and `end()` functions, which return iterators.
	/// \pre `!started()`
	template <typename Range>
	void setArguments(Range&& range) {
		setArguments(std::forward<Range>(range).begin(), std::forward<Range>(range).end());
	}

	/// Set the arguments for the program with an initializer_list
	/// \param init the initializer list
	/// \pre `!started()`
	void setArguments(std::initializer_list<const char*> init) {
		setArguments(init.begin(), init.end());
	}

	/// Get the currently set arguments for the program
	/// \return The arguments
	const std::vector<std::string>& arguments() const { return mArguments; }

	/// Attach a function handler to the child stdout. Every time data is recieved through the
	/// stdout pipe of the child, it will be sent to this handler
	/// \param stdOutHandler The handler
	/// \note `stdOutHandler` will exclusively be called from another thread.
	/// \pre `!started()`
	void attachToStdOut(pipeHandler stdOutHandler) {
		assert(!started() &&
		       "Cannot attach a differnt function to stdout after start() has been called");
		mStdOutHandler = stdOutHandler;
	}

	/// Attach a function handler to the child stderr. Every time data is recieved through the
	/// stderr pipe of the child, it will be sent to this handler
	/// \param stdErrHandler The handler
	/// \note `stdErrHandler` will exclusively be called from another thread.
	/// \pre `!started()`
	void attachToStdErr(pipeHandler stdErrHandler) {
		assert(!started() &&
		       "Cannot attach a differnt function to stderr after start() has been called");
		mStdErrHandler = stdErrHandler;
	}

	/// Attach a string to stdout. Everything added to the string is appended.
	/// This is just a convenicence function, it just calls attachToStdout with a function appending the data to the string.
	/// \param str The string to append stdout to
	/// \note Don't try to read from `str` while the program is running, it could create a race
	/// condition.
	/// \pre `!started()`
	void attachStringToStdOut(std::string& str) {
		attachToStdOut([&str](const char* data, size_t size) { str.append(data, size); });
	}

	/// Attach a string to stderr. Everything added to the string is appended.
	/// This is just a convenicence function, it just calls attachToStdout with a function appending the data to the string.
	/// \param str The string to append stderr to
	/// \note Don't try to read from `str` while the program is running, it could create a race
	/// condition.
	/// \pre `!started()`
	void attachStringToStdErr(std::string& str) {
		attachToStdErr([&str](const char* data, size_t size) { str.append(data, size); });
	}

	/// Set the working directory of the process.
	/// \param newWd The new working directory
	/// \pre `!started()`
	void setWorkingDirectory(boost::filesystem::path newWd) { mWorkingDir = std::move(newWd); }

	/// \}
	
	/// Start the process.
	/// After this is called, most of the Setup Functions cannot be called anymore
	/// \return The Result.
	/// \pre `!started()`
	/// \post `started()`
	Result start();
	
	/// Check if the child has started (`start()` has been called)
	/// \return True if it has, false otherwise
	bool started() const { return mStarted; }

	
	/// \name Runtime Functions
	/// These functions are called while the process has started (ie after start() is called)
	/// \{

	/// Pushes data to the stdin stream of the child process
	/// \param data The pointer to the start of the data
	/// \param size How long the data is
	/// \pre `started()`
	/// \pre `!isStdInClosed()`
	Result pushToStdIn(const char* data, size_t size);

	/// Close the STDIN stream (send an EOF)
	/// \return The Result
	/// \pre `!isStdInClosed()`
	/// \pre `running()`
	/// \post `isStdInClosed()`
	Result closeStdIn();

	/// Checks if the stdin is closed
	/// \return true if it is, false otherwise
	bool isStdInClosed() const { return mStdInClosed; }

	
	/// Kill the child process
	/// On POSIX, this sends SIGINT
	/// \pre `started()`
	void kill();

	/// Wait for the process to complete
	/// \pre `started()`
	void wait();

	/// Wait and gets the exit code
	/// \return The exit code. 0 for success.
	/// \pre `started()`
	int exitCode();

	/// Check if the process is still running
	/// \return true if it's still running, false otherwise
	/// \pre `started()`
	bool running();
	
	/// \}
	

private:
	struct Implementation;
	std::unique_ptr<Implementation> mPimpl;

	std::vector<std::string> mArguments;

	pipeHandler mStdOutHandler;
	pipeHandler mStdErrHandler;

	boost::filesystem::path mExePath;

	boost::filesystem::path mWorkingDir = boost::filesystem::current_path();

	bool mStarted     = false;
	bool mStdInClosed = false;
	
	boost::optional<int> mExitCode;
};

/// \example SubprocessExample.cpp

} // namespace chi

#endif  // CHI_SUBPROCESS_HPP
