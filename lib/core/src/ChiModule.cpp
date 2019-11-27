/// \file ChiModule.cpp

#include "chi/ChiModule.hpp"

#include "chi/Context.hpp"
#include "chi/Support/Result.hpp"

namespace chi {
ChiModule::ChiModule(Context& contextArg, std::filesystem::path moduleFullName)
    : mFullName{std::move(moduleFullName)}, mContext{&contextArg} {
	mName = mFullName.filename().string();
}

Result ChiModule::addDependency(std::filesystem::path newDepFullPath) {
	Result res = context().loadModule(newDepFullPath);

	if (res) { mDependencies.emplace(std::move(newDepFullPath)); }

	return res;
}
}  // namespace chi
