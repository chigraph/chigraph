#pragma once

#ifndef CHIG_GUI_CHIGNODEGUI_HPP
#define CHIG_GUI_CHIGNODEGUI_HPP

#include <nodes/NodeDataModel>
#include <nodes/DataModelRegistry>

#include <chig/ChigModule.hpp>

class ChigNodeGui : NodeDataModel {
	
public:
	
	ChigNodeGui(chig::ChigModule* module_, QString type_) : type{type_}, module{module_} { }
  
	QString type;
	chig::ChigModule* module;
	
	QString caption() const override { 
		return QString("Text Source"); 
	}

	bool captionVisible() const override { return true; }

	QString name() { 
		return QString::fromStdString(module->name) + ":" + type;
	}
	
	std::unique_ptr<NodeDataModel> clone() {
		return std::make_unique<ChigNodeGui>(type, module);
	}
	
};

inline void registerModule(chig::ChigModule* module) {

	auto nodetypes = module->getNodeTypeNames();
	
	for(auto& nodetype : nodetypes) {
		
		DataModelRegistry::registerModel<ChigNodeGui>();
	}
	
	
	
}



#endif // CHIG_GUI_CHIGNODEGUI_HPP
