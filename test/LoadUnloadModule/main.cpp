#include <chig/Context.hpp>

using namespace chig;

int main() {
	
	Context c;
	
	assert(c.modules.size() == 0);
	
	auto mod = c.loadModule("add_test.bc");
	
	assert(c.modules.size() == 1);
	assert(c.modules[0].get() == mod);
	assert(c.modules[0]->nodes.size() == 1);
	assert(c.getModuleByName("add_test.bc") == mod);
	
	c.unloadModule(mod);
	
	assert(c.modules.size() == 0);
	
	// load a module that doens't exist
	try {
		c.loadModule("wtfisthis");
		assert(false);
	} catch(std::exception& e) {
		
	}
	
	assert(c.modules.size() == 0);
	
}
