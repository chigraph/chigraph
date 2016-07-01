#ifndef CHIG_NODE_HPP
#define CHIG_NODE_HPP

#pragma once

namespace chig {

struct node {
	// this is module:name
	std::string type;

	std::vector<llvm::Value*> inputs;
	std::vector<llvm::Value*> outputs;

	size_t id;

	node* exec_output;
};

}

#endif // CHIG_NODE_HPP
