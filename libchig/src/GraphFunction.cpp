#include "chig/GraphFunction.hpp"
#include "chig/JsonModule.hpp"
#include "chig/NameMangler.hpp"

#include <llvm/AsmParser/Parser.h>
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/SourceMgr.h>

#include <boost/dynamic_bitset.hpp>
#include <boost/range/counting_range.hpp>
#include <boost/range/join.hpp>

namespace chig {
GraphFunction::GraphFunction(JsonModule& mod, gsl::cstring_span<>		   name,
							 std::vector<std::pair<DataType, std::string>> dataIns,
							 std::vector<std::pair<DataType, std::string>> dataOuts,
							 std::vector<std::string> execIns, std::vector<std::string> execOuts)
	: mModule{&mod},
	  mContext{&mod.context()},
	  mName{gsl::to_string(name)},
	  mDataInputs(std::move(dataIns)),
	  mDataOutputs(std::move(dataOuts)),
	  mExecInputs(std::move(execIns)),
	  mExecOutputs(std::move(execOuts)) {
	// TODO: check that it has at least 1 exec input and output
}

GraphFunction::~GraphFunction() = default;

GraphFunction::GraphFunction(JsonModule& module, const nlohmann::json& data, Result& res) {
	if (!data.is_object()) {
		res.addEntry("E1", "Graph json isn't a JSON object", {});
		return;
	}
	// make sure it has a type element
	if (data.find("type") == data.end()) {
		res.addEntry("E2", R"(JSON in graph doesn't have a "type" element)", {});
		return;
	}
	if (data["type"] != "function") {
		res.addEntry("E3", "JSON in graph doesn't have a function type", {});
		return;
	}
	// make sure there is a name
	if (data.find("name") == data.end()) {
		res.addEntry("E4", "JSON in graph doesn't have a name parameter", {});
		return;
	}
	std::string name = data["name"];

	if (data.find("data_inputs") == data.end() || !data["data_inputs"].is_array()) {
		res.addEntry("E43", "JSON in graph doesn't have an data_inputs array", {});
		return;
	}

	std::vector<std::pair<DataType, std::string>> datainputs;
	for (auto param : data["data_inputs"]) {
		for (auto iter = param.begin(); iter != param.end(); ++iter) {
			std::string qualifiedType = iter.value();
			std::string docString	 = iter.key();

			std::string moduleName, name;
			std::tie(moduleName, name) = parseColonPair(qualifiedType);

			DataType ty;
			res += module.context().typeFromModule(moduleName, name, &ty);

			if (!res) { return; }

			datainputs.emplace_back(ty, docString);
		}
	}

	if (data.find("data_outputs") == data.end() || !data["data_outputs"].is_array()) {
		res.addEntry("E44", "JSON in graph doesn't have an data_outputs array", {});
		return;
	}

	std::vector<std::pair<DataType, std::string>> dataoutputs;
	for (auto param : data["data_outputs"]) {
		for (auto iter = param.begin(); iter != param.end(); ++iter) {
			std::string qualifiedType = iter.value();
			std::string docString	 = iter.key();

			std::string moduleName, name;
			std::tie(moduleName, name) = parseColonPair(qualifiedType);

			DataType ty;
			res += module.context().typeFromModule(moduleName, name, &ty);

			if (!res) { return; }

			dataoutputs.emplace_back(ty, docString);
		}
	}

	// get exec I/O
	if (data.find("exec_inputs") == data.end() || !data["exec_inputs"].is_array()) {
		res.addEntry("EUKN", "JSON in graph doesn't have an exec_inputs array", {});
		return;
	}

	std::vector<std::string> execinputs;
	for (const auto& param : data["exec_inputs"]) {
		std::string name = param;

		execinputs.emplace_back(name);
	}

	if (data.find("exec_outputs") == data.end() || !data["exec_outputs"].is_array()) {
		res.addEntry("EUKN", "JSON in graph doesn't have an data_outputs array", {});
		return;
	}

	std::vector<std::string> execoutputs;
	for (const auto& param : data["exec_outputs"]) {
		std::string name = param;

		execoutputs.emplace_back(name);
	}

	// construct it
	mModule		 = &module;
	mContext	 = &module.context();
	mName		 = name;
	mDataInputs  = std::move(datainputs);
	mDataOutputs = std::move(dataoutputs);
	mExecInputs  = std::move(execinputs);
	mExecOutputs = std::move(execoutputs);

	mSource = data;
}

Result GraphFunction::toJSON(nlohmann::json* toFill) const {
	Result res;

	*toFill		   = nlohmann::json{};
	auto& jsonData = *toFill;

	jsonData["type"] = "function";
	jsonData["name"] = name();

	auto& datainputsjson = jsonData["data_inputs"];
	datainputsjson		 = nlohmann::json::array();

	for (auto& in : dataInputs()) {
		datainputsjson.push_back({{in.second, in.first.qualifiedName()}});
	}

	auto& dataoutputsjson = jsonData["data_outputs"];
	dataoutputsjson		  = nlohmann::json::array();

	for (auto& out : dataOutputs()) {
		dataoutputsjson.push_back({{out.second, out.first.qualifiedName()}});
	}

	auto& execinputsjson = jsonData["exec_inputs"];
	execinputsjson		 = nlohmann::json::array();

	for (auto& in : execInputs()) { execinputsjson.push_back(in); }

	auto& execoutputsjson = jsonData["exec_outputs"];
	execoutputsjson		  = nlohmann::json::array();

	for (auto& out : execOutputs()) { execoutputsjson.push_back(out); }

	res += graph().toJson(toFill);

	return res;
}

struct cache {
	std::vector<llvm::Value*>	  outputs;
	std::vector<llvm::BasicBlock*> inputBlock;  // one for each exec input
};

// Codegens a single input to a node
void codegenHelper(NodeInstance* node, unsigned execInputID, llvm::BasicBlock* block,
				   llvm::BasicBlock* allocblock, llvm::Module* mod, llvm::DIBuilder* dbuilder,
				   llvm::Function* f, llvm::DISubprogram*	diFunc,
				   std::unordered_map<NodeInstance*, cache>& nodeCache, Result& res,
				   boost::bimap<unsigned, NodeInstance*>& nodeLocations) {
	llvm::IRBuilder<> builder(block);

	// get inputs and outputs
	std::vector<llvm::Value*> io;

	// add inputs
	{
		size_t inputID = 0;
		for (auto& param : node->inputDataConnections) {
			// make sure everything is A-OK
			if (param.first == nullptr) {
				res.addEntry("EUKN", "No data input to node",
							 {{"nodeid", node->id()},
							  {"input ID", inputID},
							  {"nodetype", node->type().qualifiedName()}});

				return;
			}

			auto cacheiter = nodeCache.find(param.first);

			if (cacheiter == nodeCache.end()) {
				res.addEntry("EUKN", "Failed to find in cache", {{"nodeid", param.first->id()}});

				return;
			}

			auto& cacheObject = cacheiter->second;
			if (param.second >= cacheObject.outputs.size()) {
				res.addEntry("EUKN", "No data input to node",
							 {{"nodeid", node->id()},
							  {"input ID", inputID},
							  {"nodetype", node->type().qualifiedName()}});

				return;
			}

			// get pointers to the objects
			auto value = cacheObject.outputs[param.second];
			// dereference
			io.push_back(builder.CreateLoad(value));  // TODO: pass ptr to value

			// make sure it's the right type
			if (io[io.size() - 1]->getType() !=
				node->type().dataInputs()[inputID].first.llvmType()) {
				res.addEntry(
					"EINT", "Internal codegen error: unexpected type in cache.",
					{{"Expected LLVM type",
					  stringifyLLVMType(node->type().dataInputs()[inputID].first.llvmType())},
					 {"Found type", stringifyLLVMType(io[io.size() - 1]->getType())},
					 {"Node ID", node->id()},
					 {"Input ID", inputID}});
			}

			++inputID;
		}
	}

	// get outputs
	{
		llvm::IRBuilder<> allocBuilder(allocblock);

		// create outputs
		auto& outputCache = nodeCache[node].outputs;

		for (auto& output : node->type().dataOutputs()) {
			// TODO: research address spaces
			llvm::AllocaInst* alloc = allocBuilder.CreateAlloca(output.first.llvmType(), nullptr,
																node->id() + "__" + output.second);
			outputCache.push_back(alloc);
			io.push_back(alloc);

			// create debug info
			{
				// get type
				llvm::DIType* dType =
					output.first.module().debugTypeFromName(output.first.unqualifiedName());

				// TODO: better names
                auto debugVar = dbuilder->
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 7
                    createLocalVariable(llvm::dwarf::DW_TAG_auto_variable,
#else
                    createAutoVariable(
#endif
                      diFunc, node->id() + "__" + output.second, diFunc->getFile(), 1, dType);

				dbuilder->insertDeclare(alloc, debugVar, dbuilder->createExpression(),
										llvm::DebugLoc::get(1, 1, diFunc), allocblock);
			}

			// make sure the type is right
			if (llvm::PointerType::get(output.first.llvmType(), 0) != alloc->getType()) {
				res.addEntry(
					"EINT", "Internal codegen error: unexpected type returned from alloca.",
					{{"Expected LLVM type",
					  stringifyLLVMType(llvm::PointerType::get(output.first.llvmType(), 0))},
					 {"Yielded type", stringifyLLVMType(alloc->getType())},
					 {"Node ID", node->id()}});
			}
		}
	}

	// add this block to the cache
	if (node->type().qualifiedName() != "lang:entry") {
		nodeCache[node].inputBlock.resize(node->inputExecConnections.size(), nullptr);
		nodeCache[node].inputBlock[execInputID] = block;
	}

	// create output blocks
	std::vector<llvm::BasicBlock*> outputBlocks;
	std::vector<llvm::BasicBlock*> unusedBlocks;
	boost::dynamic_bitset<>
		needsCodegen;  // this holds if each block needs to recurse of it it's already in the cache
	for (auto idx = 0ull; idx < node->outputExecConnections.size(); ++idx) {
		// see if we can find it in the cache
		NodeInstance* remoteNode;
		size_t		  remotePortIdx;
		std::tie(remoteNode, remotePortIdx) = node->outputExecConnections[idx];

		auto remoteCache = nodeCache[remoteNode];  // this will construct it if it doesn't exist
		if (remoteCache.inputBlock.size() > remotePortIdx &&
			remoteCache.inputBlock[remotePortIdx] != nullptr) {
			// this means it's already in the cache
			outputBlocks.push_back(remoteCache.inputBlock[remotePortIdx]);
			needsCodegen.push_back(false);
		} else {
			auto outBlock =
				llvm::BasicBlock::Create(f->getContext(), node->type().execOutputs()[idx], f);
			outputBlocks.push_back(outBlock);
			needsCodegen.push_back(true);  // these need codegen
			if (node->outputExecConnections[idx].first != nullptr) {
				outBlock->setName("node_" + node->outputExecConnections[idx].first->id());
			} else {
				unusedBlocks.push_back(outBlock);
			}
		}
	}

	// codegen
	res += node->type().codegen(execInputID, mod,
								llvm::DebugLoc::get(nodeLocations.right.at(node), 1, diFunc), f, io,
								block, outputBlocks);
	if (!res) { return; }

	// TODO: sequence nodes
	for (auto& bb : unusedBlocks) {
		llvm::IRBuilder<> builder(bb);

		builder.CreateRet(
			llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(mod->getContext()), 0, true));
	}

	// recurse!
	for (auto idx = 0ull; idx < node->outputExecConnections.size(); ++idx) {
		auto& output = node->outputExecConnections[idx];
		if (output.first != nullptr && needsCodegen[idx]) {
			codegenHelper(output.first, output.second, outputBlocks[idx], allocblock, mod, dbuilder,
						  f, diFunc, nodeCache, res, nodeLocations);
		}
	}
}

Result GraphFunction::compile(llvm::Module* mod, llvm::DICompileUnit* debugCU,
							  llvm::DIBuilder& debugBuilder) const {
	Expects(mod != nullptr);

	Result res;

	auto entry = entryNode();
	if (entry == nullptr) {
		res.addEntry("EUKN", "No entry node", {});
		return res;
	}

	// make sure that the entry node has the functiontype
	if (!std::equal(dataInputs().begin(), dataInputs().end(),
					entry->type().dataOutputs().begin())) {
		nlohmann::json inFunc = nlohmann::json::array();
		for (auto& in : dataInputs()) { inFunc.push_back({{in.second, in.first.qualifiedName()}}); }

		nlohmann::json inEntry = nlohmann::json::array();
		for (auto& in :
			 entry->type().dataOutputs()) {  // outputs to entry are inputs to the function
			inEntry.push_back({{in.second, in.first.qualifiedName()}});
		}

		res.addEntry("EUKN", "Inputs to function doesn't match function inputs",
					 {{"Function Inputs", inFunc}, {"Entry Inputs", inEntry}});
		return res;
	}

	// make sure that the entry node has the functiontype
	if (!std::equal(dataOutputs().begin(), dataOutputs().end(), dataOutputs().begin())) {
		nlohmann::json outFunc = nlohmann::json::array();
		for (auto& out : dataOutputs()) {
			outFunc.push_back({{out.second, out.first.qualifiedName()}});
		}

		nlohmann::json outEntry = nlohmann::json::array();
		for (auto& out : dataOutputs()) {
			// inputs to the exit are outputs to the function
			outEntry.push_back({{out.second, out.first.qualifiedName()}});
		}

		res.addEntry("EUKN", "Outputs to function doesn't match function exit",
					 {{"Function Outputs", outFunc}, {"Entry Outputs", outEntry}});
		return res;
	}

	auto debugFile   = debugBuilder.createFile(debugCU->getFilename(), debugCU->getDirectory());
    
	// create function type
	llvm::DISubroutineType* subroutineType;
	{
		// create param list
		std::vector<llvm::Metadata*> params;
		{
			// ret first
			llvm::DIType* intType;
			res += context().debugTypeFromModule("lang", "i32", &intType);
			params.push_back(intType);

			// then first in inputexec id
			params.push_back(intType);

			// add paramters
			for (const auto& dType : boost::range::join(dataInputs(), dataOutputs())) {
				llvm::DIType* debugTy;
				res += context().debugTypeFromModule(dType.first.module().name(),
													 dType.first.unqualifiedName(), &debugTy);
				params.push_back(debugTy);
			}
		}

		// create type
		subroutineType =
			debugBuilder.createSubroutineType(
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 7
              debugFile,
#endif
              debugBuilder.getOrCreateTypeArray(params));
	}

	auto mangledName = mangleFunctionName(module().fullName(), name());
	llvm::Function* f =
		llvm::cast<llvm::Function>(mod->getOrInsertFunction(mangledName, functionType()));
    
	// TODO: line numbers?
	auto debugFunc =
		debugBuilder.createFunction(debugFile, module().fullName() + ":" + name(), mangledName,
									debugFile, 0, subroutineType, false, true, 0, 0, false
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 7
        , f);
#else
        );

	f->setSubprogram(debugFunc);
#endif
	llvm::BasicBlock* allocblock = llvm::BasicBlock::Create(mod->getContext(), "alloc", f);
	llvm::BasicBlock* block	= llvm::BasicBlock::Create(mod->getContext(), name() + "_entry", f);
	auto			  blockcpy = block;

	// follow "linked list"
	unsigned execInputID =
		entry->outputExecConnections[0].second;  // the exec connection to codegen from

	std::unordered_map<NodeInstance*, cache> nodeCache;

	// set argument names
	auto idx = 0ull;
	for (auto& arg : f->getArgumentList()) {
		// the first one is the input exec ID
		if (idx == 0) {
			arg.setName("inputexec_id");

			// create debug info
			llvm::DIType* intDebugType;
			res += context().debugTypeFromModule("lang", "i32", &intDebugType);
			auto debugParam = debugBuilder.
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 7
              createLocalVariable(llvm::dwarf::DW_TAG_arg_variable, debugFunc, "inputexec_id", debugFile, 0, intDebugType);
#else
              createParameterVariable(debugFunc, "inputexec_id", 0, debugFile, 0, intDebugType);
#endif         
			debugBuilder.insertDeclare(&arg, debugParam, debugBuilder.createExpression(),
									   llvm::DebugLoc::get(1, 1, debugFunc),
									   allocblock);  // TODO: "line" numbers

			++idx;
			continue;
		}

		std::pair<DataType, std::string> tyAndName;
		// all the - 1's is becaues the first is the inputexec_id
		if (idx - 1 < dataInputs().size()) {
			tyAndName = dataInputs()[idx - 1];
		} else {
			tyAndName = dataOutputs()[idx - 1 - entry->type().dataOutputs().size()];
		}
		arg.setName(tyAndName.second);

		// create debug info

		// create DIType*
		llvm::DIType* dType =
			tyAndName.first.module().debugTypeFromName(tyAndName.first.unqualifiedName());
		auto debugParam = debugBuilder.
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 7
              createLocalVariable(llvm::dwarf::DW_TAG_arg_variable, debugFunc, tyAndName.second, debugFile, 0, dType);
#else
		createParameterVariable(debugFunc, tyAndName.second, idx,
															   debugFile, 0, dType);
#endif     
		debugBuilder.insertDeclare(&arg, debugParam, debugBuilder.createExpression(),
								   llvm::DebugLoc::get(1, 1, debugFunc),
								   allocblock);  // TODO: line numbers

		++idx;
	}

	auto nodeLocations = module().createLineNumberAssoc();
	codegenHelper(entry, execInputID, block, allocblock, mod, &debugBuilder, f, debugFunc,
				  nodeCache, res, nodeLocations);

	llvm::IRBuilder<> allocbuilder(allocblock);
	allocbuilder.CreateBr(blockcpy);

	return res;
}

NodeInstance* GraphFunction::entryNode() const noexcept {
	auto matching = graph().nodesWithType("lang", "entry");

	if (matching.size() == 1) {
		auto span = matching[0]->type().dataOutputs();
		// make sure it has the same signature as the method
		if (!std::equal(dataInputs().begin(), dataInputs().end(), span.begin(), span.end())) {
			return nullptr;
		}
		// make sure it has the same exec names and size
		if (!std::equal(execInputs().begin(), execInputs().end(),
						matching[0]->type().execOutputs().begin())) {
			return nullptr;
		}
		return matching[0];
	}
	return nullptr;
}

Result GraphFunction::insertNode(gsl::cstring_span<> moduleName, gsl::cstring_span<> typeName,
								 const nlohmann::json& typeJSON, float x, float y,
								 gsl::cstring_span<> id, NodeInstance** toFill) {
	std::unique_ptr<NodeType> nodeType;
	Result res = context().nodeTypeFromModule(moduleName, typeName, typeJSON, &nodeType);

	if (!res) { return res; }
	res += insertNode(std::move(nodeType), x, y, id, toFill);
	return res;
}

Result GraphFunction::removeNode(NodeInstance* nodeToRemove) {
	Result res;

	// disconnect it's connections

	// disconnect input exec
	for (const auto& execSlot : nodeToRemove->inputExecConnections) {
		for (const auto& pair : execSlot) {
			if (pair.first != nullptr) { res += disconnectExec(*pair.first, pair.second); }
		}
	}
	// disconnect output exec
	auto ID = 0ull;
	for (const auto& pair : nodeToRemove->outputExecConnections) {
		if (pair.first != nullptr) { res += disconnectExec(*nodeToRemove, ID); }
		++ID;
	}

	// disconnect input data
	for (const auto& pair : nodeToRemove->inputDataConnections) {
		if (pair.first != nullptr) {
			res += disconnectData(*pair.first, pair.second, *nodeToRemove);
		}
	}

	// disconnect output data
	ID = 0ull;
	for (const auto& dataSlot : nodeToRemove->outputDataConnections) {
		for (const auto& pair : dataSlot) {
			if (pair.first != nullptr) { disconnectData(*nodeToRemove, ID, *pair.first); }
		}
		++ID;
	}
	// then delete the node
	graph().nodes().erase(nodeToRemove->id());

	return res;
}

Result GraphFunction::createEntryNodeType(std::unique_ptr<NodeType>* toFill) const {
	Result res;

	nlohmann::json entry = nlohmann::json::object();

	auto& data = entry["data"];
	data	   = nlohmann::json::array();
	for (auto in : dataInputs()) { data.push_back({{in.second, in.first.qualifiedName()}}); }

	auto& exec = entry["exec"];
	exec	   = nlohmann::json::array();
	for (const auto& in : execInputs()) { exec.push_back(in); }

	res += context().nodeTypeFromModule("lang", "entry", entry, toFill);

	return res;
}

Result GraphFunction::createExitNodeType(std::unique_ptr<NodeType>* toFill) const {
	Result res;

	nlohmann::json exit = nlohmann::json::object();

	auto& data = exit["data"];
	data	   = nlohmann::json::array();
	for (auto out : dataOutputs()) { data.push_back({{out.second, out.first.qualifiedName()}}); }

	auto& exec = exit["exec"];
	exec	   = nlohmann::json::array();
	for (const auto& out : execOutputs()) { exec.push_back(out); }

	res += context().nodeTypeFromModule("lang", "exit", exit, toFill);

	return res;
}

Result GraphFunction::getOrInsertEntryNode(float x, float y, gsl::cstring_span<> id,
										   NodeInstance** toFill) {
	Result res;

	if (auto ent = entryNode()) {
		if (toFill != nullptr) { *toFill = ent; }
		return {};
	}

	std::unique_ptr<NodeType> entryNodeType;
	res += createEntryNodeType(&entryNodeType);

	res += insertNode(std::move(entryNodeType), x, y, id, toFill);

	return res;
}

Result GraphFunction::loadGraph() {
	Result res;
	mGraph = Graph(*this, mSource, res);

	return res;
}

Result GraphFunction::validateGraph() const {
	Result res;
	// make sure all connections connect back

	for (const auto& node : graph().nodes()) {
		// go through input data
		auto id = 0ull;
		for (const auto& conn : node.second->inputDataConnections) {
			// make sure it connects  back
			bool connectsBack = false;
			for (const auto& remoteConn : conn.first->outputDataConnections[conn.second]) {
				if (remoteConn.first == node.second.get() && remoteConn.second == id) {
					connectsBack = true;
					break;
				}
			}
			if (!connectsBack) {
				res.addEntry("EUKN", "Data connection doesn't connect back",
							 {{"Left Node", conn.first->id()},
							  {"Right Node", node.second->id()},
							  {"Right input ID", id}});
			}
			++id;
		}

		// output data
		id = 0ull;
		for (const auto& outputDataSlot : node.second->outputDataConnections) {
			// this connection type can make multiple connections, so two for loops are needed
			for (const auto& connection : outputDataSlot) {
				auto& remoteConn = connection.first->inputDataConnections[connection.second];
				if (remoteConn.first != node.second.get() || remoteConn.second != id) {
					res.addEntry("EUKN", "Data connection doesn't connect back",
								 {{"Left Node", node.second->id()},
								  {"Right Node", connection.first->id()},
								  {"Right input ID", connection.second}});
				}
			}
			++id;
		}

		// input exec
		id = 0ull;
		for (const auto& inputExecSlot : node.second->inputExecConnections) {
			// this connection type can make multiple connections, so two for loops are needed
			for (const auto& connection : inputExecSlot) {
				auto& remoteConn = connection.first->outputExecConnections[connection.second];
				if (remoteConn.first != node.second.get() || remoteConn.second != id) {
					res.addEntry(
						"EUKN", "Exec connection doesn't connect back",
						{{"Left Node", connection.first->id()},
						 {"Right Node", node.second->id()},
						 {"Left output ID", connection.second}});  // TODO: better diagnostics
				}
			}
			++id;
		}

		// output exec
		id = 0ull;
		for (const auto& connection : node.second->outputExecConnections) {
			bool connectsBack = false;
			for (const auto& remoteConnection :
				 connection.first->inputExecConnections[connection.second]) {
				if (remoteConnection.second == id && remoteConnection.first == node.second.get()) {
					connectsBack = true;
					break;
				}
			}

			if (!connectsBack) {
				res.addEntry("EUKN", "Exec connection doesn't connect back",
							 {{"Left Node", node.second->id()},
							  {"Right Node", connection.first->id()},
							  {"Left output ID", id}});
			}

			++id;
		}
	}

	// make sure all entires have the right type

	return res;
}

void GraphFunction::addDataInput(const DataType& type, gsl::cstring_span<> name, int addAfter) {
	if (addAfter < mDataInputs.size()) {
		// +1 because emplace adds before
		mDataInputs.emplace(mDataInputs.cbegin() + addAfter + 1, type, gsl::to_string(name));
	} else {
		mDataInputs.emplace_back(type, gsl::to_string(name));
	}
	updateEntries();
}

void GraphFunction::removeDataInput(int idx) {
	if (idx < mDataInputs.size()) { mDataInputs.erase(mDataInputs.begin() + idx); }
	updateEntries();
}

void GraphFunction::modifyDataInput(int idx, const DataType& type,
									boost::optional<gsl::cstring_span<>> name) {
	if (idx < mDataInputs.size()) {
		if (type.valid()) { mDataInputs[idx].first = type; }
		if (name) { mDataInputs[idx].second = gsl::to_string(*name); }
	}
	updateEntries();
}

void GraphFunction::addDataOutput(const DataType& type, gsl::cstring_span<> name, int addAfter) {
	if (addAfter < mDataOutputs.size()) {
		// +1 because emplace adds before
		mDataOutputs.emplace(mDataOutputs.cbegin() + addAfter + 1, type, gsl::to_string(name));
	} else {
		mDataOutputs.emplace_back(type, gsl::to_string(name));
	}
	updateExits();
}

void GraphFunction::removeDataOutput(int idx) {
	if (idx < mDataOutputs.size()) { mDataOutputs.erase(mDataOutputs.begin() + idx); }
	updateExits();
}

void GraphFunction::modifyDataOutput(int idx, const DataType& type,
									 boost::optional<gsl::cstring_span<>> name) {
	if (idx < mDataOutputs.size()) {
		if (type.valid()) { mDataOutputs[idx].first = type; }
		if (name) { mDataOutputs[idx].second = gsl::to_string(*name); }
	}
	updateExits();
}

llvm::FunctionType* GraphFunction::functionType() const {
	std::vector<llvm::Type*> arguments;
	arguments.reserve(1 + dataInputs().size() + dataOutputs().size());

	// this is for which exec input
	arguments.push_back(llvm::IntegerType::getInt32Ty(context().llvmContext()));

	for (const auto& p : dataInputs()) { arguments.push_back(p.first.llvmType()); }

	// make these pointers
	for (const auto& p : dataOutputs()) {
		arguments.push_back(llvm::PointerType::get(p.first.llvmType(), 0));
	}

	return llvm::FunctionType::get(llvm::IntegerType::getInt32Ty(context().llvmContext()),
								   arguments, false);
}

void GraphFunction::addExecInput(gsl::cstring_span<> name, int addAfter) {
	if (addAfter < mExecInputs.size()) {
		// +1 because emplace adds before
		mExecInputs.emplace(mExecInputs.cbegin() + addAfter + 1, gsl::to_string(name));
	} else {
		mExecInputs.emplace_back(gsl::to_string(name));
	}
	updateEntries();
}

void GraphFunction::removeExecInput(int idx) {
	if (idx < mExecOutputs.size()) { mExecOutputs.erase(mExecOutputs.begin() + idx); }
	updateEntries();
}

void GraphFunction::modifyExecInput(int idx, gsl::cstring_span<> name) {
	if (idx < mExecInputs.size()) { mExecInputs[idx] = gsl::to_string(name); }
	updateEntries();
}

void GraphFunction::addExecOutput(gsl::cstring_span<> name, int addAfter) {
	if (addAfter < mExecOutputs.size()) {
		// +1 because emplace adds before
		mExecOutputs.emplace(mExecOutputs.cbegin() + addAfter + 1, gsl::to_string(name));
	} else {
		mExecOutputs.emplace_back(gsl::to_string(name));
	}
	updateExits();
}

void GraphFunction::removeExecOutput(int idx) {
	if (idx < mExecOutputs.size()) { mExecOutputs.erase(mExecOutputs.begin() + idx); }
	updateExits();
}

void GraphFunction::modifyExecOutput(int idx, gsl::cstring_span<> name) {
	if (idx < mExecOutputs.size()) { mExecOutputs[idx] = gsl::to_string(name); }
	updateExits();
}

void GraphFunction::updateEntries() {
	auto matching = graph().nodesWithType("lang", "entry");

	for (auto entry : matching) {
		if (entry == nullptr) { return; }

		std::unique_ptr<NodeType> entryNodeType;
		createEntryNodeType(&entryNodeType);

		entry->setType(std::move(entryNodeType));
	}
}

void GraphFunction::updateExits() {
	for (const auto& exitNode : graph().nodesWithType("lang", "exit")) {
		std::unique_ptr<NodeType> exitNodeType;
		createExitNodeType(&exitNodeType);

		exitNode->setType(std::move(exitNodeType));
	}
}

}  // namespace chig
