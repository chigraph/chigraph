#include <chig/Context.hpp>

using namespace chig;

int main()
{
	Context c;

	// only the lang module should be loaded
	assert(c.modules.size() == 1);


	c.searchPaths.push_back("../");

	assert(c.modules.size() == 1);  // lang and add_test.bc and add_tester.bc
//	assert(c.modules[1].get() == mod);
//	assert(c.getModuleByName("add_test.bc") == mod);
	assert(c.getModuleByName("yourmum") == nullptr);

//	assert(c.unloadModule(mod));
//	assert(!c.unloadModule(mod)); // shouldn't unload the second time



}
