#include "functionview.hpp"

#include <QHBoxLayout>

FunctionView::FunctionView(chig::GraphFunction* func_, std::shared_ptr<DataModelRegistry> reg, QWidget* parent) : QWidget(parent), func{func_} {
	
	QHBoxLayout* hlayout = new QHBoxLayout(this);
	
	hlayout->setMargin(0);
	hlayout->setSpacing(0);
	
	scene = new FlowScene(reg);
	view = new FlowView(scene);
	
	hlayout->addWidget(view);
	
}


