#include "chig/LangModule.hpp"
#include "chig/Context.hpp"

#include <llvm/AsmParser/Parser.h>
#include <llvm/AsmParser/Parser.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/Support/SourceMgr.h>
#include <gsl/gsl_assert>

namespace chig {
/// NodeType for conditionals
struct IfNodeType : NodeType {
	IfNodeType(LangModule& mod) : NodeType(mod, "if", "branch on a bools") {
		setExecInputs({""});
		setExecOutputs({"True", "False"});

		setDataInputs({{mod.typeFromName("i1"), "condition"}});
	}

	Result codegen(size_t /*execInputID*/, llvm::Module* /*mod*/,
				   const llvm::DebugLoc& nodeLocation, llvm::Function* /*f*/,
				   const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
				   const gsl::span<llvm::BasicBlock*> outputBlocks) const override {
		Expects(io.size() == 1 && codegenInto != nullptr && outputBlocks.size() == 2);

		llvm::IRBuilder<> builder(codegenInto);
		builder.SetCurrentDebugLocation(nodeLocation);

		builder.CreateCondBr(io[0], outputBlocks[0], outputBlocks[1]);

		return {};
	}

	std::unique_ptr<NodeType> clone() const override { return std::make_unique<IfNodeType>(*this); }
};

struct EntryNodeType : NodeType {
	EntryNodeType(LangModule& mod, std::vector<std::pair<DataType, std::string>> dataInputs,
				  std::vector<std::string> execInputs)
		: NodeType(mod, "entry", "entry to a function") {
		setExecOutputs(std::move(execInputs));

		setDataOutputs(std::move(dataInputs));
	}

	Result codegen(size_t /*inputExecID*/, llvm::Module* /*mod*/,
				   const llvm::DebugLoc& nodeLocation, llvm::Function* f,
				   const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
				   const gsl::span<llvm::BasicBlock*> outputBlocks) const override {
		Expects(f != nullptr && io.size() == dataOutputs().size() && codegenInto != nullptr &&
				outputBlocks.size() == 1);

		llvm::IRBuilder<> builder(codegenInto);
		builder.SetCurrentDebugLocation(nodeLocation);

		// store the arguments
		auto arg_iter = f->arg_begin();
		++arg_iter;  // skip the first argument, which is the input exec ID
		for (const auto& iovalue : io) {
			builder.CreateStore(&*arg_iter, iovalue);

			++arg_iter;
		}

		builder.CreateBr(outputBlocks[0]);

		return {};
	}

	std::unique_ptr<NodeType> clone() const override {
		return std::make_unique<EntryNodeType>(*this);
	}

	nlohmann::json toJSON() const override {
		nlohmann::json ret = nlohmann::json::object();

		auto& data = ret["data"];
		data	   = nlohmann::json::array();
		for (auto& pair : dataOutputs()) {
			data.push_back({{pair.second, pair.first.qualifiedName()}});
		}

		auto& exec = ret["exec"];
		exec	   = nlohmann::json::array();
		for (auto& name : execOutputs()) { exec.push_back(name); }

		return ret;
	}
};

struct ConstIntNodeType : NodeType {
	ConstIntNodeType(LangModule& mod, int num)
		: NodeType(mod, "const-int", "Int literal"), number(num) {
		setExecInputs({""});
		setExecOutputs({""});

		setDataOutputs({{mod.typeFromName("i32"), "out"}});
	}

	Result codegen(size_t /*inputExecID*/, llvm::Module* /*mod*/,
				   const llvm::DebugLoc& nodeLocation, llvm::Function* /*f*/,
				   const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
				   const gsl::span<llvm::BasicBlock*> outputBlocks) const override {
		Expects(io.size() == 1 && codegenInto != nullptr && outputBlocks.size() == 1);

		llvm::IRBuilder<> builder(codegenInto);
		builder.SetCurrentDebugLocation(nodeLocation);

		builder.CreateStore(
			llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(context().llvmContext()), number),
			io[0], false);
		builder.CreateBr(outputBlocks[0]);

		return {};
	}

	std::unique_ptr<NodeType> clone() const override {
		return std::make_unique<ConstIntNodeType>(*this);
	}

	nlohmann::json toJSON() const override { return number; }
	int			   number;
};

struct ConstFloatNodeType : NodeType {
	ConstFloatNodeType(LangModule& mod, double num)
		: NodeType(mod, "const-float", "Float Literal"), number(num) {
		setExecInputs({""});
		setExecOutputs({""});

		setDataOutputs({{mod.typeFromName("i32"), "out"}});
	}

	Result codegen(size_t /*inputExecID*/, llvm::Module* /*mod*/,
				   const llvm::DebugLoc& nodeLocation, llvm::Function* /*f*/,
				   const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
				   const gsl::span<llvm::BasicBlock*> outputBlocks) const override {
		Expects(io.size() == 1 && codegenInto != nullptr && outputBlocks.size() == 1);

		llvm::IRBuilder<> builder(codegenInto);
		builder.SetCurrentDebugLocation(nodeLocation);

		builder.CreateStore(
			llvm::ConstantFP::get(llvm::Type::getFloatTy(context().llvmContext()), number), io[0]);
		builder.CreateBr(outputBlocks[0]);

		return {};
	}

	std::unique_ptr<NodeType> clone() const override {
		return std::make_unique<ConstFloatNodeType>(*this);
	}

	nlohmann::json toJSON() const override { return number; }
	double		   number;
};

struct ConstBoolNodeType : NodeType {
	ConstBoolNodeType(LangModule& mod, bool num)
		: NodeType{mod, "const-bool", "Boolean literal"}, value{num} {
		setExecInputs({""});
		setExecOutputs({""});

		setDataOutputs({{mod.typeFromName("i1"), "out"}});
	}

	Result codegen(size_t /*inputExecID*/, llvm::Module* /*mod*/,
				   const llvm::DebugLoc& nodeLocation, llvm::Function* /*f*/,
				   const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
				   const gsl::span<llvm::BasicBlock*> outputBlocks) const override {
		Expects(io.size() == 1 && codegenInto != nullptr && outputBlocks.size() == 1);

		llvm::IRBuilder<> builder(codegenInto);
		builder.SetCurrentDebugLocation(nodeLocation);

		builder.CreateStore(
			llvm::ConstantInt::get(llvm::IntegerType::getInt1Ty(context().llvmContext()),
								   static_cast<uint64_t>(value)),
			io[0], false);
		builder.CreateBr(outputBlocks[0]);

		return {};
	}

	std::unique_ptr<NodeType> clone() const override {
		return std::make_unique<ConstBoolNodeType>(*this);
	}

	nlohmann::json toJSON() const override { return value; }
	bool		   value;
};

struct ExitNodeType : NodeType {
	ExitNodeType(LangModule& mod, std::vector<std::pair<DataType, std::string>> dataOutputs,
				 std::vector<std::string> execOutputs)
		: NodeType{mod, "exit", "Return from a function"} {
		// outputs to the function are inputs to the node
		setExecInputs(std::move(execOutputs));

		setDataInputs(std::move(dataOutputs));
	}

	Result codegen(size_t execInputID, llvm::Module* /*mod*/, const llvm::DebugLoc& nodeLocation,
				   llvm::Function* f, const gsl::span<llvm::Value*> io,
				   llvm::BasicBlock* codegenInto,
				   const gsl::span<llvm::BasicBlock*> /*outputBlocks*/) const override {
		Expects(execInputID < execInputs().size() && f != nullptr &&
				io.size() == dataInputs().size() && codegenInto != nullptr);

		// assign the return types
		llvm::IRBuilder<> builder(codegenInto);
		builder.SetCurrentDebugLocation(nodeLocation);

		size_t ret_start =
			f->arg_size() - io.size();  // returns are after args, find where returns start
		auto arg_iter = f->arg_begin();
		std::advance(arg_iter, ret_start);
		for (auto& value : io) {
			builder.CreateStore(value, &*arg_iter, false);  // TODO: volitility?
			++arg_iter;
		}

		builder.CreateRet(
			llvm::ConstantInt::get(llvm::Type::getInt32Ty(context().llvmContext()), execInputID));

		return {};
	}

	std::unique_ptr<NodeType> clone() const override {
		return std::make_unique<ExitNodeType>(*this);
	}

	nlohmann::json toJSON() const override {
		nlohmann::json ret = nlohmann::json::object();

		auto& data = ret["data"];
		data	   = nlohmann::json::array();
		for (auto& pair : dataInputs()) {
			data.push_back({{pair.second, pair.first.qualifiedName()}});
		}

		auto& exec = ret["exec"];
		exec	   = nlohmann::json::array();
		for (auto& name : execInputs()) { exec.push_back(name); }

		return ret;
	}
};

struct StringLiteralNodeType : NodeType {
	StringLiteralNodeType(LangModule& mod, std::string str)
		: NodeType(mod, "strliteral", "exit from a function; think return"),
		  literalString(std::move(str)) {
		setExecInputs({""});
		setExecOutputs({""});

		setDataOutputs({{mod.typeFromName("i8*"), "string"}});
	}

	Result codegen(size_t /*execInputID*/, llvm::Module* /*mod*/,
				   const llvm::DebugLoc& nodeLocation, llvm::Function* /*f*/,
				   const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
				   const gsl::span<llvm::BasicBlock*> outputBlocks) const override {
		Expects(io.size() == 1 && codegenInto != nullptr && outputBlocks.size() == 1);

		llvm::IRBuilder<> builder(codegenInto);
		builder.SetCurrentDebugLocation(nodeLocation);

		auto global = builder.CreateGlobalString(literalString);

		auto const0ID = llvm::ConstantInt::get(context().llvmContext(), llvm::APInt(32, 0, false));
		auto gep	  = builder.CreateGEP(global, {const0ID, const0ID});
		builder.CreateStore(gep, io[0], false);

		builder.CreateBr(outputBlocks[0]);

		return {};
	}

	std::unique_ptr<NodeType> clone() const override {
		return std::make_unique<StringLiteralNodeType>(*this);
	}

	nlohmann::json toJSON() const override { return literalString; }
	std::string	literalString;
};

struct IntToFloatNodeType : NodeType {
	IntToFloatNodeType(LangModule& mod) : NodeType(mod, "inttofloat", "convert integer to float") {
		setExecInputs({""});
		setExecOutputs({""});

		setDataInputs({{mod.typeFromName("i32"), ""}});
		setDataOutputs({{mod.typeFromName("float"), ""}});
	}

	Result codegen(size_t /*execInputID*/, llvm::Module* /*mod*/,
				   const llvm::DebugLoc&		 nodeLocation, llvm::Function* /*f*/,
				   const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
				   const gsl::span<llvm::BasicBlock*> outputBlocks) const override {
		Expects(io.size() == 2 && codegenInto != nullptr && outputBlocks.size() == 1);

		llvm::IRBuilder<> builder(codegenInto);
		builder.SetCurrentDebugLocation(nodeLocation);

		auto casted = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, io[0],
										 llvm::Type::getFloatTy(context().llvmContext()));
		builder.CreateStore(casted, io[1]);

		builder.CreateBr(outputBlocks[0]);

		return {};
	}

	std::unique_ptr<NodeType> clone() const override {
		return std::make_unique<IntToFloatNodeType>(*this);
	}
};


struct FloatToIntNodeType : NodeType {
	FloatToIntNodeType(LangModule& mod) : NodeType(mod, "floattoint", "convert float to integer") {
		setExecInputs({""});
		setExecOutputs({""});

		setDataInputs({{mod.typeFromName("float"), ""}});
		setDataOutputs({{mod.typeFromName("i32"), ""}});
	}

	Result codegen(size_t /*execInputID*/, llvm::Module* /*mod*/,
				   const llvm::DebugLoc&		 nodeLocation, llvm::Function* /*f*/,
				   const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
				   const gsl::span<llvm::BasicBlock*> outputBlocks) const override {
		Expects(io.size() == 2 && codegenInto != nullptr && outputBlocks.size() == 1);

		llvm::IRBuilder<> builder(codegenInto);
		builder.SetCurrentDebugLocation(nodeLocation);

		auto casted = builder.CreateCast(llvm::Instruction::CastOps::FPToSI, io[0],
										 llvm::Type::getFloatTy(context().llvmContext()));
		builder.CreateStore(casted, io[1]);

		builder.CreateBr(outputBlocks[0]);

		return {};
	}

	std::unique_ptr<NodeType> clone() const override {
		return std::make_unique<FloatToIntNodeType>(*this);
	}
};


enum class BinOp {
    Add,
    Subtract,
    Multiply,
    Divide
};


struct BinaryOperationNodeType : NodeType {
    BinaryOperationNodeType(LangModule& mod, DataType ty, BinOp binaryOperation) : NodeType(mod), mBinOp(binaryOperation) {
		setExecInputs({""});
		setExecOutputs({""});
        
        std::string opStr = [](BinOp b) {
            switch (b) {
                case BinOp::Add: return "+";
                case BinOp::Subtract: return "-";
                case BinOp::Multiply: return "*";
                case BinOp::Divide: return "/";
                default: return "";
            }
            return "";
        }(binaryOperation);
        
        setName(ty.unqualifiedName() + opStr + ty.unqualifiedName());
        
        std::string opVerb = [](BinOp b) {
            switch (b) {
                case BinOp::Add: return "Add";
                case BinOp::Subtract: return "Subtract";
                case BinOp::Multiply: return "Multiply";
                case BinOp::Divide: return "Divide";
                default: return "";
            }
            return "";
        }(binaryOperation);
        
        setDescription(opVerb + " two " + ty.unqualifiedName() + "s");

		setDataInputs({{ty, "a"}, {ty, "b"}});
		setDataOutputs({{ty, ""}});
	}

	Result codegen(size_t /*execInputID*/, llvm::Module* /*mod*/,
				   const llvm::DebugLoc&		 nodeLocation, llvm::Function* /*f*/,
				   const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
				   const gsl::span<llvm::BasicBlock*> outputBlocks) const override {
		Expects(io.size() == 3 && codegenInto != nullptr && outputBlocks.size() == 1);

		llvm::IRBuilder<> builder(codegenInto);
		builder.SetCurrentDebugLocation(nodeLocation);

		llvm::Value* result = [&](BinOp b){
            switch(b) {
                
                case BinOp::Add: return builder.CreateAdd(io[0], io[1]);
                case BinOp::Subtract: return builder.CreateSub(io[0], io[1]);
                case BinOp::Multiply: return builder.CreateMul(io[0], io[1]);
                case BinOp::Divide: return builder.CreateSDiv(io[0], io[1]);
                default: return (llvm::Value*)nullptr;
            }
            return (llvm::Value*)nullptr;
        }(mBinOp);
        
        builder.CreateStore(result, io[2]);

		builder.CreateBr(outputBlocks[0]);

		return {};
	}

	std::unique_ptr<NodeType> clone() const override {
		return std::make_unique<BinaryOperationNodeType>(*this);
	}
	
	BinOp mBinOp;
};

LangModule::LangModule(Context& ctx) : ChigModule(ctx, "lang") {
	using namespace std::string_literals;

	// populate them
	nodes = {
		{"if"s, [this](const nlohmann::json&,
					   Result&) { return std::make_unique<IfNodeType>(*this); }},
		{"i32+i32"s, [this](const nlohmann::json&,
							Result&) { return std::make_unique<BinaryOperationNodeType>(*this, LangModule::typeFromName("i32"), BinOp::Add); }},
		{"i32-i32"s, [this](const nlohmann::json&,
							Result&) { return std::make_unique<BinaryOperationNodeType>(*this, LangModule::typeFromName("i32"), BinOp::Subtract); }},
		{"i32*i32"s, [this](const nlohmann::json&,
							Result&) { return std::make_unique<BinaryOperationNodeType>(*this, LangModule::typeFromName("i32"), BinOp::Multiply); }},
		{"i32/i32"s, [this](const nlohmann::json&,
							Result&) { return std::make_unique<BinaryOperationNodeType>(*this, LangModule::typeFromName("i32"), BinOp::Divide); }},
		{"float+float"s, [this](const nlohmann::json&,
							Result&) { return std::make_unique<BinaryOperationNodeType>(*this, LangModule::typeFromName("float"), BinOp::Add); }},
		{"float-float"s, [this](const nlohmann::json&,
							Result&) { return std::make_unique<BinaryOperationNodeType>(*this, LangModule::typeFromName("float"), BinOp::Subtract); }},
		{"float*float"s, [this](const nlohmann::json&,
							Result&) { return std::make_unique<BinaryOperationNodeType>(*this, LangModule::typeFromName("float"), BinOp::Multiply); }},
		{"float/float"s, [this](const nlohmann::json&,
							Result&) { return std::make_unique<BinaryOperationNodeType>(*this, LangModule::typeFromName("float"), BinOp::Divide); }},
		{"inttofloat"s,
		 [this](const nlohmann::json&, Result&) {
			 return std::make_unique<IntToFloatNodeType>(*this);
		 }},
         {"floattoint"s,
		 [this](const nlohmann::json&, Result&) {
			 return std::make_unique<FloatToIntNodeType>(*this);
		 }},
		{"entry"s,
		 [this](const nlohmann::json& injson, Result& res) {

			 // transform the JSON data into this data structure
			 std::vector<std::pair<DataType, std::string>> dataInputs;

			 if (injson.find("data") != injson.end()) {
				 auto& data = injson["data"];

				 if (data.is_array()) {
					 for (const auto& input : data) {
						 std::string docString;
						 std::string qualifiedType;
						 for (auto iter = input.begin(); iter != input.end(); ++iter) {
							 docString	 = iter.key();
							 qualifiedType = iter.value();
						 }

						 std::string module = qualifiedType.substr(0, qualifiedType.find(':'));
						 std::string type   = qualifiedType.substr(qualifiedType.find(':') + 1);

						 DataType cty;
						 // TODO: maybe not discard res
						 res += context().typeFromModule(module, type, &cty);

						 if (!res) { continue; }

						 dataInputs.emplace_back(cty, docString);
					 }

				 } else {
					 res.addEntry("WUKN", "Data for lang:entry must be an array",
								  {{"Given Data", data}});
				 }

			 } else {
				 res.addEntry("WUKN", "Data for lang:entry must have a data element",
							  {{"Data JSON", injson}});
			 }

			 std::vector<std::string> execInputs;

			 if (injson.find("exec") != injson.end()) {
				 auto& exec = injson["exec"];
				 if (exec.is_array()) {
					 for (const auto& output : exec) { execInputs.push_back(output); }
				 }
			 } else {
				 res.addEntry("WUKN", "Data for lang:entry must have a exec element",
							  {{"Data JSON"}, injson});
			 }

			 if (res) {
				 return std::make_unique<EntryNodeType>(*this, std::move(dataInputs),
														std::move(execInputs));
			 }
			 return std::unique_ptr<EntryNodeType>();
		 }},
		{"exit"s,
		 [this](const nlohmann::json& injson, Result& res) {
			 // transform the JSON data into this data structure
			 std::vector<std::pair<DataType, std::string>> dataOutputs;

			 if (injson.find("data") != injson.end()) {
				 auto& data = injson["data"];

				 if (data.is_array()) {
					 for (const auto& output : data) {
						 std::string docString;
						 std::string qualifiedType;
						 for (auto iter = output.begin(); iter != output.end(); ++iter) {
							 docString	 = iter.key();
							 qualifiedType = iter.value();
						 }

						 std::string module = qualifiedType.substr(0, qualifiedType.find(':'));
						 std::string type   = qualifiedType.substr(qualifiedType.find(':') + 1);

						 DataType cty;
						 // TODO: maybe not discard res
						 context().typeFromModule(module, type, &cty);

						 dataOutputs.emplace_back(cty, docString);
					 }

				 } else {
					 res.addEntry("WUKN", "Data element for lang:exit must be an array",
								  {{"Data JSON", injson}});
				 }

			 } else {
				 res.addEntry("WUKN", "Data for lang:exit must have a data element",
							  {{"Data JSON", injson}});
			 }
			 std::vector<std::string> execOutputs;

			 if (injson.find("exec") != injson.end()) {
				 auto& exec = injson["exec"];
				 if (exec.is_array()) {
					 for (const auto& output : exec) { execOutputs.push_back(output); }
				 } else {
					 res.addEntry("WUKN", "Exec element for lang:exit must be an array",
								  {{"Data JSON", injson}});
				 }

			 } else {
				 res.addEntry("WUKN", "Data for lang:exit must have a exec element",
							  {{"Data JSON"}, injson});
			 }

			 return std::make_unique<ExitNodeType>(*this, std::move(dataOutputs),
												   std::move(execOutputs));

		 }},
		{"const-int"s,
		 [this](const nlohmann::json& data, Result& res) {

			 int num = 0;

			 if (data.is_number_integer()) {
				 num = data;
			 } else {
				 res.addEntry("WUKN", "Data for lang:const-int must be an integer",
							  {{"Given Data", data}});
			 }

			 return std::make_unique<ConstIntNodeType>(*this, num);
		 }},
		{"const-float"s,
		 [this](const nlohmann::json& data, Result& res) {

			 float num = 0;

			 if (data.is_number_float()) {
				 num = data;
			 } else {
				 res.addEntry("WUKN", "Data for lang:const-float must be a float",
							  {{"Given Data", data}});
			 }

			 return std::make_unique<ConstFloatNodeType>(*this, num);
		 }},
		{"const-bool"s,
		 [this](const nlohmann::json& data, Result& res) {

			 bool val = false;

			 if (data.is_boolean()) {
				 val = data;
			 } else {
				 res.addEntry("WUKN", "Data for lang:const-bool must be a boolean",
							  {{"Given Data", data}});
			 }

			 return std::make_unique<ConstBoolNodeType>(*this, val);
		 }},
		{"strliteral"s, [this](const nlohmann::json& data, Result& res) {
			 std::string str;
			 if (data.is_string()) {
				 str = data;
			 } else {
				 res.addEntry("WUKN", "Data for lang:strliteral must be a string",
							  {{"Given Data", data}});
			 }

			 return std::make_unique<StringLiteralNodeType>(*this, str);
		 }}};

	// create debug types
	mDebugTypes["i32"] =
		llvm::DIBasicType::get(context().llvmContext(), llvm::dwarf::DW_TAG_base_type, "lang:i32",
							   32, 32, llvm::dwarf::DW_ATE_signed);
	mDebugTypes["i1"] = 
		llvm::DIBasicType::get(context().llvmContext(), llvm::dwarf::DW_TAG_base_type, "lang:i1", 8,
							   8, llvm::dwarf::DW_ATE_boolean);
	mDebugTypes["double"] =
		llvm::DIBasicType::get(context().llvmContext(), llvm::dwarf::DW_TAG_base_type,
							   "lang:double", 64, 64, llvm::dwarf::DW_ATE_float);
	auto charType = llvm::DIBasicType::get(context().llvmContext(), llvm::dwarf::DW_TAG_base_type,
										   "lang:i8", 8, 8, llvm::dwarf::DW_ATE_unsigned_char);
	mDebugTypes["i8*"] = llvm::DIDerivedType::get(
		context().llvmContext(), llvm::dwarf::DW_TAG_pointer_type, nullptr, nullptr, 0, nullptr,
		charType, 64, 64, 0, 0);  // TODO: 32bit support?
}

Result LangModule::nodeTypeFromName(gsl::cstring_span<> name, const nlohmann::json& jsonData,
									std::unique_ptr<NodeType>* toFill) {
	Result res;

	auto iter = nodes.find(gsl::to_string(name));
	if (iter != nodes.end()) {
		*toFill = iter->second(jsonData, res);
		return res;
	}

	res.addEntry("E37", "Failed to find node in module",
				 {{"Module", "lang"}, {"Requested Node Type", gsl::to_string(name)}});

	return res;
}

llvm::DIType* LangModule::debugTypeFromName(gsl::cstring_span<> name) {
	auto iter = mDebugTypes.find(gsl::to_string(name));
	if (iter != mDebugTypes.end()) { return iter->second; }
	return nullptr;
}

// the lang module just has the basic llvm types.
DataType LangModule::typeFromName(gsl::cstring_span<> name) {
	using namespace std::string_literals;

	// just parse the type
	auto IR		   = "@G = external global "s + gsl::to_string(name);
	auto err	   = llvm::SMDiagnostic();
	auto tmpModule = llvm::parseAssemblyString(IR, err, context().llvmContext());
	if (!tmpModule) { return nullptr; }

	// returns the pointer type, so get the contained type
	return {this, gsl::to_string(name),
			tmpModule->getNamedValue("G")->getType()->getContainedType(0)};
}

}  // namespace chig
