#include "chig/JsonModule.hpp"

#include "chig/Result.hpp"
#include "chig/GraphFunction.hpp"
#include "chig/NodeType.hpp"

using namespace chig;

JsonModule::JsonModule(const nlohmann::json& json_data, Context& cont, Result* r) : ChigModule(cont)
{
	// load name
	{
		auto iter = json_data.find("name");
		if(iter == json_data.end()) {
			r->add_entry("E34", "No name element in module", {});
			return;
		}
		if(!iter->is_string()) {
			r->add_entry("E35", "name element in module isn't a string", {});
			return;
		}
		name = *iter;
	}
	// load dependencies
	{
		auto iter = json_data.find("dependencies");
		if(iter == json_data.end()) {
			r->add_entry("E34", "No dependencies element in module", {});
			return;
		}
		if(!iter->is_array()) {
			r->add_entry("E36", "dependencies element isn't an array", {});
			return;
		}
		dependencies.reserve(iter->size());
		for(auto dep : *iter) {
			if(!dep.is_string()) {
				r->add_entry("E37", "dependency isn't a string", {{"Actual Data", dep}});
				continue;
			}
			dependencies.push_back(dep);
		}
	}
	// load graphs
	{
		auto iter = json_data.find("graphs");
		if(iter == json_data.end()) {
			r->add_entry("E38", "no graphs element in module", {});
			return;
		}
		if(!iter->is_array()) {
			r->add_entry("E39", "graph element isn't an array", {{"Actual Data", *iter}});
			return;
		}
		functions.reserve(iter->size());
		for(auto graph : *iter) {
			std::unique_ptr<GraphFunction> newf;
			*r += GraphFunction::fromJSON(*context, graph, &newf);
			functions.push_back(std::move(newf));
		}
		
	}
}


Result JsonModule::compile(std::unique_ptr<llvm::Module>* mod) const {
	// create llvm module
	*mod = std::make_unique<llvm::Module>(name, context->llcontext);
	
	Result res;
	
	for(auto& graph : functions) {
		llvm::Function* f;
		res += graph->compile(mod->get(), &f);
	}
	
	return res;
}
