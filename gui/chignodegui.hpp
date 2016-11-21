#pragma once

#ifndef CHIG_GUI_CHIGNODEGUI_HPP
#define CHIG_GUI_CHIGNODEGUI_HPP

#include <nodes/NodeDataModel>
#include <nodes/DataModelRegistry>

#include <chig/ChigModule.hpp>

extern const char* typetocreate;
extern chig::ChigModule* chigmodule;

class ChigNodeGui : NodeDataModel {
	
public:
	
	QString type;
	chig::ChigModule* module;
	
	ChigNodeGui() {
		type = typetocreate;
		module = chigmodule;
	}
	
	QString caption() const override { 
		return QString("Text Source"); 
	}

	bool captionVisible() const override { return true; }

	static QString name() { 
		return typetocreate;
		
	}
	
};

inline void registerModule(chig::ChigModule* module) {

	chigmodule = module;

	auto nodetypes = module->getNodeTypeNames();
	
	for(auto& nodetype : nodetypes) {
		typetocreate = nodetype.c_str();
		
		DataModelRegistry::registerModel<ChigNodeGui>();
	}
	
	
	
}



#endif // CHIG_GUI_CHIGNODEGUI_HPP
