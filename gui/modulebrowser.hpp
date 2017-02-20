#pragma once

#ifndef CHIG_GUI_MODULE_BROWSER_HPP
#define CHIG_GUI_MODULE_BROWSER_HPP

#include <QTreeWidget>

class MainWindow;

#include <chig/Context.hpp>
#include <chig/GraphModule.hpp>

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
	void loadWorkspace(chig::Context& context);
	void moduleDirtied(chig::ChigModule& dirtied);
	void moduleSaved(chig::ChigModule& saved);

	const std::unordered_set<chig::ChigModule*> dirtyModules() const { return mDirtyModules; }

private:
	void updateDirtyStatus(chig::ChigModule& updated, bool dirty);

	chig::Context*                        mContext = nullptr;
	std::unordered_set<chig::ChigModule*> mDirtyModules;
	std::unordered_map<std::string, ModuleTreeItem*> mItems;
};

#endif  // CHIG_GUI_MODULE_BROWSER_HPP
