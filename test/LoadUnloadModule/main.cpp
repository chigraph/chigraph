#include <chig/Context.hpp>

using namespace chig;

int main()
{
	Context c;

	// only the lang module should be loaded
	assert(c.modules.size() == 1);

	auto mod = c.loadModuleFromBc("add_test.bc");

	c.searchPaths.push_back("../");
	auto mod2 = c.loadModuleFromBc("test/add_test.bc"); 

	assert(c.modules.size() == 3);  // lang and add_test.bc and add_tester.bc
	assert(c.modules[1].get() == mod);
	assert(static_cast<ImportedModule*>(c.modules[1].get())->nodes.size() == 1);
	assert(c.getModuleByName("add_test.bc") == mod);
	assert(c.getModuleByName("yourmum") == nullptr);

	assert(c.unloadModule(mod));
	assert(!c.unloadModule(mod)); // shouldn't unload the second time


	assert(c.modules.size() == 2);

	// load a module that doens't exist
	try {
		c.loadModuleFromBc("wtfisthis");
		assert(false);
	} catch (std::exception& e) {
	}

	assert(c.modules.size() == 2);
}
