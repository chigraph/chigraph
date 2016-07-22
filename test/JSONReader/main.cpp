#include <chig/Context.hpp>
#include <chig/GraphFunction.hpp>

#include <fstream>

using namespace chig;

int main() {
	
	Context c;
	
	// read the JSON
	
	std::ifstream t("helloworld.chig");
	assert(t);
	std::string str((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
	
	auto data = nlohmann::json::parse(str);
	
	auto graph = GraphFunction::fromJSON(c, data["graphs"][0]);
	
	assert(graph.nodes.size() == 2);
	assert(graph.nodes[0]->type->module == "lang");
	assert(graph.nodes[0]->type->name == "entry");
	assert(graph.nodes[0]->type->dataOutputs.size() == 1);
	assert(c.stringifyType(graph.nodes[0]->type->dataOutputs[0].first) == "i32*");
	assert(graph.nodes[0]->type->dataOutputs[0].second == "input");
	
	assert(graph.nodes[0]->outputExecConnections.size() == 1);
	assert(graph.nodes[0]->outputExecConnections[0].first == graph.nodes[1].get());
	assert(graph.nodes[0]->outputExecConnections[0].second == 0);
	
	assert(graph.nodes[0]->outputDataConnections.size() == 1);
	assert(graph.nodes[0]->outputDataConnections[0].first == graph.nodes[1].get());
	assert(graph.nodes[0]->outputDataConnections[0].second == 0);
	
	assert(graph.nodes[1]->type->module == "lang");
	assert(graph.nodes[1]->type->name == "exit");
	
	assert(graph.nodes[1]->inputDataConnections.size() == 1);
	assert(graph.nodes[1]->inputDataConnections[0].first == graph.nodes[0].get());
	assert(graph.nodes[1]->inputDataConnections[0].second == 0);
	
	assert(graph.nodes[1]->inputExecConnections.size() == 1);
	assert(graph.nodes[1]->inputExecConnections[0].first == graph.nodes[0].get());
	assert(graph.nodes[1]->inputExecConnections[0].second == 0);
	
}

