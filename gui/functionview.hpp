#pragma once

#ifndef CHIGGUI_FUNCTIONVIEW_HPP
#define CHIGGUI_FUNCTIONVIEW_HPP

#include <nodes/FlowScene>
#include <nodes/FlowView>

#include <chig/GraphFunction.hpp>

class FunctionView : public QWidget {
	
	Q_OBJECT
public:
	
	FlowScene* scene;
	FlowView* view;
	
	FunctionView(chig::GraphFunction* func_, std::shared_ptr< DataModelRegistry > reg, QWidget* parent = nullptr);
	
	chig::GraphFunction* func;
	
};

#endif // CHIGGUI_FUNCTIONVIEW_HPP
