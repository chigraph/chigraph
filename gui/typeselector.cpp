#include "typeselector.hpp"

#include <chig/ChigModule.hpp>
#include <chig/Context.hpp>
#include <chig/DataType.hpp>
#include <chig/GraphFunction.hpp>
#include <chig/Result.hpp>

#include <KLocalizedString>
#include <KMessageBox>

TypeSelector::TypeSelector(chig::ChigModule& module, QWidget* parent)
    : KComboBox(true, parent), mModule{&module} {
	KCompletion* completer = completionObject();

	setCompletionMode(KCompletion::CompletionPopupAuto);

	// add the module
	for (const auto& ty : module.typeNames()) {
		completer->addItem(QString::fromStdString(module.fullName() + ":" + ty));
	}

	// and its dependencies
	for (auto dep : module.dependencies()) {
		auto depMod = module.context().moduleByFullName(dep);
		for (const auto& type : depMod->typeNames()) {
			completer->addItem(QString::fromStdString(depMod->fullName() + ":" + type));
		}
	}

	connect(this, static_cast<void (KComboBox::*)()>(&KComboBox::returnPressed), this,
	        [&module, this]() { typeSelected(currentType()); });
}

void TypeSelector::setCurrentType(const chig::DataType& ty) {
	setCurrentText(QString::fromStdString(ty.qualifiedName()));
}

chig::DataType TypeSelector::currentType() {
	std::string mod, name;
	std::tie(mod, name) = chig::parseColonPair(currentText().toStdString());

	chig::DataType ty;
	auto           res = mModule->context().typeFromModule(mod, name, &ty);
	if (!res) {
		KMessageBox::detailedError(this, i18n("Failed to get type"),
		                           QString::fromStdString(res.dump()));
		return {};
	}

	return ty;
}
