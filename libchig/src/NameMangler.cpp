#include "chig/NameMangler.hpp"

#include <vector>

namespace chig {
    
  std::string mangleFunctionName(gsl::cstring_span<> fullModuleName, gsl::cstring_span<> name) {
    
    std::string modName = gsl::to_string(fullModuleName);
    // escape characters
    
    
    // escape _
    size_t id = modName.find('_');
    while(id != std::string::npos) {
      
      modName.replace(id, 1, "__");
      
      id = modName.find('_', id + 2);
    }
    
    // escape /
    id = modName.find('/');
    while(id != std::string::npos) {
      
      modName.replace(id, 1, "_s");
      
      id = modName.find('/', id);
    }
    
    // escape .
    id = modName.find('.');
    while(id != std::string::npos) {
      
      modName.replace(id, 1, "_d");
      
      id = modName.find('.', id);
    }
    
    return modName + "_m" + gsl::to_string(name);
    
  }

  std::pair<std::string, std::string> unmangleFunctionName(gsl::cstring_span<> mangledName) {
    
    std::string mangled = gsl::to_string(mangledName);
    
    size_t splitter = mangled.find("_m");
    std::string modName = mangled.substr(0, splitter);
    std::string typeName = mangled.substr(splitter + 2);
    
    int id = -1;
    while((id = modName.find('_', id + 1)) != std::string::npos && modName.size() >= id) {
      switch(modName[id + 1]) {
        case '_':
          modName.replace(id, 2, "_"); break;
        case 's':
          modName.replace(id, 2, "/"); break;
        case 'd':
          modName.replace(id, 2, "."); break;
        default:
          assert(false); // TODO: error handling
      }
    }
    
    return {modName, typeName};
  }

}
