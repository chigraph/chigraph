#include "functiondetails.hpp"
#include "functionview.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <KLocalizedString>

FunctionDetails::FunctionDetails(QWidget* parent) : QWidget(parent) {
	auto layout = new QVBoxLayout;
	setLayout(layout);

	execins = new ExecParamListWidget(i18n("Exec Inputs"));
	layout->addWidget(execins);
	
	execouts = new ExecParamListWidget(i18n("Exec Outputs"));
	layout->addWidget(execouts);
	
	ins = new ParamListWidget(i18n("Data Inputs"));
	layout->addWidget(ins);

	outs = new ParamListWidget(i18n("Data Outputs"));
	layout->addWidget(outs);
	
}

void FunctionDetails::loadFunction(FunctionView* func) {
	mFuncView = func;
	mFunc     = func->function();

	ins->clear();
	outs->clear();
	execins->clear();
	execouts->clear();
	

	// disconnect connections so we don't get a ton of callbacks here
	disconnect(ins, &ParamListWidget::paramAdded, this, &FunctionDetails::inputAdded);
	disconnect(ins, &ParamListWidget::paramDeleted, this, &FunctionDetails::inputDeleted);
	disconnect(ins, &ParamListWidget::paramChanged, this, &FunctionDetails::inputChanged);

	disconnect(outs, &ParamListWidget::paramAdded, this, &FunctionDetails::outputAdded);
	disconnect(outs, &ParamListWidget::paramDeleted, this, &FunctionDetails::outputDeleted);
	disconnect(outs, &ParamListWidget::paramChanged, this, &FunctionDetails::outputChanged);
	
	disconnect(execins, &ExecParamListWidget::execAdded, this, &FunctionDetails::execInputAdded);
	disconnect(execins, &ExecParamListWidget::execDeleted, this, &FunctionDetails::execInputDeleted);
	disconnect(execins, &ExecParamListWidget::execChanged, this, &FunctionDetails::execInputChanged);

	disconnect(execouts, &ExecParamListWidget::execAdded, this, &FunctionDetails::execOutputAdded);
	disconnect(execouts, &ExecParamListWidget::execDeleted, this, &FunctionDetails::execOutputDeleted);
	disconnect(execouts, &ExecParamListWidget::execChanged, this, &FunctionDetails::execOutputChanged);

	ins->setModule(&mFunc->module());
	outs->setModule(&mFunc->module());
	execins->setModule(&mFunc->module());
	execouts->setModule(&mFunc->module());

	for (auto in : mFunc->dataInputs()) {
		ins->addParam(in.first, QString::fromStdString(in.second), ins->paramCount() - 1);
	}

	for (auto out : mFunc->dataOutputs()) {
		outs->addParam(out.first, QString::fromStdString(out.second), ins->paramCount() - 1);
	}
	
	for (const auto& in : mFunc->execInputs()) {
		execins->addExec(QString::fromStdString(in), execins->execCount() - 1);
	}
	
	for (const auto& out : mFunc->execOutputs()) {
		execouts->addExec(QString::fromStdString(out), execouts->execCount() - 1);
	}

	// connect!

	connect(ins, &ParamListWidget::paramAdded, this, &FunctionDetails::inputAdded);
	connect(ins, &ParamListWidget::paramDeleted, this, &FunctionDetails::inputDeleted);
	connect(ins, &ParamListWidget::paramChanged, this, &FunctionDetails::inputChanged);

	connect(outs, &ParamListWidget::paramAdded, this, &FunctionDetails::outputAdded);
	connect(outs, &ParamListWidget::paramDeleted, this, &FunctionDetails::outputDeleted);
	connect(outs, &ParamListWidget::paramChanged, this, &FunctionDetails::outputChanged);
	
	
	connect(execins, &ExecParamListWidget::execAdded, this, &FunctionDetails::execInputAdded);
	connect(execins, &ExecParamListWidget::execDeleted, this, &FunctionDetails::execInputDeleted);
	connect(execins, &ExecParamListWidget::execChanged, this, &FunctionDetails::execInputChanged);

	connect(execouts, &ExecParamListWidget::execAdded, this, &FunctionDetails::execOutputAdded);
	connect(execouts, &ExecParamListWidget::execDeleted, this, &FunctionDetails::execOutputDeleted);
	connect(execouts, &ExecParamListWidget::execChanged, this, &FunctionDetails::execOutputChanged);
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
void FunctionDetails::outputChanged(int idx, const chig::DataType& newType,
                                    const QString& newName) {
	mFunc->modifyDataOutput(idx, newType, gsl::cstring_span<>(newName.toStdString()));

	for (const auto& exit : mFunc->nodesWithType("lang", "exit")) {
		mFuncView->refreshGuiForNode(mFuncView->guiNodeFromChigNode(exit));
	}
	mFuncView->refreshRegistry();
}
void FunctionDetails::outputAdded(const chig::DataType& type, const QString& name) {
	mFunc->addDataOutput(type, name.toStdString(), mFunc->dataInputs().size() - 1);

	for (const auto& exit : mFunc->nodesWithType("lang", "exit")) {
		mFuncView->refreshGuiForNode(mFuncView->guiNodeFromChigNode(exit));
	}
	mFuncView->refreshRegistry();
}
void FunctionDetails::outputDeleted(int idx) {
	mFunc->removeDataOutput(idx);
	for (const auto& exit : mFunc->nodesWithType("lang", "exit")) {
		mFuncView->refreshGuiForNode(mFuncView->guiNodeFromChigNode(exit));
	}
	mFuncView->refreshRegistry();
}


void FunctionDetails::execInputChanged(int idx, const QString& newName) {
	mFunc->modifyExecInput(idx, newName.toStdString());
	
	auto entry = mFunc->entryNode();
	if (entry == nullptr) { return; }
	mFuncView->refreshGuiForNode(mFuncView->guiNodeFromChigNode(entry));
}
void FunctionDetails::execInputAdded(const QString& name) {
	mFunc->addExecInput(name.toStdString(), mFunc->dataInputs().size() - 1);
	
	auto entry = mFunc->entryNode();
	if (entry == nullptr) { return; }
	mFuncView->refreshGuiForNode(mFuncView->guiNodeFromChigNode(entry));
}
void FunctionDetails::execInputDeleted(int idx) {
	mFunc->removeExecInput(idx);
	
	auto entry = mFunc->entryNode();
	if (entry == nullptr) { return; }
	mFuncView->refreshGuiForNode(mFuncView->guiNodeFromChigNode(entry));

}

void FunctionDetails::execOutputChanged(int idx, const QString& newName) {
	mFunc->modifyExecOutput(idx, newName.toStdString());
	
	for (const auto& exit : mFunc->nodesWithType("lang", "exit")) {
		mFuncView->refreshGuiForNode(mFuncView->guiNodeFromChigNode(exit));
	}
	mFuncView->refreshRegistry();
}
void FunctionDetails::execOutputAdded(const QString& name) {
	mFunc->addExecOutput(name.toStdString(), mFunc->dataInputs().size() - 1);
	
	for (const auto& exit : mFunc->nodesWithType("lang", "exit")) {
		mFuncView->refreshGuiForNode(mFuncView->guiNodeFromChigNode(exit));
	}
	
	mFuncView->refreshRegistry();
}
void FunctionDetails::execOutputDeleted(int idx) {
	mFunc->removeExecOutput(idx);

		
	for (const auto& exit : mFunc->nodesWithType("lang", "exit")) {
		mFuncView->refreshGuiForNode(mFuncView->guiNodeFromChigNode(exit));
	}
	mFuncView->refreshRegistry();
}

