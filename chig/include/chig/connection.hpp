#pragma once

#include <boost/variant.hpp>

namespace chi {
	
	struct node_instance;
	
	struct data_connection {
		
		node_instance* left;
		size_t left_output_id;
		node_instance* right;
		size_t right_input_id;
		
	};
	
	struct execution_connection {
		node_instance* left;
		node_instance* right;
	};
	
}
