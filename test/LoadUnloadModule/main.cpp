#include <chig/Context.hpp>

using namespace chig;

int main()
{
	Context c;

	// only the lang module should be loaded
	assert(c.modules.size() == 1);

	auto mod = c.loadModuleFromBc("add_test.bc");

	assert(c.modules.size() == 2);  // lang and add_test.bc
	assert(c.modules[1].get() == mod);
	assert(static_cast<ImportedModule*>(c.modules[1].get())->nodes.size() == 1);
	assert(c.getModuleByName("add_test.bc") == mod);

	c.unloadModule(mod);
	
	try {
		c.unloadModule(reinterpret_cast<ChigModule*>(343ull));

		assert(false);
	} catch(std::exception&) {
		
	} 

	assert(c.modules.size() == 1);

	// load a module that doens't exist
	try {
		c.loadModuleFromBc("wtfisthis");
		assert(false);
	} catch (std::exception& e) {
	}

	assert(c.modules.size() == 1);
}
