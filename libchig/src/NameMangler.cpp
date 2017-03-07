/// \file NameMangler.cpp

#include "chig/NameMangler.hpp"

#include <vector>

namespace chig {
std::string mangleFunctionName(std::string modName, const std::string& name) {
	// escape characters

	// if it is the special main module, then just return main
	if ((modName.substr(modName.rfind('/') + 1, modName.rfind('.')) == "main" ||
	     modName.substr(modName.rfind('\\') + 1, modName.rfind('.')) == "main") &&
	    name == "main") {
		return "main";
	}

	// escape _
	size_t id = modName.find('_');
	while (id != std::string::npos) {
		modName.replace(id, 1, "__");

		id = modName.find('_', id + 2);
	}

	// escape /
	id = modName.find('/');
	while (id != std::string::npos) {
		modName.replace(id, 1, "_s");

		id = modName.find('/', id);
	}

	// escape .
	id = modName.find('.');
	while (id != std::string::npos) {
		modName.replace(id, 1, "_d");

		id = modName.find('.', id);
	}

	return modName + "_m" + name;
}

std::pair<std::string, std::string> unmangleFunctionName(std::string mangled) {
	if (mangled == "main") { return {"main", "main"}; }

	size_t      splitter = mangled.find("_m");
	std::string modName  = mangled.substr(0, splitter);
	std::string typeName = mangled.substr(splitter + 2);

	size_t id = 0;
	while ((id = modName.find('_', static_cast<size_t>(id))) != std::string::npos &&
	       modName.size() >= id) {
		switch (modName[static_cast<size_t>(id + 1)]) {
		case '_': modName.replace(id, 2, "_"); break;
		case 's': modName.replace(id, 2, "/"); break;
		case 'd': modName.replace(id, 2, "."); break;
		default:
			Expects(false);  // TODO: error handling
		}

		++id;
	}

	return {modName, typeName};
}
}  // namespace chig
