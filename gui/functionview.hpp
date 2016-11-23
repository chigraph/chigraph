#pragma once

#ifndef CHIGGUI_FUNCTIONVIEW_HPP
#define CHIGGUI_FUNCTIONVIEW_HPP

#include <nodes/FlowScene>
#include <nodes/FlowView>

#include <chig/GraphFunction.hpp>
#include <chig/JsonModule.hpp>
#include <chig/NodeInstance.hpp>

#include <unordered_map>
#include <memory>

class FunctionView : public QWidget {
	
	Q_OBJECT
public:
	
	FlowScene* scene;
	FlowView* view;
	
	FunctionView(chig::JsonModule* module, chig::GraphFunction* func_, std::shared_ptr< DataModelRegistry > reg, QWidget* parent = nullptr);
	
	chig::GraphFunction* func;
	
	std::unordered_map<chig::NodeInstance*, std::weak_ptr<Node>> assoc;
	
};

#endif // CHIGGUI_FUNCTIONVIEW_HPP
