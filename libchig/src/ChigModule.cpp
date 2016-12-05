#include "chig/ChigModule.hpp"

#include "chig/Context.hpp"

using namespace chig;

ChigModule::ChigModule(Context& contextArg, std::string fullName) : mContext{&contextArg} {
  
  mFullName = std::move(fullName); 
  
  // everything after the last / and before the . so russelltg/test/lib.chigmod turns into lib
  mName = mFullName.substr(mFullName.rfind('/') + 1, mFullName.rfind('.'));

}
