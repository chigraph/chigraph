#pragma once

#ifndef CHIGRAPHGUI_MODULE_BROWSER_HPP
#define CHIGRAPHGUI_MODULE_BROWSER_HPP

#include <QTreeWidget>

#include <KLocalizedString>

class MainWindow;

#include <chi/Context.hpp>
#include <chi/GraphModule.hpp>

#include <unordered_set>

#include "toolview.hpp"

class ModuleTreeItem;

class ModuleBrowser : public QTreeWidget, public ToolView {
	Q_OBJECT

	// ToolView interface
public:
	
	QWidget* toolView() override {
		return this;
	}
	Qt::DockWidgetArea defaultArea() const override {
		return Qt::LeftDockWidgetArea;
	}
	QString label() override {
		return i18n("Module Browser");
	}
	
	
	
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
	
	QAction* mDiscardChangesAction = nullptr;
};

#endif  // CHIGRAPHGUI_MODULE_BROWSER_HPP
