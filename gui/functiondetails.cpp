#include "functiondetails.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <KLocalizedString>

FunctionDetails::FunctionDetails(QWidget* parent) : QWidget(parent)
{
	auto layout = new QVBoxLayout;
	setLayout(layout);

	ins = new ParamListWidget(i18n("Inputs"));
	layout->addWidget(ins);

	outs = new ParamListWidget(i18n("Outputs"));
	layout->addWidget(outs);
}

void FunctionDetails::loadFunction(chig::GraphFunction* func)
{
	mFunc = func;

	ins->clear();
	outs->clear();

	// disconnect connections so we don't get a ton of callbacks here
	disconnect(ins, &ParamListWidget::paramAdded, this, &FunctionDetails::inputAdded);
	disconnect(ins, &ParamListWidget::paramDeleted, this, &FunctionDetails::inputDeleted);
	disconnect(ins, &ParamListWidget::paramChanged, this, &FunctionDetails::inputChanged);
	
	disconnect(outs, &ParamListWidget::paramAdded, this, &FunctionDetails::outputAdded);
	disconnect(outs, &ParamListWidget::paramDeleted, this, &FunctionDetails::outputDeleted);
	disconnect(outs, &ParamListWidget::paramChanged, this, &FunctionDetails::outputChanged);
	
	ins->setModule(&func->module());
	outs->setModule(&func->module());

    for (auto in : func->dataInputs()) {
		ins->addParam(in.first, QString::fromStdString(in.second), ins->paramCount() - 1);
	}

    for (auto out : func->dataOutputs()) {
		outs->addParam(out.first, QString::fromStdString(out.second), ins->paramCount() - 1);
	}
    // TODO: exec
	
	// connect!
	
	connect(ins, &ParamListWidget::paramAdded, this, &FunctionDetails::inputAdded);
	connect(ins, &ParamListWidget::paramDeleted, this, &FunctionDetails::inputDeleted);
	connect(ins, &ParamListWidget::paramChanged, this, &FunctionDetails::inputChanged);
	
	connect(outs, &ParamListWidget::paramAdded, this, &FunctionDetails::outputAdded);
	connect(outs, &ParamListWidget::paramDeleted, this, &FunctionDetails::outputDeleted);
    connect(outs, &ParamListWidget::paramChanged, this, &FunctionDetails::outputChanged);
}

void FunctionDetails::inputChanged(int idx, chig::DataType newType, QString newName)
{

}

void FunctionDetails::inputAdded(chig::DataType type, QString name)
{

}

void FunctionDetails::inputDeleted(int idx)
{

}

void FunctionDetails::outputChanged(int idx, chig::DataType newType, QString newName)
{

}

void FunctionDetails::outputAdded(chig::DataType type, QString name)
{

}

void FunctionDetails::outputDeleted(int idx)
{

}
