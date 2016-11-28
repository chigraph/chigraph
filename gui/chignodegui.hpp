#pragma once

#ifndef CHIG_GUI_CHIGNODEGUI_HPP
#define CHIG_GUI_CHIGNODEGUI_HPP

#include <nodes/NodeDataModel>
#include <nodes/DataModelRegistry>

#include <chig/ChigModule.hpp>
#include <chig/NodeType.hpp>

#include <memory>

class ChigNodeGui : public NodeDataModel {
	
public:
	
	ChigNodeGui(chig::ChigModule* module_, std::string type_, nlohmann::json j) : type{type_}, module{module_}, jsonData{j} {
		auto res = module->createNodeType(type.c_str(), j, &ty);
		if(!res) {
			std::cerr << "Failed to create stuff for module. Error: " << res.result_json.dump(2) << std::endl;
		}
	}
  
	std::string type;
	chig::ChigModule* module;
	std::unique_ptr<chig::NodeType> ty;
	nlohmann::json jsonData;
	
	QString caption() const override { 
		return QString::fromStdString(type); 
	}

	QString name() const override { 
		return QString::fromStdString(module->name + ":" + type);
	}
	
	std::unique_ptr<NodeDataModel> clone() const override {
		return std::unique_ptr<ChigNodeGui>(new ChigNodeGui(module, type, jsonData));
	}
	
	virtual unsigned int nPorts(PortType portType) const override {
		if(portType == PortType::In) {
			return ty->execInputs.size() + ty->dataInputs.size();
		} else if(portType == PortType::Out) {
			return ty->execOutputs.size() + ty->dataOutputs.size();
		}
		
		return 1; // ?
	}
	
	virtual NodeDataType dataType(PortType pType, PortIndex pIndex) const override {
		if(pType == PortType::In) {
			std::pair<std::string, std::string> idandname;
			if(pIndex >= int(ty->execInputs.size())) {
				
				if(pIndex - ty->execInputs.size() >= ty->dataInputs.size()) return {};
				
				idandname = {module->context->stringifyType(ty->dataInputs[pIndex - ty->execInputs.size()].first), 
					ty->dataInputs[pIndex - ty->execInputs.size()].second};
				
			} else {
				idandname = {"exec", ty->execInputs[pIndex]};
			}
			return {QString::fromStdString(idandname.first), QString::fromStdString(idandname.second)};
		} else if(pType == PortType::Out) {
			std::pair<std::string, std::string> idandname;
			if(pIndex >= int(ty->execOutputs.size())) {
				
				idandname = {module->context->stringifyType(ty->dataOutputs[pIndex - ty->execOutputs.size()].first), 
					ty->dataOutputs[pIndex - ty->execOutputs.size()].second};
				
			} else {
				idandname = {"exec", ty->execOutputs[pIndex]};
			}
			return {QString::fromStdString(idandname.first), QString::fromStdString(idandname.second)};

		}
		
		return {};
		
	}
	
	virtual void setInData(std::shared_ptr<NodeData>, PortIndex) override {}
	
	virtual std::shared_ptr<NodeData> outData(PortIndex port) override { return nullptr; }
    virtual QWidget* embeddedWidget() override { return nullptr; };
	
	// We don't need saving...chigraph has its own serialization
	void save(Properties&) const override {}
	
};



#endif // CHIG_GUI_CHIGNODEGUI_HPP
