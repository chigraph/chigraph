/// \file LangModule.cpp

#include "chi/LangModule.hpp"
#include "chi/Context.hpp"
#include "chi/DataType.hpp"
#include "chi/NodeType.hpp"
#include "chi/Result.hpp"

#include <llvm/AsmParser/Parser.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/IR/DebugInfo.h>

namespace chi {

namespace {
/// NodeType for conditionals
struct IfNodeType : NodeType {
	IfNodeType(LangModule& mod) : NodeType(mod, "if", "branch on a bools") {
		setExecInputs({""});
		setExecOutputs({"True", "False"});

		setDataInputs({{"condition", mod.typeFromName("i1")}});
	}

	Result codegen(
	    size_t /*execInputID*/, const llvm::DebugLoc& nodeLocation,
	    const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
	    const gsl::span<llvm::BasicBlock*> outputBlocks,
	    std::unordered_map<std::string, std::shared_ptr<void>>& /*compileCache*/) override {
		Expects(io.size() == 1 && codegenInto != nullptr && outputBlocks.size() == 2);

		llvm::IRBuilder<> builder(codegenInto);
		builder.SetCurrentDebugLocation(nodeLocation);

		builder.CreateCondBr(io[0], outputBlocks[0], outputBlocks[1]);

		return {};
	}

	std::unique_ptr<NodeType> clone() const override { return std::make_unique<IfNodeType>(*this); }
};

struct EntryNodeType : NodeType {
	EntryNodeType(LangModule& mod, std::vector<NamedDataType> dataInputs,
	              std::vector<std::string> execInputs)
	    : NodeType(mod, "entry", "entry to a function") {
		setExecOutputs(std::move(execInputs));

		setDataOutputs(std::move(dataInputs));
	}

	Result codegen(
	    size_t /*inputExecID*/, const llvm::DebugLoc& nodeLocation,
	    const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
	    const gsl::span<llvm::BasicBlock*> outputBlocks,
	    std::unordered_map<std::string, std::shared_ptr<void>>& /*compileCache*/) override {
		Expects(io.size() == dataOutputs().size() && codegenInto != nullptr &&
		        outputBlocks.size() == execOutputs().size());

		llvm::IRBuilder<> builder(codegenInto);
		builder.SetCurrentDebugLocation(nodeLocation);

		// store the arguments
		auto arg_iter = codegenInto->getParent()->arg_begin();
		++arg_iter;  // skip the first argument, which is the input exec ID
		for (const auto& iovalue : io) {
			builder.CreateStore(&*arg_iter, iovalue);

			++arg_iter;
		}

		auto inExecID   = &*codegenInto->getParent()->arg_begin();
		auto switchInst = builder.CreateSwitch(inExecID, outputBlocks[0], execOutputs().size());

		for (auto id = 0ull; id < execOutputs().size(); ++id) {
			switchInst->addCase(builder.getInt32(id), outputBlocks[id]);
		}
		return {};
	}

	std::unique_ptr<NodeType> clone() const override {
		return std::make_unique<EntryNodeType>(*this);
	}

	nlohmann::json toJSON() const override {
		nlohmann::json ret = nlohmann::json::object();

		auto& data = ret["data"];
		data       = nlohmann::json::array();
		for (auto& pair : dataOutputs()) {
			data.push_back({{pair.name, pair.type.qualifiedName()}});
		}

		auto& exec = ret["exec"];
		exec       = nlohmann::json::array();
		for (auto& name : execOutputs()) { exec.push_back(name); }

		return ret;
	}
};

struct ConstIntNodeType : NodeType {
	ConstIntNodeType(LangModule& mod, int num)
	    : NodeType(mod, "const-int", "Int literal"), number(num) {
		makePure();

		setDataOutputs({{"", mod.typeFromName("i32")}});
	}

	Result codegen(
	    size_t /*inputExecID*/, const llvm::DebugLoc& nodeLocation,
	    const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
	    const gsl::span<llvm::BasicBlock*> outputBlocks,
	    std::unordered_map<std::string, std::shared_ptr<void>>& /*compileCache*/) override {
		Expects(io.size() == 1 && codegenInto != nullptr && outputBlocks.size() == 1);

		llvm::IRBuilder<> builder(codegenInto);
		builder.SetCurrentDebugLocation(nodeLocation);

		builder.CreateStore(builder.getInt32(number), io[0], false);
		builder.CreateBr(outputBlocks[0]);

		return {};
	}

	std::unique_ptr<NodeType> clone() const override {
		return std::make_unique<ConstIntNodeType>(*this);
	}

	nlohmann::json toJSON() const override { return number; }
	int            number;
};

struct ConstFloatNodeType : NodeType {
	ConstFloatNodeType(LangModule& mod, double num)
	    : NodeType(mod, "const-float", "Float Literal"), number(num) {
		makePure();

		setDataOutputs({{"", mod.typeFromName("float")}});
	}

	Result codegen(
	    size_t /*inputExecID*/, const llvm::DebugLoc& nodeLocation,
	    const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
	    const gsl::span<llvm::BasicBlock*> outputBlocks,
	    std::unordered_map<std::string, std::shared_ptr<void>>& /*compileCache*/) override {
		Expects(io.size() == 1 && codegenInto != nullptr && outputBlocks.size() == 1);

		llvm::IRBuilder<> builder(codegenInto);
		builder.SetCurrentDebugLocation(nodeLocation);

		builder.CreateStore(llvm::ConstantFP::get(builder.getFloatTy(), number), io[0]);
		builder.CreateBr(outputBlocks[0]);

		return {};
	}

	std::unique_ptr<NodeType> clone() const override {
		return std::make_unique<ConstFloatNodeType>(*this);
	}

	nlohmann::json toJSON() const override { return number; }
	double         number;
};

struct ConstBoolNodeType : NodeType {
	ConstBoolNodeType(LangModule& mod, bool num)
	    : NodeType{mod, "const-bool", "Boolean literal"}, value{num} {
		makePure();

		setDataOutputs({{"", mod.typeFromName("i1")}});
	}

	Result codegen(
	    size_t /*inputExecID*/, const llvm::DebugLoc& nodeLocation,
	    const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
	    const gsl::span<llvm::BasicBlock*> outputBlocks,
	    std::unordered_map<std::string, std::shared_ptr<void>>& /*compileCache*/) override {
		Expects(io.size() == 1 && codegenInto != nullptr && outputBlocks.size() == 1);

		llvm::IRBuilder<> builder(codegenInto);
		builder.SetCurrentDebugLocation(nodeLocation);

		builder.CreateStore(builder.getInt1(value), io[0], false);
		builder.CreateBr(outputBlocks[0]);

		return {};
	}

	std::unique_ptr<NodeType> clone() const override {
		return std::make_unique<ConstBoolNodeType>(*this);
	}

	nlohmann::json toJSON() const override { return value; }
	bool           value;
};

struct ExitNodeType : NodeType {
	ExitNodeType(LangModule& mod, std::vector<NamedDataType> dataOutputs,
	             std::vector<std::string> execOutputs)
	    : NodeType{mod, "exit", "Return from a function"} {
		// outputs to the function are inputs to the node
		setExecInputs(std::move(execOutputs));

		setDataInputs(std::move(dataOutputs));
	}

	Result codegen(
	    size_t execInputID, const llvm::DebugLoc& nodeLocation, const gsl::span<llvm::Value*> io,
	    llvm::BasicBlock* codegenInto, const gsl::span<llvm::BasicBlock*> /*outputBlocks*/,
	    std::unordered_map<std::string, std::shared_ptr<void>>& /*compileCache*/) override {
		Expects(execInputID < execInputs().size() && io.size() == dataInputs().size() &&
		        codegenInto != nullptr);

		// assign the return types
		llvm::IRBuilder<> builder(codegenInto);
		builder.SetCurrentDebugLocation(nodeLocation);

		llvm::Function* f = codegenInto->getParent();
		size_t          ret_start =
		    f->arg_size() - io.size();  // returns are after args, find where returns start
		auto arg_iter = f->arg_begin();
		std::advance(arg_iter, ret_start);
		for (auto& value : io) {
			builder.CreateStore(value, &*arg_iter, false);  // TODO: volitility?
			++arg_iter;
		}

		builder.CreateRet(builder.getInt32(execInputID));

		return {};
	}

	std::unique_ptr<NodeType> clone() const override {
		return std::make_unique<ExitNodeType>(*this);
	}

	nlohmann::json toJSON() const override {
		nlohmann::json ret = nlohmann::json::object();

		auto& data = ret["data"];
		data       = nlohmann::json::array();
		for (auto& pair : dataInputs()) {
			data.push_back({{pair.name, pair.type.qualifiedName()}});
		}

		auto& exec = ret["exec"];
		exec       = nlohmann::json::array();
		for (auto& name : execInputs()) { exec.push_back(name); }

		return ret;
	}
};

struct StringLiteralNodeType : NodeType {
	StringLiteralNodeType(LangModule& mod, std::string str)
	    : NodeType(mod, "strliteral", "string literal"), literalString(std::move(str)) {
		makePure();

		setDataOutputs({{"", mod.typeFromName("i8*")}});
	}

	Result codegen(
	    size_t /*execInputID*/, const llvm::DebugLoc& nodeLocation,
	    const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
	    const gsl::span<llvm::BasicBlock*> outputBlocks,
	    std::unordered_map<std::string, std::shared_ptr<void>>& /*compileCache*/) override {
		Expects(io.size() == 1 && codegenInto != nullptr && outputBlocks.size() == 1);

		llvm::IRBuilder<> builder(codegenInto);
		builder.SetCurrentDebugLocation(nodeLocation);

		auto global = builder.CreateGlobalString(literalString);

		auto const0ID = llvm::ConstantInt::get(context().llvmContext(), llvm::APInt(32, 0, false));
		auto gep      = builder.CreateGEP(global, {const0ID, const0ID});
		builder.CreateStore(gep, io[0], false);

		builder.CreateBr(outputBlocks[0]);

		return {};
	}

	std::unique_ptr<NodeType> clone() const override {
		return std::make_unique<StringLiteralNodeType>(*this);
	}

	nlohmann::json toJSON() const override { return literalString; }
	std::string    literalString;
};

struct IntToFloatNodeType : NodeType {
	IntToFloatNodeType(LangModule& mod) : NodeType(mod, "inttofloat", "convert integer to float") {
		makePure();

		setDataInputs({{"", mod.typeFromName("i32")}});
		setDataOutputs({{"", mod.typeFromName("float")}});
	}

	Result codegen(
	    size_t /*execInputID*/, const llvm::DebugLoc& nodeLocation,
	    const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
	    const gsl::span<llvm::BasicBlock*> outputBlocks,
	    std::unordered_map<std::string, std::shared_ptr<void>>& /*compileCache*/) override {
		Expects(io.size() == 2 && codegenInto != nullptr && outputBlocks.size() == 1);

		llvm::IRBuilder<> builder(codegenInto);
		builder.SetCurrentDebugLocation(nodeLocation);

		auto casted =
		    builder.CreateCast(llvm::Instruction::CastOps::SIToFP, io[0], builder.getFloatTy());
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
		makePure();

		setDataInputs({{"", mod.typeFromName("float")}});
		setDataOutputs({{"", mod.typeFromName("i32")}});
	}

	Result codegen(
	    size_t /*execInputID*/, const llvm::DebugLoc& nodeLocation,
	    const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
	    const gsl::span<llvm::BasicBlock*> outputBlocks,
	    std::unordered_map<std::string, std::shared_ptr<void>>& /*compileCache*/) override {
		Expects(io.size() == 2 && codegenInto != nullptr && outputBlocks.size() == 1);

		llvm::IRBuilder<> builder(codegenInto);
		builder.SetCurrentDebugLocation(nodeLocation);

		auto casted =
		    builder.CreateCast(llvm::Instruction::CastOps::FPToSI, io[0], builder.getInt32Ty());
		builder.CreateStore(casted, io[1]);

		builder.CreateBr(outputBlocks[0]);

		return {};
	}

	std::unique_ptr<NodeType> clone() const override {
		return std::make_unique<FloatToIntNodeType>(*this);
	}
};

/// \internal
enum class BinOp { Add, Subtract, Multiply, Divide };

struct BinaryOperationNodeType : NodeType {
	BinaryOperationNodeType(LangModule& mod, DataType ty, BinOp binaryOperation)
	    : NodeType(mod), mBinOp(binaryOperation), mType{ty} {
		makePure();

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

		setDataInputs({{"a", ty}, {"b", ty}});
		setDataOutputs({{"", ty}});
	}

	Result codegen(
	    size_t /*execInputID*/, const llvm::DebugLoc& nodeLocation,
	    const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
	    const gsl::span<llvm::BasicBlock*> outputBlocks,
	    std::unordered_map<std::string, std::shared_ptr<void>>& /*compileCache*/) override {
		Expects(io.size() == 3 && codegenInto != nullptr && outputBlocks.size() == 1);

		llvm::IRBuilder<> builder(codegenInto);
		builder.SetCurrentDebugLocation(nodeLocation);

		llvm::Value* result = nullptr;
		if (mType.unqualifiedName() == "i32") {
			result = [&](BinOp b) {
				switch (b) {
				case BinOp::Add: return builder.CreateAdd(io[0], io[1]);
				case BinOp::Subtract: return builder.CreateSub(io[0], io[1]);
				case BinOp::Multiply: return builder.CreateMul(io[0], io[1]);
				case BinOp::Divide: return builder.CreateSDiv(io[0], io[1]);
				default: return (llvm::Value*)nullptr;
				}
				return (llvm::Value*)nullptr;
			}(mBinOp);

		} else {
			result = [&](BinOp b) {
				switch (b) {
				case BinOp::Add: return builder.CreateFAdd(io[0], io[1]);
				case BinOp::Subtract: return builder.CreateFSub(io[0], io[1]);
				case BinOp::Multiply: return builder.CreateFMul(io[0], io[1]);
				case BinOp::Divide: return builder.CreateFDiv(io[0], io[1]);
				default: return (llvm::Value*)nullptr;
				}
				return (llvm::Value*)nullptr;
			}(mBinOp);
		}

		builder.CreateStore(result, io[2]);

		builder.CreateBr(outputBlocks[0]);

		return {};
	}

	std::unique_ptr<NodeType> clone() const override {
		return std::make_unique<BinaryOperationNodeType>(*this);
	}

	BinOp    mBinOp;
	DataType mType;
};

enum class CmpOp { Lt, Gt, Let, Get, Eq, Neq };

struct CompareNodeType : NodeType {
	CompareNodeType(LangModule& mod, DataType ty, CmpOp op)
	    : NodeType(mod), mCompOp(op), mType{ty} {
		makePure();

		std::string opStr = [](CmpOp b) {
			switch (b) {
			case CmpOp::Lt: return "<";
			case CmpOp::Gt: return ">";
			case CmpOp::Let: return "<=";
			case CmpOp::Get: return ">=";
			case CmpOp::Eq: return "==";
			case CmpOp::Neq: return "!=";
			default: return "";
			}
			return "";
		}(mCompOp);

		setName(ty.unqualifiedName() + opStr + ty.unqualifiedName());
		setDescription(ty.unqualifiedName() + opStr + ty.unqualifiedName());

		setDataInputs({{"a", ty}, {"b", ty}});
		setDataOutputs({{"", mod.typeFromName("i1")}});
	}

	Result codegen(
	    size_t /*execInputID*/, const llvm::DebugLoc& nodeLocation,
	    const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
	    const gsl::span<llvm::BasicBlock*> outputBlocks,
	    std::unordered_map<std::string, std::shared_ptr<void>>& /*compileCache*/) override {
		Expects(io.size() == 3 && codegenInto != nullptr && outputBlocks.size() == 1);

		llvm::IRBuilder<> builder(codegenInto);
		builder.SetCurrentDebugLocation(nodeLocation);

		llvm::Value* result = nullptr;
		if (mType.unqualifiedName() == "i32") {
			result = [&](CmpOp b) -> llvm::Value* {
				switch (b) {
				case CmpOp::Lt: return builder.CreateICmpSLT(io[0], io[1]);
				case CmpOp::Gt: return builder.CreateICmpSGT(io[0], io[1]);
				case CmpOp::Let: return builder.CreateICmpSLE(io[0], io[1]);
				case CmpOp::Get: return builder.CreateICmpSGE(io[0], io[1]);
				case CmpOp::Eq: return builder.CreateICmpEQ(io[0], io[1]);
				case CmpOp::Neq: return builder.CreateICmpNE(io[0], io[1]);
				default: return nullptr;
				}
				return nullptr;
			}(mCompOp);

		} else {
			result = [&](CmpOp b) -> llvm::Value* {
				switch (b) {
				case CmpOp::Lt: return builder.CreateFCmpULT(io[0], io[1]);
				case CmpOp::Gt: return builder.CreateFCmpUGT(io[0], io[1]);
				case CmpOp::Let: return builder.CreateFCmpULE(io[0], io[1]);
				case CmpOp::Get: return builder.CreateFCmpUGE(io[0], io[1]);
				case CmpOp::Eq: return builder.CreateFCmpUEQ(io[0], io[1]);
				case CmpOp::Neq: return builder.CreateFCmpUNE(io[0], io[1]);
				default: return nullptr;
				}
				return nullptr;
			}(mCompOp);
		}

		builder.CreateStore(result, io[2]);

		builder.CreateBr(outputBlocks[0]);

		return {};
	}

	std::unique_ptr<NodeType> clone() const override {
		return std::make_unique<CompareNodeType>(*this);
	}

	CmpOp    mCompOp;
	DataType mType;
};

}  // anonymous namespace

LangModule::LangModule(Context& ctx) : ChiModule(ctx, "lang") {
	using namespace std::string_literals;

	// populate them
	nodes = {
	    {"if"s,
	     [this](const nlohmann::json&, Result&) { return std::make_unique<IfNodeType>(*this); }},
	    {"i32+i32"s,
	     [this](const nlohmann::json&, Result&) {
		     return std::make_unique<BinaryOperationNodeType>(
		         *this, LangModule::typeFromName("i32"), BinOp::Add);
		 }},
	    {"i32-i32"s,
	     [this](const nlohmann::json&, Result&) {
		     return std::make_unique<BinaryOperationNodeType>(
		         *this, LangModule::typeFromName("i32"), BinOp::Subtract);
		 }},
	    {"i32*i32"s,
	     [this](const nlohmann::json&, Result&) {
		     return std::make_unique<BinaryOperationNodeType>(
		         *this, LangModule::typeFromName("i32"), BinOp::Multiply);
		 }},
	    {"i32/i32"s,
	     [this](const nlohmann::json&, Result&) {
		     return std::make_unique<BinaryOperationNodeType>(
		         *this, LangModule::typeFromName("i32"), BinOp::Divide);
		 }},
	    {"float+float"s,
	     [this](const nlohmann::json&, Result&) {
		     return std::make_unique<BinaryOperationNodeType>(
		         *this, LangModule::typeFromName("float"), BinOp::Add);
		 }},
	    {"float-float"s,
	     [this](const nlohmann::json&, Result&) {
		     return std::make_unique<BinaryOperationNodeType>(
		         *this, LangModule::typeFromName("float"), BinOp::Subtract);
		 }},
	    {"float*float"s,
	     [this](const nlohmann::json&, Result&) {
		     return std::make_unique<BinaryOperationNodeType>(
		         *this, LangModule::typeFromName("float"), BinOp::Multiply);
		 }},
	    {"float/float"s,
	     [this](const nlohmann::json&, Result&) {
		     return std::make_unique<BinaryOperationNodeType>(
		         *this, LangModule::typeFromName("float"), BinOp::Divide);
		 }},
	    {"i32<i32"s,
	     [this](const nlohmann::json&, Result&) {
		     return std::make_unique<CompareNodeType>(*this, LangModule::typeFromName("i32"),
		                                              CmpOp::Lt);
		 }},
	    {"i32>i32"s,
	     [this](const nlohmann::json&, Result&) {
		     return std::make_unique<CompareNodeType>(*this, LangModule::typeFromName("i32"),
		                                              CmpOp::Gt);
		 }},
	    {"i32<=i32"s,
	     [this](const nlohmann::json&, Result&) {
		     return std::make_unique<CompareNodeType>(*this, LangModule::typeFromName("i32"),
		                                              CmpOp::Let);
		 }},
	    {"i32>=i32"s,
	     [this](const nlohmann::json&, Result&) {
		     return std::make_unique<CompareNodeType>(*this, LangModule::typeFromName("i32"),
		                                              CmpOp::Get);
		 }},
	    {"i32==i32"s,
	     [this](const nlohmann::json&, Result&) {
		     return std::make_unique<CompareNodeType>(*this, LangModule::typeFromName("i32"),
		                                              CmpOp::Eq);
		 }},
	    {"i32!=i32"s,
	     [this](const nlohmann::json&, Result&) {
		     return std::make_unique<CompareNodeType>(*this, LangModule::typeFromName("i32"),
		                                              CmpOp::Neq);
		 }},
	    {"float<float"s,
	     [this](const nlohmann::json&, Result&) {
		     return std::make_unique<CompareNodeType>(*this, LangModule::typeFromName("float"),
		                                              CmpOp::Lt);
		 }},
	    {"float>float"s,
	     [this](const nlohmann::json&, Result&) {
		     return std::make_unique<CompareNodeType>(*this, LangModule::typeFromName("float"),
		                                              CmpOp::Gt);
		 }},
	    {"float<=float"s,
	     [this](const nlohmann::json&, Result&) {
		     return std::make_unique<CompareNodeType>(*this, LangModule::typeFromName("float"),
		                                              CmpOp::Let);
		 }},
	    {"float>=float"s,
	     [this](const nlohmann::json&, Result&) {
		     return std::make_unique<CompareNodeType>(*this, LangModule::typeFromName("float"),
		                                              CmpOp::Get);
		 }},
	    {"float==float"s,
	     [this](const nlohmann::json&, Result&) {
		     return std::make_unique<CompareNodeType>(*this, LangModule::typeFromName("float"),
		                                              CmpOp::Eq);
		 }},
	    {"float!=float"s,
	     [this](const nlohmann::json&, Result&) {
		     return std::make_unique<CompareNodeType>(*this, LangModule::typeFromName("float"),
		                                              CmpOp::Neq);
		 }},
	    {"inttofloat"s, [this](const nlohmann::json&,
	                           Result&) { return std::make_unique<IntToFloatNodeType>(*this); }},
	    {"floattoint"s, [this](const nlohmann::json&,
	                           Result&) { return std::make_unique<FloatToIntNodeType>(*this); }},
	    {"entry"s,
	     [this](const nlohmann::json& injson, Result& res) {

		     // transform the JSON data into this data structure
		     std::vector<NamedDataType> dataInputs;

		     if (injson.find("data") != injson.end()) {
			     auto& data = injson["data"];

			     if (data.is_array()) {
				     for (const auto& input : data) {
					     std::string docString;
					     std::string qualifiedType;
					     for (auto iter = input.begin(); iter != input.end(); ++iter) {
						     docString     = iter.key();
						     qualifiedType = iter.value();
					     }

					     std::string module = qualifiedType.substr(0, qualifiedType.find(':'));
					     std::string type   = qualifiedType.substr(qualifiedType.find(':') + 1);

					     DataType cty;
					     // TODO: maybe not discard res
					     res += context().typeFromModule(module, type, &cty);

					     if (!res) { continue; }

					     dataInputs.emplace_back(docString, cty);
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
		     std::vector<NamedDataType> dataOutputs;

		     if (injson.find("data") != injson.end()) {
			     auto& data = injson["data"];

			     if (data.is_array()) {
				     for (const auto& output : data) {
					     std::string docString;
					     std::string qualifiedType;
					     for (auto iter = output.begin(); iter != output.end(); ++iter) {
						     docString     = iter.key();
						     qualifiedType = iter.value();
					     }

					     std::string module = qualifiedType.substr(0, qualifiedType.find(':'));
					     std::string type   = qualifiedType.substr(qualifiedType.find(':') + 1);

					     DataType cty;
					     // TODO: maybe not discard res
					     context().typeFromModule(module, type, &cty);

					     dataOutputs.emplace_back(docString, cty);
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

		     if (data.is_number()) {
			     num = data;
		     } else {
			     res.addEntry("WUKN", "Data for lang:const-float must be a number",
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
	mDebugTypes["float"] =
	    llvm::DIBasicType::get(context().llvmContext(), llvm::dwarf::DW_TAG_base_type, "lang:float",
	                           64, 64, llvm::dwarf::DW_ATE_float);
	auto charType = llvm::DIBasicType::get(context().llvmContext(), llvm::dwarf::DW_TAG_base_type,
	                                       "lang:i8", 8, 8, llvm::dwarf::DW_ATE_unsigned_char);

	mDebugTypes["i8*"] = llvm::DIDerivedType::get(
	    context().llvmContext(), llvm::dwarf::DW_TAG_pointer_type, nullptr, nullptr, 0, nullptr,
	    charType, 64, 64, 0, llvm::DINode::DIFlags());  // TODO: 32bit support?
}

Result LangModule::nodeTypeFromName(boost::string_view name, const nlohmann::json& jsonData,
                                    std::unique_ptr<NodeType>* toFill) {
	Result res;

	auto iter = nodes.find(name.to_string());
	if (iter != nodes.end()) {
		*toFill = iter->second(jsonData, res);
		return res;
	}

	res.addEntry("E37", "Failed to find node in module",
	             {{"Module", "lang"}, {"Requested Node Type", name.to_string()}});

	return res;
}

// the lang module just has the basic llvm types.
DataType LangModule::typeFromName(boost::string_view name) {
	using namespace std::string_literals;

	llvm::Type* ty;

	auto err = llvm::SMDiagnostic();
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 8
	// just parse the type
	auto IR = "@G = external global "s + name.to_string();

	auto tmpModule = llvm::parseAssemblyString(IR, err, context().llvmContext());
	if (!tmpModule) { return nullptr; }

	ty = tmpModule->getNamedValue("G")->getType()->getContainedType(0);
#else
	{
		llvm::Module tMod("tmp", context().llvmContext());
		ty = llvm::parseType(name.to_string(), err, tMod, nullptr);
	}
#endif

	// get debug type
	auto iter = mDebugTypes.find(name.to_string());
	if (iter == mDebugTypes.end()) { return {}; }

	return DataType{this, name.to_string(), ty, iter->second};
}

Result LangModule::generateModule(llvm::Module&) { return {}; }
}  // namespace chi
