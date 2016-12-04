
#include "chig/LangModule.hpp"
#include "chig/Context.hpp"

#include <llvm/AsmParser/Parser.h>
#include <llvm/AsmParser/Parser.h>
#include <llvm/Support/SourceMgr.h>
#include <gsl/gsl_assert>

using namespace chig;

LangModule::LangModule(Context& ctx) : ChigModule(ctx)
{
	using namespace std::string_literals;

	setName("lang");

	// populate them
	nodes = {{"if"s, [this](const nlohmann::json&,
						 Result& res) { return std::make_unique<IfNodeType>(*this); }},
		{"entry"s,
			[this](const nlohmann::json& data, Result& res) {

				// transform the JSON data into this data structure
				std::vector<std::pair<DataType, std::string>> inputs;

				if (data.is_array()) {
					for (const auto& input : data) {
						std::string docString;
						std::string qualifiedType;
						for (auto iter = input.begin(); iter != input.end(); ++iter) {
							docString = iter.key();
							qualifiedType = iter.value();
						}

						std::string module = qualifiedType.substr(0, qualifiedType.find(':'));
						std::string type = qualifiedType.substr(qualifiedType.find(':') + 1);

						DataType cty;
						// TODO: maybe not discard res
						res += context().typeFromModule(module, type, &cty);

						if (!res) {
							continue;
						}

						inputs.emplace_back(cty, docString);
					}

				} else {
					res.add_entry(
						"WUKN", "Data for lang:entry must be an array", {{"Given Data", data}});
				}

				if (res) {
					return std::make_unique<EntryNodeType>(*this, inputs);
				}
				return std::unique_ptr<EntryNodeType>();
			}},
		{"exit"s,
			[this](const nlohmann::json& data, Result& res) {
				// transform the JSON data into this data structure
				std::vector<std::pair<DataType, std::string>> outputs;

				if (data.is_array()) {
					for (const auto& output : data) {
						std::string docString;
						std::string qualifiedType;
						for (auto iter = output.begin(); iter != output.end(); ++iter) {
							docString = iter.key();
							qualifiedType = iter.value();
						}

						std::string module = qualifiedType.substr(0, qualifiedType.find(':'));
						std::string type = qualifiedType.substr(qualifiedType.find(':') + 1);

						DataType cty;
						// TODO: maybe not discard res
						context().typeFromModule(module, type, &cty);

						outputs.emplace_back(cty, docString);
					}

				} else {
					res.add_entry(
						"WUKN", "Data for lang:exit must be an array", {{"Given Data", data}});
				}

				return std::make_unique<ExitNodeType>(*this, outputs);

			}},
		{"const-int"s,
			[this](const nlohmann::json& data, Result& res) {

				int num = 0;

				if (data.is_number_integer()) {
					num = data;
				} else {
					res.add_entry("WUKN", "Data for lang:const-int must be an integer",
						{{"Given Data", data}});
				}

				return std::make_unique<ConstIntNodeType>(*this, num);
			}},
		{"const-bool"s,
			[this](const nlohmann::json& data, Result& res) {

				bool val = false;

				if (data.is_boolean()) {
					val = data;
				} else {
					res.add_entry("WUKN", "Data for lang:const-bool must be a boolean",
						{{"Given Data", data}});
				}

				return std::make_unique<ConstBoolNodeType>(*this, val);
			}},
		{"strliteral"s, [this](const nlohmann::json& data, Result& res) {
			 std::string str;
			 if (data.is_string()) {
				 str = data;
			 } else {
				 res.add_entry(
					 "WUKN", "Data for lang:strliteral must be a string", {{"Given Data", data}});
			 }

			 return std::make_unique<StringLiteralNodeType>(*this, str);
		 }}};
}

Result LangModule::nodeTypeFromName(
	gsl::cstring_span<> name, const nlohmann::json& json_data, std::unique_ptr<NodeType>* toFill)
{
	Result res;

	auto iter = nodes.find(gsl::to_string(name));
	if (iter != nodes.end()) {
		*toFill = iter->second(json_data, res);
		return res;
	}

	res.add_entry("E37", "Failed to find node in module",
		{{"Module", "lang"}, {"Requested Node Type", gsl::to_string(name)}});

	return res;
}

// the lang module just has the basic llvm types.
DataType LangModule::typeFromName(gsl::cstring_span<> name)
{
	using namespace std::string_literals;

	// just parse the type
	auto IR = "@G = external global "s + gsl::to_string(name);
	auto err = llvm::SMDiagnostic();

	auto lltype = llvm::parseType(
		gsl::to_string(name), err, llvm::Module("tmp", context().llvmContext()), nullptr);
	if (!lltype) {
		return {};
	}

	return {this, gsl::to_string(name), lltype};
}

Result IfNodeType::codegen(size_t /*inExecID*/, llvm::Module* /*mod*/, llvm::Function* /*f*/,
	const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
	const gsl::span<llvm::BasicBlock*> outputBlocks) const
{
	Expects(io.size() == 1 && codegenInto != nullptr && outputBlocks.size() == 2);

	llvm::IRBuilder<> builder(codegenInto);
	builder.CreateCondBr(io[0], outputBlocks[0], outputBlocks[1]);

	return {};
}

IfNodeType::IfNodeType(LangModule& mod) : NodeType(mod)
{
	name = "if";
	description = "branch on a bool";

	execInputs = {""};
	execOutputs = {"True", "False"};

	dataInputs = {{mod.typeFromName("i1"), "condition"}};
}

std::unique_ptr<chig::NodeType, std::default_delete<chig::NodeType>> IfNodeType::clone() const
{
	return std::make_unique<IfNodeType>(*this);
}

EntryNodeType::EntryNodeType(
	LangModule& mod, const gsl::span<std::pair<DataType, std::string>> funInputs)
	: NodeType{mod}
{
	name = "entry";
	description = "entry to a function";

	execOutputs = {""};

	dataOutputs = {funInputs.begin(), funInputs.end()};
}

Result EntryNodeType::codegen(size_t /*inputConnId*/, llvm::Module* /*mod*/, llvm::Function* f,
	const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
	const gsl::span<llvm::BasicBlock*> outputBlocks) const
{
	Expects(f != nullptr && io.size() == dataOutputs.size() && codegenInto != nullptr &&
			outputBlocks.size() == 1);

	llvm::IRBuilder<> builder(codegenInto);

	// store the arguments
	auto arg_iter = f->arg_begin();
	for (const auto& iovalue : io) {
		builder.CreateStore(&*arg_iter, iovalue);

		++arg_iter;
	}

	builder.CreateBr(outputBlocks[0]);

	return {};
}

std::unique_ptr<NodeType> EntryNodeType::clone() const
{
	return std::make_unique<EntryNodeType>(*this);
}

nlohmann::json EntryNodeType::toJSON() const
{
	nlohmann::json ret = nlohmann::json::array();

	for (auto& pair : dataOutputs) {
		ret.push_back({{pair.second, pair.first.getQualifiedName()}});
	}

	return ret;
}

ConstIntNodeType::ConstIntNodeType(LangModule& mod, int num) : NodeType{mod}, number{num}
{
	name = "const-int";
	description = "constant int value";

	execInputs = {""};
	execOutputs = {""};

	dataOutputs = {{mod.typeFromName("i32"), "out"}};
}

Result ConstIntNodeType::codegen(size_t /*inputExecID*/, llvm::Module* /*mod*/,
	llvm::Function* /*f*/, const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
	const gsl::span<llvm::BasicBlock*> outputBlocks) const
{
	Expects(io.size() == 1 && codegenInto != nullptr && outputBlocks.size() == 1);

	llvm::IRBuilder<> builder(codegenInto);
	// just go to the block

	builder.CreateStore(
		llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(context->llvmContext()), number), io[0],
		false);
	builder.CreateBr(outputBlocks[0]);

	return {};
}

std::unique_ptr<chig::NodeType> ConstIntNodeType::clone() const
{
	return std::make_unique<ConstIntNodeType>(*this);
}

nlohmann::json ConstIntNodeType::toJSON() const { return number; }
ConstBoolNodeType::ConstBoolNodeType(LangModule& mod, bool num) : NodeType{mod}, value{num}
{
	name = "const-bool";
	description = "constant boolean value";

	execInputs = {""};
	execOutputs = {""};

	dataOutputs = {{mod.typeFromName("i1"), "out"}};
}

Result ConstBoolNodeType::codegen(size_t /*inputExecID*/, llvm::Module* /*mod*/,
	llvm::Function* /*f*/, const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
	const gsl::span<llvm::BasicBlock*> outputBlocks) const
{
	Expects(io.size() == 1 && codegenInto != nullptr && outputBlocks.size() == 1);

	llvm::IRBuilder<> builder(codegenInto);
	// just go to the block

	builder.CreateStore(llvm::ConstantInt::get(llvm::IntegerType::getInt1Ty(context->llvmContext()),
							static_cast<uint64_t>(value)),
		io[0], false);
	builder.CreateBr(outputBlocks[0]);

	return {};
}

std::unique_ptr<chig::NodeType> ConstBoolNodeType::clone() const
{
	return std::make_unique<ConstBoolNodeType>(*this);
}

nlohmann::json ConstBoolNodeType::toJSON() const { return value; }
ExitNodeType::ExitNodeType(
	LangModule& mod, const gsl::span<std::pair<DataType, std::string>> funOutputs)
	: NodeType{mod}
{
	execInputs = {""};

	name = "exit";
	description = "exit from a function; think return";

	dataInputs = {funOutputs.begin(), funOutputs.end()};
}

Result ExitNodeType::codegen(size_t execInputID, llvm::Module* /*mod*/, llvm::Function* f,
	const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
	const gsl::span<llvm::BasicBlock*> /*outputBlocks*/) const
{
	Expects(execInputID < execInputs.size() && f != nullptr && io.size() == dataInputs.size() &&
			codegenInto != nullptr);

	// assign the return types
	llvm::IRBuilder<> builder(codegenInto);
	size_t ret_start =
		f->arg_size() - io.size();  // returns are after args, find where returns start
	auto arg_iter = f->arg_begin();
	std::advance(arg_iter, ret_start);
	for (auto& value : io) {
		builder.CreateStore(value, &*arg_iter, false);  // TODO: volitility?
		++arg_iter;
	}

	builder.CreateRet(
		llvm::ConstantInt::get(llvm::Type::getInt32Ty(context->llvmContext()), execInputID));

	return {};
}

std::unique_ptr<NodeType> ExitNodeType::clone() const
{
	return std::make_unique<ExitNodeType>(*this);
}

nlohmann::json ExitNodeType::toJSON() const
{
	nlohmann::json ret = nlohmann::json::array();

	for (auto& pair : dataInputs) {
		ret.push_back({{pair.second, pair.first.getQualifiedName()}});
	}

	return ret;
}

StringLiteralNodeType::StringLiteralNodeType(LangModule& mod, std::string str)
	: NodeType{mod}, literalString(std::move(str))
{
	execInputs = {""};
	execOutputs = {""};

	name = "strliteral";
	description = "exit from a function; think return";

	// TODO: research address types
	dataOutputs = {{mod.typeFromName("i8*"), "string"}};
}

Result StringLiteralNodeType::codegen(size_t /*execInputID*/, llvm::Module* /*mod*/,
	llvm::Function* /*f*/, const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
	const gsl::span<llvm::BasicBlock*> outputBlocks) const
{
	Expects(io.size() == 1 && codegenInto != nullptr && outputBlocks.size() == 1);

	llvm::IRBuilder<> builder(codegenInto);

	auto global = builder.CreateGlobalString(literalString);

	auto const0ID = llvm::ConstantInt::get(context->llvmContext(), llvm::APInt(32, 0, false));
	auto gep = builder.CreateGEP(global, {const0ID, const0ID});
	builder.CreateStore(gep, io[0], false);

	builder.CreateBr(outputBlocks[0]);

	return {};
}

std::unique_ptr<NodeType> StringLiteralNodeType::clone() const
{
	return std::make_unique<StringLiteralNodeType>(*this);
}

nlohmann::json StringLiteralNodeType::toJSON() const { return literalString; }
