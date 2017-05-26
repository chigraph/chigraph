/// \file Subprocess.hpp

#pragma once

#ifndef CHI_SUBPROCESS_HPP
#define CHI_SUBPROCESS_HPP

#include <chi/Fwd.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <vector>
#include <string>
#include <chrono>

namespace chi{

	/// Provides an platform-independent abstraction for creating subprocesses.
	/// On OSX and Linux, this uses the posix api (pipe(), fork(), exec(), write(), read(), etc)
	/// and on windows it uses the win32 API (CreatePipe(), CreateProcess(), etc)
	/// 
	/// Usage is you create a Subprocess class:
	/// 
	struct Subprocess {

		enum PipeType {
			Binary,
			Text,
		};

		using pipeHandler = std::function<void(const char* data, size_t size)>;

		/// \pre `boost::filesystem::is_regular_file(pathToExecutable)`
		Subprocess(const boost::filesystem::path& pathToExecutable);
		
		~Subprocess();

		
		template<typename ForwardIterator>
		void setArguments(const ForwardIterator& begin, const ForwardIterator& end);

		template<typename Range>
		void setArguments(const Range& range);

		const std::vector<std::string>& arguments() { return mArguments; }

		void attachToStdOut(pipeHandler stdOutHandler) { mStdOutHandler = stdOutHandler;  }
		void attachToStdErr(pipeHandler stdErrHandler) { mStdErrHandler = stdErrHandler; }
		
		/// Attach a string to stdout. Everything added to the string is appended.
		/// \param str The string to append stdout to
		/// \note Don't try to read from `str` while the program is running, it could create a race condition.
		/// Io is read in a separate thread
		void attachStringToStdOut(std::string& str) {
			attachToStdOut([&str](const char* data, size_t size) {
				str.append(data, size);
			});
		}
		
		/// Attach a string to stderr. Everything added to the string is appended.
		/// \param str The string to append stderr to
		/// \note Don't try to read from `str` while the program is running, it could create a race condition.
		/// Io is read in a separate thread
		void attachStringToStdErr(std::string& str) {
			attachToStdErr([&str](const char* data, size_t size) {
				str.append(data, size);
			});
		}
		
		

		
		void setWorkingDirectory(boost::filesystem::path newWd) {
			mWorkingDir = std::move(newWd);
		}
		
		/// \pre `started()`
		/// \prre `!isStdInClosed()`
		Result pushToStdIn(const char* data, size_t size);
		
		/// \pre `!isStdInClosed()`
		/// \pre `running()`
		/// \post `isstdInClosed()`
		Result closeStdIn();
		
		bool isStdInClosed() const { return mStdInClosed; }

		/// \pre `!started()`
		Result start();
		
		/// \pre `started()`
		void kill();

		void wait();

		// wait and get exit code
		int exitCode();

		/// \pre `started()`
		bool running() const;
		
		bool started() const { return mStarted; }

	private:
		struct Implementation;
		std::unique_ptr<Implementation> mPimpl;

		std::vector<std::string> mArguments;

		pipeHandler mStdOutHandler;
		pipeHandler mStdErrHandler;

		boost::filesystem::path mExePath;

		boost::filesystem::path mWorkingDir = boost::filesystem::current_path();
		
		bool mStarted = false;
		bool mStdInClosed = false;
	};

	template<typename ForwardIterator>
	inline void Subprocess::setArguments(const ForwardIterator& begin, const ForwardIterator& end)
	{
		mArguments = std::vector<std::string>( begin, end );
	}

	template<typename Range>
	inline void Subprocess::setArguments(const Range& range)
	{
		mArguments = std::vector<std::string>(range.begin(), range.end());
	}

}

#endif // CHI_SUBPROCESS_HPP
