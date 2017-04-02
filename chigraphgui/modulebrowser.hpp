#pragma once

#ifndef CHIGRAPHGUI_MODULE_BROWSER_HPP
#define CHIGRAPHGUI_MODULE_BROWSER_HPP

#include <QTreeView>

#include <KLocalizedString>

class MainWindow;

#include <chi/Fwd.hpp>

#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "toolview.hpp"

#include <boost/filesystem.hpp>

struct WorkspaceTree;
class ModuleTreeModel;

class ModuleBrowser : public QTreeView, public ToolView {
	Q_OBJECT

	// ToolView interface
public:
	QWidget*           toolView() override { return this; }
	Qt::DockWidgetArea defaultArea() const override { return Qt::LeftDockWidgetArea; }
	QString            label() override { return i18n("Module Browser"); }

public:
	ModuleBrowser(QWidget* parent = nullptr);
	~ModuleBrowser();

	std::unordered_set<chi::GraphModule*> dirtyModules();

signals:
	void functionSelected(chi::GraphFunction& name);

public slots:
	void loadWorkspace(chi::Context& context);
	void moduleDirtied(chi::GraphModule& dirtied);
	void moduleSaved(chi::GraphModule& saved);

private:
	void updateDirtyStatus(chi::GraphModule& updated, bool dirty);
	std::pair<WorkspaceTree*, QModelIndex> idxFromModuleName(const boost::filesystem::path& name);

	chi::Context* mContext = nullptr;

	std::unordered_set<chi::GraphModule*> mDirtyModules;

	WorkspaceTree*                   mTree = nullptr;
	std::unique_ptr<ModuleTreeModel> mModel;
};

#endif  // CHIGRAPHGUI_MODULE_BROWSER_HPP
