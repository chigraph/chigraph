/// \file ChigModule.cpp

#include "chig/ChigModule.hpp"
#include "chig/Context.hpp"
#include "chig/Result.hpp"

namespace chig {
ChigModule::ChigModule(Context& contextArg, std::string moduleFullName)
    : mFullName{std::move(moduleFullName)}, mContext{&contextArg} {
	// everything after the last / and before the . so russelltg/test/lib.chigmod turns into lib
#ifndef WIN32
	mName = mFullName.substr(mFullName.rfind('/') + 1, mFullName.rfind('.'));
#else

	mName = mFullName.substr(mFullName.rfind('\\') + 1, mFullName.rfind('.'));
#endif
}

Result ChigModule::addDependency(std::string newDepFullPath) {
	Result res = context().loadModule(newDepFullPath);
	
	if (res) {
		mDependencies.emplace(std::move(newDepFullPath));
	}

	return res;
}
}  // namespace chig
