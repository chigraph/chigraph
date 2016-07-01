#ifndef CHIG_CONTEXT_HPP
#define CHIG_CONTEXT_HPP

#pragma once

#include "chig/json.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <vector>
#include <string>

namespace chig {

struct context {

	context() {}

	llvm::Module* compile_json(const nlohmann::json& file);

	void load_module(const std::string& name);

	void add_search_path(std::string new_path) {
		m_search_paths.emplace_back(std::move(new_path));
	}

	llvm::Module* get_module_by_name(const std::string& name);

	llvm::Function* get_function_by_name(const std::string& name);
	llvm::Type* get_type_by_name(const std::string& name);

private:

	std::vector<std::unique_ptr<llvm::Module>> m_modules;
	std::vector<std::string> m_search_paths;

	llvm::LLVMContext m_context;
};

}

#endif // CHIG_CONTEXT_HPP
