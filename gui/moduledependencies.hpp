#pragma once

#ifndef CHIGGUI_MODULE_DEPENDENCIES_HPP
#define CHIGGUI_MODULE_DEPENDENCIES_HPP

#include <QInputDialog>
#include <QListWidget>
#include <QListWidgetItem>

#include <KLocalizedString>
#include <KMessageBox>

#include <chi/GraphModule.hpp>

class ModuleDependencies : public QListWidget {
	Q_OBJECT

public:
	explicit ModuleDependencies(QWidget* parent = nullptr);
	~ModuleDependencies()                       = default;

	chi::GraphModule* module() const { return mModule; }
public slots:
	void setModule(chi::GraphModule& mod) {
		clear();
		for (const auto& dep : mod.dependencies()) {
			if (dep == "c" || dep == "lang") { continue; }
			addItem(QString::fromStdString(dep.generic_string()));
		}
		addNewDepItem();
		mModule = &mod;
	}

signals:
	void dependencyAdded(const QString& str);
	void dependencyRemoved(const QString& str);

private:
	void addNewDepItem();

	chi::GraphModule* mModule = nullptr;

	QListWidgetItem* mAddDepItem = nullptr;
};

#endif  // CHIGGUI_MODULE_DEPENDENCIES_HPP
