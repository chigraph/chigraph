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

	struct Subprocess {

		enum PipeType {
			Binary,
			Text,
		};

		using pipeHandler = std::function<void(const char* data, size_t size)>;

		Subprocess(const boost::filesystem::path& pathToExecutable);
		~Subprocess();

		template<typename ForwardIterator>
		void setArguments(const ForwardIterator& begin, const ForwardIterator& end);

		template<typename Range>
		void setArguments(Range&& range);

		void setArguments(std::initializer_list<const char*> init);

		const std::vector<std::string>& arguments() { return mArguments; }

		void attachToStdOut(pipeHandler stdOutHandler) { mStdOutHandler = stdOutHandler;  }
		void attachToStdErr(pipeHandler stdErrHandler) { mStdErrHandler = stdErrHandler; }


		void setWorkingDirectory(boost::filesystem::path newWd) {
			mWorkingDir = std::move(newWd);
		}
		
		Result pushToStdIn(const char* data, size_t size);
		Result closeStdIn();

		Result start();
		void kill();

		void wait();

		// wait and get exit code
		int exitCode();

		bool running();

		void wait_for(std::chrono::milliseconds duration);

	private:
		struct Implementation;
		std::unique_ptr<Implementation> mPimpl;

		std::vector<std::string> mArguments;

		pipeHandler mStdOutHandler;
		pipeHandler mStdErrHandler;

		boost::filesystem::path mExePath;

		boost::filesystem::path mWorkingDir = boost::filesystem::current_path();
	};

	template<typename ForwardIterator>
	inline void Subprocess::setArguments(const ForwardIterator& begin, const ForwardIterator& end)
	{
		mArguments = std::vector<std::string>( begin, end );
	}

	template<typename Range>
	inline void Subprocess::setArguments(Range&& range)
	{
		mArguments = std::vector<std::string>(std::forward<Range>(range).begin(), std::forward<Range>(range).end());
	}

	inline void Subprocess::setArguments(std::initializer_list<const char*> init)
	{
		mArguments = std::vector<std::string>(init.begin(), init.end());
	}

}

#endif // CHI_SUBPROCESS_HPP
