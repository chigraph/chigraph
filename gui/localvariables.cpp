#include "localvariables.hpp"
#include "execparamlistwidget.hpp"
#include "paramlistwidget.hpp"

#include <QPushButton>
#include <QGridLayout>
#include <QComboBox>

#include <chig/LangModule.hpp>

LocalVariables::LocalVariables(QWidget* parent) : QWidget{parent}
{
}

void LocalVariables::loadFunction(FunctionView* func)
{
	mFunctionView = func;
	
	if (layout()) { deleteLayout(layout()); }
	
	auto layout = new QGridLayout;
	setLayout(layout);
	
	auto id = 0;
	for (const auto& var : func->function()->localVariables()) {
		auto nameEdit = new QLineEdit;
		nameEdit->setText(QString::fromStdString(var.name));
		connect(nameEdit, &QLineEdit::textChanged, this, [this, id](const QString& newText) {
			auto stdstr = newText.toStdString();
			mFunctionView->function()->renameLocalVariable(mFunctionView->function()->localVariables()[id].name, stdstr);
			// update the registry
			mFunctionView->refreshRegistry();
			// update the nodes
			refreshReferencingNodes(stdstr);
		});
		layout->addWidget(nameEdit, id, 0);
		
		auto combo = new QComboBox;
		combo->addItems(createTypeOptions(mFunctionView->function()->module()));
		combo->setCurrentText(QString::fromStdString(var.type.qualifiedName()));
		connect(combo, &QComboBox::currentTextChanged, this, [this, id](const QString& newType) {
			std::string mod, name;
			std::tie(mod, name) = chig::parseColonPair(newType.toStdString());

			chig::DataType ty;
			auto           res = mFunctionView->function()->context().typeFromModule(mod, name, &ty);
			if (!res) {
				KMessageBox::detailedError(this, i18n("Failed to get type"),
				                           QString::fromStdString(res.dump()));
				return;
			}
			auto localName = mFunctionView->function()->localVariables()[id].name;

			mFunctionView->function()->retypeLocalVariable(localName, ty);
			mFunctionView->refreshRegistry();
			refreshReferencingNodes(localName);

		});
		layout->addWidget(combo, id, 1);
		
		auto deleteButton = new QPushButton(QIcon::fromTheme(QStringLiteral("list-remove")), {});
		connect(deleteButton, &QPushButton::clicked, this, [this, id] {
			auto name = mFunctionView->function()->localVariables()[id].name;
				
			// delete the referencing gui nodes
			deleteReferencingNodes(name);
			
			mFunctionView->function()->removeLocalVariable(name);
			mFunctionView->refreshRegistry();
			
			// refresh the local vars panel
			loadFunction(mFunctionView);
		
		});
		layout->addWidget(deleteButton, id, 2);
		
		++id;
	}
	auto newButton = new QPushButton(QIcon::fromTheme(QStringLiteral("list-add")), {});
	newButton->setSizePolicy({QSizePolicy::Maximum, QSizePolicy::Maximum});
	connect(newButton, &QPushButton::clicked, this, [this] {
		chig::DataType ty = mFunctionView->function()->context().langModule()->typeFromName("i32");
		
		mFunctionView->function()->getOrCreateLocalVariable("", ty);
		mFunctionView->refreshRegistry();
		
		loadFunction(mFunctionView);
	});
	layout->addWidget(newButton, id, 2);
	
}

void LocalVariables::refreshReferencingNodes(const std::string& name)
{
	auto setNodes = mFunctionView->function()->nodesWithType(mFunctionView->function()->module().fullName(), "_set_" + name);
	for (const auto& node : setNodes) {
		mFunctionView->refreshGuiForNode(mFunctionView->guiNodeFromChigNode(node));
	}
	auto getNodes = mFunctionView->function()->nodesWithType(mFunctionView->function()->module().fullName(), "_get_" + name);
	for (const auto& node : getNodes) {
		mFunctionView->refreshGuiForNode(mFunctionView->guiNodeFromChigNode(node));
	}
}


void LocalVariables::deleteReferencingNodes(const std::string& name)
{
	auto setNodes = mFunctionView->function()->nodesWithType(mFunctionView->function()->module().fullName(), "_set_" + name);
	for (const auto& node : setNodes) {
		mFunctionView->scene().removeNode(*mFunctionView->guiNodeFromChigNode(node));
	}
	auto getNodes = mFunctionView->function()->nodesWithType(mFunctionView->function()->module().fullName(), "_get_" + name);
	for (const auto& node : getNodes) {
		mFunctionView->scene().removeNode(*mFunctionView->guiNodeFromChigNode(node));
	}
}
