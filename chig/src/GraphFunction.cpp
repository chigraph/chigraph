#include "chig/GraphFunction.hpp"

#include <llvm/Support/raw_ostream.h>

using namespace chig;

GraphFunction::GraphFunction(const nlohmann::json& data) {
	// TODO: implement
}

nlohmann::json GraphFunction::toJSON() {
	auto jsonData = nlohmann::json{};
	
	jsonData["type"] = "function";
	jsonData["name"] = graphName;
	
	// create inputs vector
	std::vector<std::string> inputNames(inputs.size());
	for(size_t i = 0; i < inputs.size(); ++i) {
		llvm::raw_string_ostream stream{inputNames[i]};
		inputs[i]->print(stream);
	}
	jsonData["inputs"] = inputNames;
	
	// create outputs vector
	std::vector<std::string> outputNames(outputs.size());
	for(size_t i = 0; i < outputs.size(); ++i) {
		llvm::raw_string_ostream stream{outputNames[i]};
		outputs[i]->print(stream);
	}
	jsonData["outputs"] = outputNames;
	
	// serialize the nodes
	auto& jsonNodes = jsonData["nodes"];
	auto& jsonConnections = jsonData["connections"];
	
	for(auto& node : nodes) {
		jsonNodes.push_back({
			{"type", node->type->module + ':' + node->type->name},
			{"location", {node->x, node->y}}
		});
		// add its connections. Just out the outputs to avoid duplicates
		
		
		
	}
	
	return jsonData;
}

llvm::Module* GraphFunction::compile() {
}

NodeInstance* GraphFunction::insertNode(NodeType* type, float x, float y) {
	auto ptr = std::make_unique<NodeInstance>(type);
	ptr->x = x;
	ptr->y = y;
	
	nodes.push_back(std::move(ptr));
	
	return nodes[nodes.size() - 1].get();
}

