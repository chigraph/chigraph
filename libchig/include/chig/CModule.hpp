#pragma once

#ifndef CHIG_CMODULE_HPP
#define CHIG_CMODULE_HPP

#include <unordered_map>
#include <functional>
#include <memory>

#include "chig/Fwd.hpp"
#include "chig/ChigModule.hpp"
#include "chig/NodeType.hpp"
#include "chig/json.hpp"

namespace chig {

struct CModule : ChigModule {
	CModule(Context& context);
	~CModule() = default;

	virtual std::unique_ptr<NodeType> createNodeType(
		const char* name, const nlohmann::json& json_data) const override;
	virtual llvm::Type* getType(const char* name) const override;

	std::unordered_map<std::string, std::function<std::unique_ptr<NodeType>(const nlohmann::json&)>>
		nodes;
	
};

struct CFuncNode : NodeType {
	
	CFuncNode(Context& con, const std::string& modulecode, const std::string& functocall);

	// the function doesn't have to do anything...this class just holds metadata
	virtual Result codegen(size_t /*inputExecID*/, llvm::Module* mod, llvm::Function* f,
		const std::vector<llvm::Value*>& io, llvm::BasicBlock* codegenInto,
		const std::vector<llvm::BasicBlock*>& outputBlocks) const override;

	virtual std::unique_ptr<NodeType> clone() const override;

	nlohmann::json toJSON() const override;

	std::string functocall;
	std::string ccode;
	
	std::unique_ptr<llvm::Module> llcompiledmod;
};

}

#endif // CHIG_CMODULE_HPP
