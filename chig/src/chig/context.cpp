#include "chig/context.hpp"
#include "chig/built_in_types.hpp"
#include "chig/node.hpp"

#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/BasicBlock.h"


#include <cstdlib>

#include <boost/filesystem.hpp>

using namespace llvm;

namespace chig {

std::unique_ptr<node> create_node(size_t id, std::vector<nlohmann::json>& nodes, std::vector<nlohmann::json>& connections) {

	auto node = std::make_unique<node>();

	auto node_json = nodes[id];

	node->type = node_json["type"];


}

Module* context::compile_json(const nlohmann::json& input_json) {
	std::string module_name = input_json["module"]["name"];
	auto mod = std::make_unique<Module>(module_name, m_context);

	// load dependencies
	for(auto dependency : input_json["module"]["dependencies"]) {
		load_module(dependency);
	}

	// compile the graphs
	for(const nlohmann::json& json_graph : input_json["graph"]) {
		// TODO: support more types
		assert(json_graph["type"] == "function");

		std::string fn_name = json_graph["name"];

		// parse inputs and outputs, and get their names
		std::vector<Type*> types;
		std::vector<std::string> names;
		auto inputs = json_graph["inputs"];
		for(auto iterator = inputs.begin(); iterator != inputs.end(); ++iterator) {
			// see if it is a built-in type
			Type* ty = get_built_in_type(iterator.value(), m_context);
			// if it wasn't a built-in type then it is from a module
			if(!ty) {
				ty = get_type_by_name(iterator.value());
			}
			types.emplace_back(ty);
			names.push_back(iterator.key());
		}
		// do the same with inputs, but add pointer
		auto outputs = json_graph["outputs"];
		for(auto iterator = outputs.begin(); iterator != outputs.end(); ++iterator) {
			// see if it is a built-in type
			Type* ty = get_built_in_type(iterator.value(), m_context);
			// if it wasn't a built-in type then it is from a module
			if(!ty) {
				ty = get_type_by_name(iterator.value());
			}
			// add pointer
			ty = PointerType::get(ty, 0);
			types.emplace_back(ty);
			names.push_back(iterator.key());
		}

		FunctionType* func_type = FunctionType::get(llvm::Type::getInt64Ty(m_context), types, false);

		Function* llfunc = llvm::cast<Function>(mod->getOrInsertFunction(fn_name, func_type));
		llfunc->setCallingConv(CallingConv::C);

		// set the names
		auto nameiter = names.begin();
		for(auto& arg : llfunc->args()) {
			arg.setName(*nameiter);
			++nameiter;
		}

		// start going through the json graph
		std::vector<nlohmann::json> json_nodes = json_graph["nodes"];
		std::vector<nlohmann::json> json_connections = json_graph["conections"];

		std::vector<std::unique_ptr<node>> nodes(json_nodes.size());

		// find the lang:entry node
		size_t entry_node_id = ~0;
		for(size_t node_id = 0; node_id != json_nodes.size(); ++node_id) {
			if(json_nodes[node_id]["type"] == "lang:entry") {
				entry_node_id = node_id;
			}
		}
		// if we didn't find it
		if(entry_node_id = ~0) {
			throw std::runtime_error("Failed to find a \"lang:entry\" node");
		}

		// start real codegen!

		//

	}
}

void context::load_module(const std::string& name) {

	// find absolute path
	boost::filesystem::path abs_path;
	// look in the current directory
	if(boost::filesystem::is_regular_file(name + ".bc")) {
		abs_path = name + ".bc";
	} else {
		for(const auto& p : m_search_paths) {
			if(boost::filesystem::is_regular_file(p + "/" + name + ".bc")) {
				abs_path = p + "/" + name + ".bc";
				break;
			}
		}
	}

	// if we haven't found it, throw
	if(abs_path.empty()) {
		throw std::runtime_error("Unable to find .bc file named: " + name);
	}



	// load it up

	// get the memory buffer
	auto buffer_or_err = MemoryBuffer::getFile(abs_path.string());
	if(!buffer_or_err) {
		throw std::runtime_error("Error opening memory buffer");
	}

	// parse that into a module
	auto module_or_err = parseBitcodeFile(*buffer_or_err.get(), m_context);
	if(!module_or_err) {
		throw std::runtime_error("Failed to load module: " + name);
	}

	// put that in our vector
	m_modules.push_back(std::move(module_or_err.get()));
}

Module* context::get_module_by_name(const std::string &name) {
	// find the module
	llvm::Module* mod = nullptr;
	for(const std::unique_ptr<Module>& module : m_modules) {
		if(module->getName() == name) {
			mod = module.get();
			break;
		}
	}

	// return it, even if it is null
	return mod;
}

Function* context::get_function_by_name(const std::string& s) {

	// the format is module:name so do that
	std::string module_name = s.substr(0, s.find(":"));
	std::string func_name = s.substr(s.find(":") + 1);

	// find the module
	Module* mod = get_module_by_name(module_name);
	// make sure we found it
	if(!mod) {
		throw std::runtime_error("Failed to find module: " + module_name + " when trying to get llvm::Function* for " + s);
	}

	auto func = mod->getFunction(func_name);
	// TODO: also get the metadata

	return func;
}

Type* context::get_type_by_name(const std::string &name) {
	// the format is module:name so do that
	std::string module_name = name.substr(0, name.find(":"));
	std::string type_name = name.substr(name.find(":") + 1);


	int num_ptrs = 0;
	// remove the pointers
	while(type_name[type_name.length() - 1] = '*') {
		++num_ptrs;
		type_name.pop_back();
	}

	Module* module = get_module_by_name(module_name);

	Type* type = module->getTypeByName(type_name);

	// add back pointers
	while(num_ptrs) {
		type = PointerType::get(type, 0);
		--num_ptrs;
	}

	return type;
}

}
