#pragma once

#include <llvm/IR/Function.h>

namespace chi {
	
	struct node_dock {
		llvm::Type* m_type;
		std::string m_desc;
	};
	
	struct executed_node_type {
		
		// these functions are special. Because they can have multiple output execution paths, their return type is the ID of the execute output. 
		std::vector<llvm::Function*> m_execute_inputs;
		std::vector<std::string> m_execute_outptus;
		
		std::vector<node_dock> m_inputs;
		std::vector<node_dock> m_outputs;
		
	};
	
	struct pure_node_type {
		
		// pure functions have a `void` return--there are no options for output
		llvm::Function* m_function;
		
		std::vector<node_dock> m_inputs;
		std::vector<node_dock> m_outputs;
		
		
	};
	
}

