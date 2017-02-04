#include "paramlistwidget.hpp"

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


ParamListWidget::ParamListWidget(QWidget* parent) : QWidget(parent) {
	
}

void ParamListWidget::setFunction(FunctionView* func, Type ty) { 
	mFunc = func; 
	mType = ty; 
	
	if (layout()) {
		delete layout();
	}
	
	auto layout = new QGridLayout;
	setLayout(layout);
	
	// populate it
	auto& typeVec = ty == Input ? mFunc->function()->dataInputs() : mFunc->function()->dataOutputs();
	
	auto id = 0;
	for (const auto& param : typeVec) {
		
		auto edit = new QLineEdit;
		edit->setText(QString::fromStdString(param.name));
		connect(edit, &QLineEdit::textChanged, this, [this, id](const QString& newText) {
			if (mType == Input) {
				mFunc->function()->modifyDataInput(id, {}, newText.toStdString());
				refreshEntry();
			} else {
				mFunc->function()->modifyDataOutput(id, {}, newText.toStdString());
				refreshExits();
			}
		});
		layout->addWidget(edit, id, 0);
		
		auto combo = new QComboBox;
		combo->addItems(createTypeOptions(mFunc->function()->module()));
		combo->setCurrentText(QString::fromStdString(param.type.qualifiedName()));
		connect(combo, &QComboBox::currentTextChanged, this, [this, id](const QString& newType) {
			std::string mod, name;
			std::tie(mod, name) = chig::parseColonPair(newType.toStdString());
			
			chig::DataType ty;
			auto res = mFunc->function()->context().typeFromModule(mod, name, &ty);
			if(!res) {
				KMessageBox::detailedError(this, i18n("Failed to get type"), QString::fromStdString(res.dump()));
				return;
			}
			if (mType == Input) {
				mFunc->function()->modifyDataInput(id, ty, {});
				refreshEntry();
			} else {
				mFunc->function()->modifyDataOutput(id, ty, {});
				refreshExits();
			}
			
		});
		layout->addWidget(combo, id, 1);
		
		auto deleteButton = new QPushButton(QIcon::fromTheme(QStringLiteral("list-remove")), {});
		connect(deleteButton, &QPushButton::pressed, this, [this, id]{
			if (mType == Input) {
				mFunc->function()->removeDataInput(id);
				refreshEntry();
			} else {
				mFunc->function()->removeDataInput(id);
				refreshExits();
			}
		});
		layout->addWidget(deleteButton, id, 2);
		
		++id;
	}
	
	
}



void ParamListWidget::refreshEntry() {
	
	auto entry = mFunc->function()->entryNode();
	if (entry == nullptr) { return; }
	mFunc->refreshGuiForNode(mFunc->guiNodeFromChigNode(entry));
}
void ParamListWidget::refreshExits() {
	
	for (const auto& exit : mFunc->function()->nodesWithType("lang", "exit")) {
		mFunc->refreshGuiForNode(mFunc->guiNodeFromChigNode(exit));
	}
	
	mFunc->refreshRegistry();
}
