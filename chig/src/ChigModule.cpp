#include "chig/ChigModule.hpp"

#include "chig/Context.hpp"

using namespace chig;

// ChigModule::~ChigModule() {
// 	// remove from context
// 	
// 	auto iter = std::find_if(context->modules.begin(), context->modules.end(), [this](auto& ptr) { return this == ptr.get(); });
// 	
// 	assert(iter != context->modules.end());
// 	
// 	context->modules.erase(iter);
// }
