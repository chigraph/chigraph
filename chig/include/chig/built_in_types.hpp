#ifndef CHIG_BUILT_IN_TYPES_HPP
#define CHIG_BUILT_IN_TYPES_HPP

#pragma once

#include "llvm/IR/Type.h"

namespace chig {

llvm::Type* get_built_in_type(std::string s, llvm::LLVMContext& context) {
	// check if it is a pointer
	int num_pointers = 0;
	while(s[s.length() - 1] == '*') {
		++num_pointers;
		s.pop_back();
	}
	llvm::Type* ty = nullptr;
	if(s == "float") {
		ty = llvm::Type::getDoubleTy(context);
	} else if (s == "int"){
		ty = llvm::Type::getInt64Ty(context);
	} else if (s == "bool") {
		ty = llvm::Type::getInt1Ty(context);
	}

	// add those pointers
	while(num_pointers) {
		ty = llvm::PointerType::get(ty, 0);
		--num_pointers;
	}
}

}

#endif // CHIG_BUILT_IN_TYPES_HPP
