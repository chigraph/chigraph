#include "execparamlistwidget.hpp"

#include <QGridLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

#include <KMessageBox>

#include <chig/GraphFunction.hpp>

#include "functionview.hpp"

namespace {

QStringList createTypeOptions(const chig::GraphModule& mod) {
	QStringList ret;
	
	// add the module
	for (const auto& ty : mod.typeNames()) {
		ret << QString::fromStdString(mod.name() + ":" + ty);
	}
	
	// and its dependencies
	for (auto dep : mod.dependencies()) {
		auto depMod = mod.context().moduleByFullName(dep);
		for (const auto& type : depMod->typeNames()) {
			ret << QString::fromStdString(depMod->name() + ":" + type);
		}
	}
	return ret;
}

} // anon namespace


ExecParamListWidget::ExecParamListWidget(QWidget* parent) : QWidget(parent) {
	
}

void ExecParamListWidget::setFunction(FunctionView* func, Type ty) { 
	mFunc = func; 
	mType = ty; 
	
	if (layout()) {
		delete layout();
	}
	
	auto layout = new QGridLayout;
	setLayout(layout);
	
	// populate it
	auto& typeVec = ty == Input ? mFunc->function()->execInputs() : mFunc->function()->execOutputs();
	
	auto id = 0;
	for (const auto& param : typeVec) {
		
		auto edit = new QLineEdit;
		edit->setText(QString::fromStdString(param));
		connect(edit, &QLineEdit::textChanged, this, [this, id](const QString& newText) {
			if (mType == Input) {
				mFunc->function()->modifyExecInput(id, newText.toStdString());
				refreshEntry();
			} else {
				mFunc->function()->modifyExecOutput(id, newText.toStdString());
				refreshExits();
			}
		});
		layout->addWidget(edit, id, 0);
		
		auto deleteButton = new QPushButton(QIcon::fromTheme(QStringLiteral("list-remove")), {});
		connect(deleteButton, &QPushButton::pressed, this, [this, id]{
			if (mType == Input) {
				mFunc->function()->removeExecInput(id);
				refreshEntry();
				setFunction(mFunc, mType);
			} else {
				mFunc->function()->removeExecOutput(id);
				refreshExits();
				setFunction(mFunc, mType);
			}
		});
		layout->addWidget(deleteButton, id, 1);
		
		++id;
	}
	
	// create the "new" button
	auto newButton = new QPushButton(QIcon::fromTheme("list-add"), {});
	connect(newButton, &QPushButton::pressed, this, [this] {
		if (mType == Input) {
			
			mFunc->function()->addExecInput("", mFunc->function()->execInputs().size() - 1);
			refreshEntry();
			
			setFunction(mFunc, mType); // TODO: not the most efficient way...
		} else {
			
			mFunc->function()->addExecOutput("", mFunc->function()->execOutputs().size() - 1);
			refreshExits();
			
			setFunction(mFunc, mType);
		}
		
	});
	layout->addWidget(newButton, id, 1);
	
	
}



void ExecParamListWidget::refreshEntry() {
	
	auto entry = mFunc->function()->entryNode();
	if (entry == nullptr) { return; }
	mFunc->refreshGuiForNode(mFunc->guiNodeFromChigNode(entry));
}
void ExecParamListWidget::refreshExits() {
	
	for (const auto& exit : mFunc->function()->nodesWithType("lang", "exit")) {
		mFunc->refreshGuiForNode(mFunc->guiNodeFromChigNode(exit));
	}
	
	mFunc->refreshRegistry();
}
