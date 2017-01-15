#include "functiondetails.hpp"
#include "functionview.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <KLocalizedString>

FunctionDetails::FunctionDetails(QWidget* parent) : QWidget(parent) {
	auto layout = new QVBoxLayout;
	setLayout(layout);

	ins = new ParamListWidget(i18n("Inputs"));
	layout->addWidget(ins);

	outs = new ParamListWidget(i18n("Outputs"));
	layout->addWidget(outs);
}

void FunctionDetails::loadFunction(FunctionView* func) {
	mFuncView = func;
	mFunc	 = func->function();

	ins->clear();
	outs->clear();

	// disconnect connections so we don't get a ton of callbacks here
	disconnect(ins, &ParamListWidget::paramAdded, this, &FunctionDetails::inputAdded);
	disconnect(ins, &ParamListWidget::paramDeleted, this, &FunctionDetails::inputDeleted);
	disconnect(ins, &ParamListWidget::paramChanged, this, &FunctionDetails::inputChanged);

	disconnect(outs, &ParamListWidget::paramAdded, this, &FunctionDetails::outputAdded);
	disconnect(outs, &ParamListWidget::paramDeleted, this, &FunctionDetails::outputDeleted);
	disconnect(outs, &ParamListWidget::paramChanged, this, &FunctionDetails::outputChanged);

	ins->setModule(&mFunc->module());
	outs->setModule(&mFunc->module());

	for (auto in : mFunc->dataInputs()) {
		ins->addParam(in.first, QString::fromStdString(in.second), ins->paramCount() - 1);
	}

	for (auto out : mFunc->dataOutputs()) {
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

void FunctionDetails::inputChanged(int idx, const chig::DataType& newType, const QString& newName) {
	mFunc->modifyDataInput(idx, newType, gsl::cstring_span<>(newName.toStdString()));

	auto entry = mFunc->entryNode();
	if (entry == nullptr) { return; }
	mFuncView->refreshGuiForNode(mFuncView->guiNodeFromChigNode(entry));
}
void FunctionDetails::inputAdded(const chig::DataType& type, const QString& name) {
	mFunc->addDataInput(type, name.toStdString(), mFunc->dataInputs().size() - 1);  // add to end

	auto entry = mFunc->entryNode();
	if (entry == nullptr) { return; }
	mFuncView->refreshGuiForNode(mFuncView->guiNodeFromChigNode(entry));
}
void FunctionDetails::inputDeleted(int idx) {
	mFunc->removeDataInput(idx);

	auto entry = mFunc->entryNode();
	if (entry == nullptr) { return; }
	mFuncView->refreshGuiForNode(mFuncView->guiNodeFromChigNode(entry));
}
void FunctionDetails::outputChanged(int idx, const chig::DataType& newType, const QString& newName) {
	mFunc->modifyDataOutput(idx, newType, gsl::cstring_span<>(newName.toStdString()));

	for (const auto& exit : mFunc->graph().nodesWithType("lang", "exit")) {
		mFuncView->refreshGuiForNode(mFuncView->guiNodeFromChigNode(exit));
	}
}
void FunctionDetails::outputAdded(const chig::DataType& type, const QString& name) {
	mFunc->addDataOutput(type, name.toStdString(), mFunc->dataInputs().size() - 1);

	for (const auto& exit : mFunc->graph().nodesWithType("lang", "exit")) {
		mFuncView->refreshGuiForNode(mFuncView->guiNodeFromChigNode(exit));
	}
}
void FunctionDetails::outputDeleted(int idx) {
	mFunc->removeDataOutput(idx);
	for (const auto& exit : mFunc->graph().nodesWithType("lang", "exit")) {
		mFuncView->refreshGuiForNode(mFuncView->guiNodeFromChigNode(exit));
	}
}
