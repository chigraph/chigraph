#include "functiondetails.hpp"
#include "functionview.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include <KLocalizedString>

FunctionDetails::FunctionDetails(QWidget* parent) : QScrollArea(parent) {
	
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setWidgetResizable(false);
	
	auto layout = new QVBoxLayout;
	setLayout(layout);

// 	execins = new ExecParamListWidget(i18n("Exec Inputs"));
// 	layout->addWidget(execins);
// 	
// 	execouts = new ExecParamListWidget(i18n("Exec Outputs"));
// 	layout->addWidget(execouts);
// 	
	ins = new ParamListWidget(/*i18n("Data Inputs")*/);
	layout->addWidget(ins);

	outs = new ParamListWidget(/*i18n("Data Outputs")*/);
	layout->addWidget(outs);
	
}

void FunctionDetails::loadFunction(FunctionView* func) {
	mFuncView = func;
	mFunc     = func->function();

// 	execins->clear();
// 	execouts->clear();
	
	ins->setFunction(func, ParamListWidget::Input);
	outs->setFunction(func, ParamListWidget::Output);

// 	// disconnect connections so we don't get a ton of callbacks here
// 	disconnect(execins, &ExecParamListWidget::execAdded, this, &FunctionDetails::execInputAdded);
// 	disconnect(execins, &ExecParamListWidget::execDeleted, this, &FunctionDetails::execInputDeleted);
// 	disconnect(execins, &ExecParamListWidget::execChanged, this, &FunctionDetails::execInputChanged);
// 
// 	disconnect(execouts, &ExecParamListWidget::execAdded, this, &FunctionDetails::execOutputAdded);
// 	disconnect(execouts, &ExecParamListWidget::execDeleted, this, &FunctionDetails::execOutputDeleted);
// 	disconnect(execouts, &ExecParamListWidget::execChanged, this, &FunctionDetails::execOutputChanged);
// 
// 	execins->setModule(&mFunc->module());
// 	execouts->setModule(&mFunc->module());
// 
// 	for (const auto& in : mFunc->execInputs()) {
// 		execins->addExec(QString::fromStdString(in), execins->execCount() - 1);
// 	}
// 	
// 	for (const auto& out : mFunc->execOutputs()) {
// 		execouts->addExec(QString::fromStdString(out), execouts->execCount() - 1);
// 	}
// 
// 	// connect!
// 	
// 	connect(execins, &ExecParamListWidget::execAdded, this, &FunctionDetails::execInputAdded);
// 	connect(execins, &ExecParamListWidget::execDeleted, this, &FunctionDetails::execInputDeleted);
// 	connect(execins, &ExecParamListWidget::execChanged, this, &FunctionDetails::execInputChanged);
// 
// 	connect(execouts, &ExecParamListWidget::execAdded, this, &FunctionDetails::execOutputAdded);
// 	connect(execouts, &ExecParamListWidget::execDeleted, this, &FunctionDetails::execOutputDeleted);
// 	connect(execouts, &ExecParamListWidget::execChanged, this, &FunctionDetails::execOutputChanged);
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

