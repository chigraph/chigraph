#include <chig/Context.hpp>
#include <chig/NodeType.hpp>
#include <chig/LangModule.hpp>
#include <chig/GraphFunction.hpp>

#include <boost/program_options.hpp>

#include <llvm/Support/raw_os_ostream.h>

#include <iostream>
#include <vector>
#include <string>

using namespace chig;

int main(int argc, char** argv) {
	
	Context c;
	c.addModule(std::make_unique<LangModule>(c));

	std::unique_ptr<GraphFunction> func;
	
	auto jsondata = R"ENDJSON(
		{
				  "module": {
						      "name": "main",
							      "dependencies": [
								        "io"
										    ]
											  },
											    "graphs": [
												    {
															      "type": "function", 
																        "name": "main",
																		      "nodes": [
																			          {
																							            "type": "lang:entry",
																										          "location": [0, 0],
																												            "data": {
																																	            "input": "lang:i32"
																																				          }
																																						          },
																																								          {
																																												            "type": "lang:exit",
																																															          "location": [10, 0],
																																																	            "data": {
																																																						            "output": "lang:i32"
																																																									          }
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
																																																																																														  ]
		}
	)ENDJSON"_json;

	Result r = GraphFunction::fromJSON(c, jsondata["graphs"][0], &func);
	std::unique_ptr<llvm::Module> mod = std::make_unique<llvm::Module>("hello", c.context);
	auto llfunc = func->compile(mod.get());

	llvm::raw_os_ostream stream(std::cout);
	mod->print(stream, nullptr);

	namespace po = boost::program_options;
	
	po::options_description desc("chigc: Chigraph compiler command line parameters", 50);
	
	desc.add_options()
		("help,h", "Produce Help Message")
		("input-file", po::value<std::vector<std::string>>(), "Input files")
		("output", po::value<std::string>()->default_value("a.bc"), "Output File")
		;
	
	po::positional_options_description input_desc;
	input_desc.add("input-file", -1);
	
	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).positional(input_desc).run(), vm);
	
	po::notify(vm);
	
	// see help
	if(vm.count("help")) {
		std::cout << desc << std::endl;
		return 0;
	}
	
	if(vm.count("input-file") == 0) {
		std::cerr << "error: no input files" << std::endl;
		return 1;
	}
	
	
	
}
