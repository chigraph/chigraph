#pragma once

#ifndef CHIG_GUI_MODULE_BROWSER_HPP
#define CHIG_GUI_MODULE_BROWSER_HPP

#include <QTreeWidget>

class MainWindow;

#include <chi/Context.hpp>
#include <chi/GraphModule.hpp>

#include <unordered_set>

class ModuleTreeItem;

class ModuleBrowser : public QTreeWidget {
	Q_OBJECT

public:
	ModuleBrowser(QWidget* parent = nullptr);

signals:
	void moduleSelected(const QString& name);
	void discardChanges(const std::string& moduleName);

public slots:
	void loadWorkspace(chi::Context& context);
	void moduleDirtied(chi::ChiModule& dirtied);
	void moduleSaved(chi::ChiModule& saved);

	const std::unordered_set<chi::ChiModule*> dirtyModules() const { return mDirtyModules; }

private:
	void updateDirtyStatus(chi::ChiModule& updated, bool dirty);

	chi::Context*                        mContext = nullptr;
	std::unordered_set<chi::ChiModule*> mDirtyModules;
	std::unordered_map<std::string, ModuleTreeItem*> mItems;
};

#endif  // CHIG_GUI_MODULE_BROWSER_HPP
