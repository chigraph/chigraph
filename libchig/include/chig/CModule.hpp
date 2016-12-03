#pragma once

#ifndef CHIG_CMODULE_HPP
#define CHIG_CMODULE_HPP

#include <functional>
#include <memory>
#include <unordered_map>

#include "chig/ChigModule.hpp"
#include "chig/Fwd.hpp"
#include "chig/NodeType.hpp"
#include "chig/json.hpp"

namespace chig
{
struct CModule : ChigModule {
	CModule(Context& ctx);
	~CModule() = default;

	virtual Result createNodeType(gsl::cstring_span<> typeName, const nlohmann::json& json_data,
		std::unique_ptr<NodeType>* toFill) const override;
	virtual llvm::Type* getType(gsl::cstring_span<> typeName) const override;

	virtual std::vector<std::string> getNodeTypeNames() const override { return {"func"}; };
	virtual std::vector<std::string> getTypeNames() const override { return {}; };
	std::unordered_map<std::string, std::function<std::unique_ptr<NodeType>(const nlohmann::json&)>>
		nodes;
};

struct CFuncNode : NodeType {
	CFuncNode(
		Context& con, gsl::cstring_span<> cCode, gsl::cstring_span<> functionName, Result& res);

	// the function doesn't have to do anything...this class just holds metadata
	virtual Result codegen(size_t /*inputExecID*/, llvm::Module* mod, llvm::Function* f,
		const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
		const gsl::span<llvm::BasicBlock*> outputBlocks) const override;

	virtual std::unique_ptr<NodeType> clone() const override;

	nlohmann::json toJSON() const override;

	std::string functocall;
	std::string ccode;

	std::unique_ptr<llvm::Module> llcompiledmod;
};
}

#endif  // CHIG_CMODULE_HPP
