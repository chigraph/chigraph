#include "moduledependencies.hpp"

ModuleDependencies::ModuleDependencies(QWidget* parent) : QListWidget(parent)
{
	connect(this, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
		if (item != mAddDepItem) {
			return;
		}
		if (mModule == nullptr) {
			return;
		}

		// get module list
		QStringList modList;
		for (const auto& module : mModule->context().listModulesInWorkspace()) {
			if (module != mModule->fullName()) {
				modList << QString::fromStdString(module);
			}
		}

		QString mod =
			QInputDialog::getItem(this, i18n("Add Dependency"), i18n("Dependency"), modList);

		chig::Result res = mModule->addDependency(mod.toStdString());
		if (!res) {
			KMessageBox::detailedError(
				this, "Failed to load dependency: " + mod, QString::fromStdString(res.dump()));
			return;
		}
		dependencyAdded(mod);

		insertItem(count() - 1, mod);
	});
}

void ModuleDependencies::addNewDepItem()
{
	mAddDepItem = new QListWidgetItem(QStringLiteral("  <Add Dependnecy>  "));
	addItem(mAddDepItem);
}
