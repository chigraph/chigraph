#pragma once

#include "chig/node_type.hpp"

#include <vector>
#include <string>

namespace chi {

// stores a bunch of node_types and TODO: data types
struct module {
	
	void add_node(executed_node_type type, std::string name) {
		
	}
	
	void add_node(pure_node_type type, std::string name) {
		
	}
	
	
	
	std::vector<executed_node_type> m_executed_nodes;
	std::vector<pure_node_type> m_pure_nodes;
	
};

}
