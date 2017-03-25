#include "typeselector.hpp"

#include <chi/ChiModule.hpp>
#include <chi/Context.hpp>
#include <chi/DataType.hpp>
#include <chi/GraphFunction.hpp>
#include <chi/Result.hpp>

#include <KLocalizedString>
#include <KMessageBox>

namespace {

class StringListValidator : public QValidator {
public:
	explicit StringListValidator(QStringList acceptableValues)
	    : mAcceptableValues{std::move(acceptableValues)} {}

	State validate(QString& input, int& pos) const override {
		if (mAcceptableValues.contains(input)) { return Acceptable; }
		return Intermediate;
	}

private:
	QStringList mAcceptableValues;
};

}  // anonymous namespace

TypeSelector::TypeSelector(chi::ChiModule& module, QWidget* parent)
    : KComboBox(true, parent), mModule{&module} {
	KCompletion* completer = completionObject();

	setCompletionMode(KCompletion::CompletionPopupAuto);

	QStringList possibleTypes;
	// add the module
	for (const auto& ty : module.typeNames()) {
		possibleTypes << QString::fromStdString(module.fullName() + ":" + ty);
	}

	// and its dependencies
	for (auto dep : module.dependencies()) {
		auto depMod = module.context().moduleByFullName(dep);
		for (const auto& type : depMod->typeNames()) {
			possibleTypes << QString::fromStdString(depMod->fullName() + ":" + type);
		}
	}

	completer->setItems(possibleTypes);
	setValidator(new StringListValidator(std::move(possibleTypes)));

	connect(this, static_cast<void (KComboBox::*)()>(&KComboBox::returnPressed), this,
	        [&module, this]() { typeSelected(currentType()); });
}

void TypeSelector::setCurrentType(const chi::DataType& ty) {
	setCurrentText(QString::fromStdString(ty.qualifiedName()));
}

chi::DataType TypeSelector::currentType() {
	std::string mod, name;
	std::tie(mod, name) = chi::parseColonPair(currentText().toStdString());

	chi::DataType ty;
	auto          res = mModule->context().typeFromModule(mod, name, &ty);
	if (!res) {
		KMessageBox::detailedError(this, i18n("Failed to get type"),
		                           QString::fromStdString(res.dump()));
		return {};
	}

	return ty;
}
