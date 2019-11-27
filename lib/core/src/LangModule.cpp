/// \file LangModule.cpp

#include "chi/LangModule.hpp"

#include <llvm-c/Core.h>
#include <llvm-c/DebugInfo.h>

#include "chi/Context.hpp"
#include "chi/DataType.hpp"
#include "chi/Dwarf.hpp"
#include "chi/FunctionCompiler.hpp"
#include "chi/NodeType.hpp"
#include "chi/Support/Result.hpp"

using namespace std::string_literals;

namespace chi {

namespace {
/// NodeType for conditionals
struct IfNodeType : NodeType {
	IfNodeType(LangModule& mod) : NodeType(mod, "if", "If") {
		setExecInputs({""});
		setExecOutputs({"True", "False"});

		setDataInputs({{"condition", mod.typeFromName("i1")}});
	}

	Result codegen(NodeCompiler& compiler, LLVMBasicBlockRef codegenInto, size_t execInputID,
	               LLVMMetadataRef nodeLocation, const std::vector<LLVMValueRef>& io,
	               const std::vector<LLVMBasicBlockRef>& outputBlocks) override {
		assert(io.size() == 1 && outputBlocks.size() == 2);

		auto builder = OwnedLLVMBuilder(LLVMCreateBuilder());
		LLVMPositionBuilder(*builder, codegenInto, nullptr);

		LLVMSetCurrentDebugLocation(*builder,
		                            LLVMMetadataAsValue(context().llvmContext(), nodeLocation));

		LLVMBuildCondBr(*builder, io[0], outputBlocks[0], outputBlocks[1]);

		return {};
	}

	std::unique_ptr<NodeType> clone() const override { return std::make_unique<IfNodeType>(*this); }
};

struct EntryNodeType : NodeType {
	EntryNodeType(LangModule& mod, std::vector<NamedDataType> dataInputs,
	              std::vector<std::string> execInputs)
	    : NodeType(mod, "entry", "Entry") {
		setExecOutputs(std::move(execInputs));

		setDataOutputs(std::move(dataInputs));
	}

	Result codegen(NodeCompiler& compiler, LLVMBasicBlockRef codegenInto, size_t execInputID,
	               LLVMMetadataRef nodeLocation, const std::vector<LLVMValueRef>& io,
	               const std::vector<LLVMBasicBlockRef>& outputBlocks) override {
		assert(io.size() == dataOutputs().size() && outputBlocks.size() == execOutputs().size());

		auto builder = OwnedLLVMBuilder(LLVMCreateBuilder());
		LLVMPositionBuilder(*builder, codegenInto, nullptr);
		LLVMSetCurrentDebugLocation(*builder,
		                            LLVMMetadataAsValue(context().llvmContext(), nodeLocation));

		// store the arguments
		auto function = LLVMGetBasicBlockParent(codegenInto);

		auto current_arg = LLVMGetParam(function, 1);
		for (const auto& iovalue : io) {
			LLVMBuildStore(*builder, current_arg, iovalue);

			current_arg = LLVMGetNextParam(current_arg);
		}

		auto inExecID   = LLVMGetFirstParam(function);
		auto switchInst = LLVMBuildSwitch(*builder, inExecID, outputBlocks[0], outputBlocks.size());

		for (auto id = 0ull; id < execOutputs().size(); ++id) {
			LLVMAddCase(switchInst, context().constI32(id), outputBlocks[id]);
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
	    : NodeType(mod, "const-int", "Integer"), number(num) {
		makePure();

		setDataOutputs({{"", mod.typeFromName("i32")}});
	}

	Result codegen(NodeCompiler& compiler, LLVMBasicBlockRef codegenInto, size_t execInputID,
	               LLVMMetadataRef nodeLocation, const std::vector<LLVMValueRef>& io,
	               const std::vector<LLVMBasicBlockRef>& outputBlocks) override {
		assert(io.size() == 1 && outputBlocks.size() == 1);

		auto builder = OwnedLLVMBuilder(LLVMCreateBuilder());
		LLVMPositionBuilder(*builder, codegenInto, nullptr);
		LLVMSetCurrentDebugLocation(*builder,
		                            LLVMMetadataAsValue(context().llvmContext(), nodeLocation));

		LLVMBuildStore(*builder, context().constI32(number), io[0]);
		LLVMBuildBr(*builder, outputBlocks[0]);

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
	    : NodeType(mod, "const-float", "Float"), number(num) {
		makePure();

		setDataOutputs({{"", mod.typeFromName("float")}});
	}

	Result codegen(NodeCompiler& compiler, LLVMBasicBlockRef codegenInto, size_t execInputID,
	               LLVMMetadataRef nodeLocation, const std::vector<LLVMValueRef>& io,
	               const std::vector<LLVMBasicBlockRef>& outputBlocks) override {
		assert(io.size() == 1 && outputBlocks.size() == 1);

		auto builder = OwnedLLVMBuilder(LLVMCreateBuilder());
		LLVMPositionBuilder(*builder, codegenInto, nullptr);
		LLVMSetCurrentDebugLocation(*builder,
		                            LLVMMetadataAsValue(context().llvmContext(), nodeLocation));

		LLVMBuildStore(*builder, context().constF64(number), io[0]);
		LLVMBuildBr(*builder, outputBlocks[0]);

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

	Result codegen(NodeCompiler& compiler, LLVMBasicBlockRef codegenInto, size_t execInputID,
	               LLVMMetadataRef nodeLocation, const std::vector<LLVMValueRef>& io,
	               const std::vector<LLVMBasicBlockRef>& outputBlocks) override {
		assert(io.size() == 1 && outputBlocks.size() == 1);

		auto builder = OwnedLLVMBuilder(LLVMCreateBuilder());
		LLVMPositionBuilder(*builder, codegenInto, nullptr);
		LLVMSetCurrentDebugLocation(*builder,
		                            LLVMMetadataAsValue(context().llvmContext(), nodeLocation));

		LLVMBuildStore(*builder, context().constBool(value), io[0]);
		LLVMBuildBr(*builder, outputBlocks[0]);

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

	Result codegen(NodeCompiler& compiler, LLVMBasicBlockRef codegenInto, size_t execInputID,
	               LLVMMetadataRef nodeLocation, const std::vector<LLVMValueRef>& io,
	               const std::vector<LLVMBasicBlockRef>& outputBlocks) override {
		assert(execInputID < execInputs().size() && io.size() == dataInputs().size());

		// assign the return types
		auto builder = OwnedLLVMBuilder(LLVMCreateBuilder());
		LLVMPositionBuilder(*builder, codegenInto, nullptr);
		LLVMSetCurrentDebugLocation(*builder,
		                            LLVMMetadataAsValue(context().llvmContext(), nodeLocation));

		auto   f = LLVMGetBasicBlockParent(codegenInto);
		size_t ret_start =
		    LLVMCountParams(f) - io.size();  // returns are after args, find where returns start
		auto current_arg = LLVMGetParam(f, ret_start);
		for (auto& value : io) {
			LLVMBuildStore(*builder, value, current_arg);

			current_arg = LLVMGetNextParam(current_arg);
		}

		LLVMBuildRet(*builder, context().constI32(execInputID));

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
	    : NodeType(mod, "strliteral", "String"), literalString(std::move(str)) {
		makePure();

		setDataOutputs({{"", mod.typeFromName("i8*")}});
	}

	Result codegen(NodeCompiler& compiler, LLVMBasicBlockRef codegenInto, size_t execInputID,
	               LLVMMetadataRef nodeLocation, const std::vector<LLVMValueRef>& io,
	               const std::vector<LLVMBasicBlockRef>& outputBlocks) override {
		assert(io.size() == 1 && outputBlocks.size() == 1);

		auto builder = OwnedLLVMBuilder(LLVMCreateBuilder());
		LLVMPositionBuilder(*builder, codegenInto, nullptr);
		LLVMSetCurrentDebugLocation(*builder,
		                            LLVMMetadataAsValue(context().llvmContext(), nodeLocation));

		auto global = LLVMBuildGlobalStringPtr(*builder, literalString.c_str(), "");

		LLVMBuildStore(*builder, global, io[0]);

		LLVMBuildBr(*builder, outputBlocks[0]);

		return {};
	}

	std::unique_ptr<NodeType> clone() const override {
		return std::make_unique<StringLiteralNodeType>(*this);
	}

	nlohmann::json toJSON() const override { return literalString; }
	std::string    literalString;
};

struct IntToFloatNodeType : NodeType {
	IntToFloatNodeType(LangModule& mod) : NodeType(mod, "inttofloat", "Float -> Integer") {
		makePure();

		setDataInputs({{"", mod.typeFromName("i32")}});
		setDataOutputs({{"", mod.typeFromName("float")}});

		makeConverter();
	}

	Result codegen(NodeCompiler& /*compiler*/, LLVMBasicBlockRef codegenInto,
	               size_t /*execInputID*/, LLVMMetadataRef       nodeLocation,
	               const std::vector<LLVMValueRef>&      io,
	               const std::vector<LLVMBasicBlockRef>& outputBlocks) override {
		assert(io.size() == 2 && outputBlocks.size() == 1);

		auto builder = OwnedLLVMBuilder(LLVMCreateBuilder());
		LLVMPositionBuilder(*builder, codegenInto, nullptr);
		LLVMSetCurrentDebugLocation(*builder,
		                            LLVMMetadataAsValue(context().llvmContext(), nodeLocation));

		auto casted = LLVMBuildCast(*builder, LLVMSIToFP, io[0],
		                            LLVMDoubleTypeInContext(context().llvmContext()), "");
		LLVMBuildStore(*builder, casted, io[1]);

		LLVMBuildBr(*builder, outputBlocks[0]);

		return {};
	}

	std::unique_ptr<NodeType> clone() const override {
		return std::make_unique<IntToFloatNodeType>(*this);
	}
};

struct FloatToIntNodeType : NodeType {
	FloatToIntNodeType(LangModule& mod) : NodeType(mod, "floattoint", "Float -> Integer") {
		makePure();

		setDataInputs({{"", mod.typeFromName("float")}});
		setDataOutputs({{"", mod.typeFromName("i32")}});

		makeConverter();
	}

	Result codegen(NodeCompiler& compiler, LLVMBasicBlockRef codegenInto, size_t execInputID,
	               LLVMMetadataRef nodeLocation, const std::vector<LLVMValueRef>& io,
	               const std::vector<LLVMBasicBlockRef>& outputBlocks) override {
		assert(io.size() == 2 && outputBlocks.size() == 1);

		auto builder = OwnedLLVMBuilder(LLVMCreateBuilder());
		LLVMPositionBuilder(*builder, codegenInto, nullptr);
		LLVMSetCurrentDebugLocation(*builder,
		                            LLVMMetadataAsValue(context().llvmContext(), nodeLocation));

		auto casted = LLVMBuildCast(*builder, LLVMFPToSI, io[0],
		                            LLVMInt32TypeInContext(context().llvmContext()), "");
		LLVMBuildStore(*builder, casted, io[1]);

		LLVMBuildBr(*builder, outputBlocks[0]);

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

	Result codegen(NodeCompiler& /*compiler*/, LLVMBasicBlockRef codegenInto,
	               size_t /*execInputID*/, LLVMMetadataRef       nodeLocation,
	               const std::vector<LLVMValueRef>&      io,
	               const std::vector<LLVMBasicBlockRef>& outputBlocks) override {
		assert(io.size() == 3 && outputBlocks.size() == 1);

		auto builder = OwnedLLVMBuilder(LLVMCreateBuilder());
		LLVMPositionBuilder(*builder, codegenInto, nullptr);
		LLVMSetCurrentDebugLocation(*builder,
		                            LLVMMetadataAsValue(context().llvmContext(), nodeLocation));

		LLVMValueRef result = nullptr;
		if (mType.unqualifiedName() == "i32") {
			result = [&](BinOp b) {
				switch (b) {
				case BinOp::Add: return LLVMBuildAdd(*builder, io[0], io[1], "");
				case BinOp::Subtract: return LLVMBuildSub(*builder, io[0], io[1], "");
				case BinOp::Multiply: return LLVMBuildMul(*builder, io[0], io[1], "");
				case BinOp::Divide: return LLVMBuildSDiv(*builder, io[0], io[1], "");
				default: return (LLVMValueRef) nullptr;
				}
				return (LLVMValueRef) nullptr;
			}(mBinOp);

		} else {
			result = [&](BinOp b) {
				switch (b) {
				case BinOp::Add: return LLVMBuildFAdd(*builder, io[0], io[1], "");
				case BinOp::Subtract: return LLVMBuildFSub(*builder, io[0], io[1], "");
				case BinOp::Multiply: return LLVMBuildFMul(*builder, io[0], io[1], "");
				case BinOp::Divide: return LLVMBuildFDiv(*builder, io[0], io[1], "");
				default: return (LLVMValueRef) nullptr;
				}
				return (LLVMValueRef) nullptr;
			}(mBinOp);
		}

		LLVMBuildStore(*builder, result, io[2]);

		LLVMBuildBr(*builder, outputBlocks[0]);

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

	Result codegen(NodeCompiler& /*compiler*/, LLVMBasicBlockRef codegenInto,
	               size_t /*execInputID*/, LLVMMetadataRef       nodeLocation,
	               const std::vector<LLVMValueRef>&      io,
	               const std::vector<LLVMBasicBlockRef>& outputBlocks) override {
		assert(io.size() == 3 && outputBlocks.size() == 1);

		auto builder = OwnedLLVMBuilder(LLVMCreateBuilder());
		LLVMPositionBuilder(*builder, codegenInto, nullptr);
		LLVMSetCurrentDebugLocation(*builder,
		                            LLVMMetadataAsValue(context().llvmContext(), nodeLocation));

		LLVMValueRef result = nullptr;
		if (mType.unqualifiedName() == "i32") {
			LLVMIntPredicate pred;
			switch (mCompOp) {
			case CmpOp::Lt: pred = LLVMIntSLT; break;
			case CmpOp::Gt: pred = LLVMIntSGT; break;
			case CmpOp::Let: pred = LLVMIntSLE; break;
			case CmpOp::Get: pred = LLVMIntSGE; break;
			case CmpOp::Eq: pred = LLVMIntEQ; break;
			case CmpOp::Neq: pred = LLVMIntNE; break;
			default: assert(false); return {};
			}
			result = LLVMBuildICmp(*builder, pred, io[0], io[1], "");

		} else {
			LLVMRealPredicate pred;
			switch (mCompOp) {
			case CmpOp::Lt: pred = LLVMRealOLT; break;
			case CmpOp::Gt: pred = LLVMRealOGT; break;
			case CmpOp::Let: pred = LLVMRealOLE; break;
			case CmpOp::Get: pred = LLVMRealOGE; break;
			case CmpOp::Eq: pred = LLVMRealOEQ; break;
			case CmpOp::Neq: pred = LLVMRealONE; break;
			default: assert(false); return {};
			}
			result = LLVMBuildFCmp(*builder, pred, io[0], io[1], "");
		}

		LLVMBuildStore(*builder, result, io[2]);

		LLVMBuildBr(*builder, outputBlocks[0]);

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
			                  {{"Data JSON", injson}});
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
			                  {{"Data JSON", injson}});
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
}

Result LangModule::nodeTypeFromName(std::string_view name, const nlohmann::json& jsonData,
                                    std::unique_ptr<NodeType>* toFill) {
	Result res;

	auto iter = nodes.find(std::string(name));
	if (iter != nodes.end()) {
		*toFill = iter->second(jsonData, res);
		return res;
	}

	res.addEntry("E37", "Failed to find node in module",
	             {{"Module", "lang"}, {"Requested Node Type", name}});

	return res;
}

LangModule::~LangModule() = default;

// the lang module just has the basic llvm types.
DataType LangModule::typeFromName(std::string_view name) {
	LLVMTypeRef ty = nullptr;
	if (name == "i32") {
		ty = LLVMInt32TypeInContext(context().llvmContext());
	} else if (name == "i1") {
		ty = LLVMInt1TypeInContext(context().llvmContext());
	} else if (name == "float") {
		ty = LLVMDoubleTypeInContext(context().llvmContext());
	} else if (name == "i8*") {
		ty = LLVMPointerType(LLVMInt8TypeInContext(context().llvmContext()), 0);
	} else {
		return {};
	}

	return DataType{this, std::string(name), ty};
}  // namespace chi

LLVMMetadataRef LangModule::debugType(FunctionCompiler& compiler, const DataType& dType) const {
	auto getDebugType = [&](const char* name, size_t size, DwarfEncoding encoding) {
		auto full_name = "lang:"s + name;
		return LLVMDIBuilderCreateBasicType(compiler.diBuilder(), full_name.c_str(),
		                                    full_name.size(), size, (LLVMDWARFTypeEncoding)encoding,
		                                    LLVMDIFlagZero);
	};

	if (dType.unqualifiedName() == "i32") {
		return getDebugType("i32", 32, DwarfEncoding::Signed);
	} else if (dType.unqualifiedName() == "i1") {
		return getDebugType("i1", 8, DwarfEncoding::Boolean);
	} else if (dType.unqualifiedName() == "float") {
		return getDebugType("float", 64, DwarfEncoding::Float);
	} else if (dType.unqualifiedName() == "i8*") {
		auto charType = LLVMDIBuilderCreateBasicType(
		    compiler.diBuilder(), "lang:i8", strlen("lang:i8"), 8,
		    (LLVMDWARFTypeEncoding)DwarfEncoding::SignedChar, LLVMDIFlagZero);
		return LLVMDIBuilderCreatePointerType(compiler.diBuilder(), charType, 64, 0, 0, "lang:i8*",
		                                      strlen("lang:i8*"));
	}
	return nullptr;
}

Result LangModule::addForwardDeclarations(LLVMModuleRef) const { return {}; }

Result LangModule::generateModule(LLVMModuleRef) { return {}; }
}  // namespace chi
