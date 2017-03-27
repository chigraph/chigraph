#pragma once

#ifndef CHIGRAPHGUI_MODULE_BROWSER_HPP
#define CHIGRAPHGUI_MODULE_BROWSER_HPP

#include <QTreeView>

#include <KLocalizedString>

class MainWindow;

#include <chi/Fwd.hpp>

#include <unordered_map>
#include <unordered_set>

#include "toolview.hpp"

class WorkspaceTree;

class ModuleBrowser : public QTreeView, public ToolView {
	Q_OBJECT

	// ToolView interface
public:
	QWidget*           toolView() override { return this; }
	Qt::DockWidgetArea defaultArea() const override { return Qt::LeftDockWidgetArea; }
	QString            label() override { return i18n("Module Browser"); }

public:
	ModuleBrowser(QWidget* parent = nullptr);
	
	std::unordered_set<chi::GraphModule*> dirtyModules();

signals:
	void functionSelected(chi::GraphFunction& name);
	void discardChanges(chi::GraphModule& moduleName);

public slots:
	void loadWorkspace(chi::Context& context);
	void moduleDirtied(chi::GraphModule& dirtied);
	void moduleSaved(chi::GraphModule& saved);

private:
	void updateDirtyStatus(chi::GraphModule& updated, bool dirty);

	chi::Context*                         mContext = nullptr;

	QAction* mDiscardChangesAction = nullptr;
	
	std::unordered_set<chi::GraphModule*> mDirtyModules;
	
	WorkspaceTree* mTree = nullptr;
};

#endif  // CHIGRAPHGUI_MODULE_BROWSER_HPP
