/// \file ChigModule.cpp

#include "chig/ChigModule.hpp"
#include "chig/Context.hpp"
#include "chig/Result.hpp"

namespace chig {
ChigModule::ChigModule(Context& contextArg, boost::filesystem::path moduleFullName)
    : mFullName{std::move(moduleFullName)}, mContext{&contextArg} {

	mName = mFullName.filename().string();
}

Result ChigModule::addDependency(const boost::filesystem::path& newDepFullPath) {
	Result res = context().loadModule(newDepFullPath);

	if (res) { mDependencies.emplace(std::move(newDepFullPath)); }

	return res;
}
}  // namespace chig
