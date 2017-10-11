/// \file NodeType.cpp

#include "chi/NodeType.hpp"
#include "chi/ChiModule.hpp"
#include "chi/DataType.hpp"

namespace chi {

NodeType::NodeType(ChiModule& mod, std::string name, std::string description)
    : mModule{&mod},
      mContext{&mod.context()},
      mName{std::move(name)},
      mDescription{std::move(description)} {}

NodeType::~NodeType() = default;

std::string NodeType::qualifiedName() const { return module().fullName() + ":" + name(); }

void NodeType::setDataInputs(
    std::vector<chi::NamedDataType, std::allocator<chi::NamedDataType> > newInputs) {
	mDataInputs = std::move(newInputs);
}

void NodeType::setDataOutputs(
    std::vector<chi::NamedDataType, std::allocator<chi::NamedDataType> > newOutputs) {
	mDataOutputs = std::move(newOutputs);
}

void NodeType::setExecInputs(std::vector<std::string> newInputs) {
	mExecInputs = std::move(newInputs);
}

void NodeType::setExecOutputs(std::vector<std::string> newOutputs) {
	mExecOutputs = std::move(newOutputs);
}

void NodeType::makePure() {
	setExecInputs({});
	setExecOutputs({});

	mPure = true;
}

void NodeType::makeConverter() {
	assert(pure() && "Cannot have a nonpure converter node");
	assert(dataInputs().size() == 1 && "A converter node must have one data input");
	assert(dataOutputs().size() == 1 && "A converter node must have one data output");
	
	mConverter = true;
}

NodeInstance* NodeType::nodeInstance() const { return mNodeInstance; }

void NodeType::setName(std::string newName) { mName = std::move(newName); }

void NodeType::setDescription(std::string newDesc) { mDescription = std::move(newDesc); }
}  // namespace chi
