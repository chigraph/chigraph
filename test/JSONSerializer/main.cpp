#include <chig/Context.hpp>
#include <chig/GraphFunction.hpp>
#include <chig/LangModule.hpp>

using namespace chig;

int main() {
	
	// create some random nodes
	Context c;
	
	GraphFunction func(c, "hello");
	
	std::vector<std::pair<llvm::Type*, std::string>> inputs = {{llvm::Type::getInt32Ty(c.context), "in1"}};
	auto entry = func.insertNode(std::make_unique<EntryNodeType>(c, inputs), 32, 32);
	auto ifNode = func.insertNode(c.getNodeType("lang", "if", {}), 44.f, 23.f);
	
	
	connectExec(*entry, 0, *ifNode, 0);
	connectData(*entry, 0, *ifNode, 0);
	
	// dump to JSON, make sure it's legit
	auto dumpedJSON = func.toJSON();
	
	using namespace nlohmann;
	
	auto correctJSON = R"ENDJSON(
{
  "type": "function",
  "name": "hello",
  "nodes": [
    {
      "type": "lang:entry",
      "location": [32.0,32.0],
      "data": {
        "in1": "lang:i32"
      }
    },
    {
      "type": "lang:if",
      "location": [44.0,23.0],
      "data": null
    }
  ],
  "connections": [
    {
      "type": "exec",
      "input": [0,0],
      "output": [1,0]
    },
    {
      "type": "data",
      "input": [0,0],
      "output": [1,0]
    }
  ]
}
)ENDJSON"_json;

	if(dumpedJSON != correctJSON) {
		std::cout << dumpedJSON << std::endl << std::endl << "is not the same as\n\n\n" << correctJSON;
		return 1;
	}

}

