/// \file ChiModule.cpp

#include "ChiModule.hpp"
#include "Context.hpp"

#include "chi/Support/Result.hpp"

namespace chi {
ChiModule::ChiModule(Context& contextArg, boost::filesystem::path moduleFullName)
    : mFullName{std::move(moduleFullName)}, mContext{&contextArg} {
	mName = mFullName.filename().string();
}

Result ChiModule::addDependency(boost::filesystem::path newDepFullPath) {
	Result res;
	
	auto resCtx = res.addScopedContext({{"Adding Dependent Module", newDepFullPath.string()}, 
	{"Requiring Module", fullName()}});

	res += context().loadModule(newDepFullPath);

	if (res) { mDependencies.emplace(std::move(newDepFullPath)); }

	return res;
}
}  // namespace chi
