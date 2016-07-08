#include <chig/Context.hpp>
#include <chig/GraphFunction.hpp>

using namespace chig;

int main() {
	
	// create some random nodes
	Context c;
	
	GraphFunction func(c, "hello", {{llvm::Type::getInt32Ty(c.context), "in1"}}, {{llvm::Type::getInt1PtrTy(c.context), "out"}} );
	auto ifNode = func.insertNode(std::make_unique<IfNodeType>(), 44.f, 23.f);
	
	connectExec(*func.entry, 0, *ifNode, 0);
	connectData(*func.entry, 0, *ifNode, 0);
	
	// dump to JSON, make sure it's legit
	auto dumpedJSON = func.toJSON();
	
	using namespace nlohmann;
	
	auto correctJSON = R"ENDJSON(
{
  "type": "function",
  "name": "hello",
  "inputs": {
    "in1": "i32"
  },
  "outputs": {
    "out": "i1*"
  },
  "nodes": [
    {
      "type": "lang:entry",
      "location": [0.0,0.0]
    },
    {
      "type": "lang:if",
      "location": [44.0,23.0]
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

