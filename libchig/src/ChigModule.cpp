#include "chig/ChigModule.hpp"

#include "chig/Context.hpp"

namespace chig {
ChigModule::ChigModule(Context& contextArg, std::string moduleFullName)
    : mFullName{std::move(moduleFullName)}, mContext{&contextArg} {
	// everything after the last / and before the . so russelltg/test/lib.chigmod turns into lib
	mName = mFullName.substr(mFullName.rfind('/') + 1, mFullName.rfind('.'));
}

Result ChigModule::addDependency(std::string newDepFullPath) {
	Result res = context().loadModule(newDepFullPath);
	mDependencies.emplace(std::move(newDepFullPath));

	return res;
}
}  // namespace chig
