#include "moduledependencies.hpp"

#include <chi/Result.hpp>
#include <chi/Context.hpp>

#include <QMenu>

ModuleDependencies::ModuleDependencies(QWidget* parent) : QListWidget(parent) {
	connect(this, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
		if (item != mAddDepItem) { return; }
		if (mModule == nullptr) { return; }

		// get module list
		QStringList modList;
		for (const auto& module : mModule->context().listModulesInWorkspace()) {
			if (module != mModule->fullName()) { modList << QString::fromStdString(module); }
		}

		QString mod =
		    QInputDialog::getItem(this, i18n("Add Dependency"), i18n("Dependency"), modList);

		auto res = mModule->addDependency(mod.toStdString());
		if (!res) {
			KMessageBox::detailedError(this, "Failed to load dependency: " + mod,
			                           QString::fromStdString(res.dump()));
			return;
		}
		dependencyAdded(mod);

		insertItem(count() - 1, mod);
	});

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &QWidget::customContextMenuRequested, this, [this](QPoint p) {
		QPoint global = mapToGlobal(p);

		QListWidgetItem* funcItem = item(indexAt(p).row());
		if (mAddDepItem == funcItem) { return; }
		// if we didn't right click on an item, then don't do anything
		if (funcItem == nullptr) { return; }

		QMenu contextMenu;
		contextMenu.addAction(QIcon::fromTheme(QStringLiteral("edit-delete")), i18n("Delete"),
		                      [this, funcItem] {
			                      std::string text = funcItem->text().toStdString();

			                      mModule->removeDependency(text);

			                      // reload
			                      setModule(*mModule);
			                  });  // TODO: shortcut
		contextMenu.exec(global);
	});
}

void ModuleDependencies::addNewDepItem() {
	mAddDepItem = new QListWidgetItem(QStringLiteral("  <Add Dependency>  "));
	addItem(mAddDepItem);
}
