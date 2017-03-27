#include "modulebrowser.hpp"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QHeaderView>
#include <QMenu>
#include <QTreeWidgetItem>

#include <KActionCollection>

#include "mainwindow.hpp"

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

struct WorkspaceTree {
	WorkspaceTree* parent = nullptr;
	std::vector<std::unique_ptr<WorkspaceTree>> children;
	bool isModule = false;
	chi::GraphModule* module = nullptr;
	chi::GraphFunction* func = nullptr;
	QString name;
	bool dirty = false;
	int row = 0;
};

class ModuleTreeModel : public QAbstractItemModel {
public:
	ModuleTreeModel(std::unique_ptr<WorkspaceTree> t) : tree{std::move(t)} {}
	
	int columnCount(const QModelIndex& parent) const override {
		return 1;
	}
	QModelIndex index(int row, int column, const QModelIndex& parent) const override {
		if (!hasIndex(row, column, parent)) {
			return {};
		}
		
		WorkspaceTree* parentItem;
		if (parent.isValid()) {
			parentItem = static_cast<WorkspaceTree*>(parent.internalPointer());
		} else {
			parentItem = tree.get();
		}
		
		if (row < parentItem->children.size()) {
			return createIndex(row, column, parentItem->children[row].get());
		}
		return {};
	}
	QModelIndex parent(const QModelIndex &index) const override {
		if (!index.isValid()) {
			return {};
		}
		
		auto childItem = static_cast<WorkspaceTree*>(index.internalPointer());
		auto parentItem = childItem->parent;

		if (parentItem == nullptr)
			return {};

		return createIndex(parentItem->row, 0, parentItem);

	}
	int rowCount(const QModelIndex &index) const override {
		if (!index.isValid()) {
			return 0;
		}
		auto item = static_cast<WorkspaceTree*>(index.internalPointer());
		return item->children.size();
	}
	QVariant data(const QModelIndex &index, int role) const override {
		if (!index.isValid()) {
			return {};
		}
		
		auto item = static_cast<WorkspaceTree*>(index.internalPointer());
		
		switch (role) {
		case Qt::DisplayRole:
			return item->name;
		default:
			return {};
		}
	}
	std::unique_ptr<WorkspaceTree> tree;
	
};

ModuleBrowser::ModuleBrowser(QWidget* parent) : QTreeView(parent) {
	setXMLFile("chigraphmodulebrowserui.rc");

	setAnimated(true);
	setSortingEnabled(true);
	header()->close();
	connect(this, &QTreeView::doubleClicked, this,
	        [this](const QModelIndex &index) {
				auto item = static_cast<WorkspaceTree*>(index.internalPointer());
				
		        if (item->func == nullptr) {  // don't do module folders or modules
			        return;
		        }
		        emit functionSelected(*item->func);
		    });
	setContextMenuPolicy(Qt::CustomContextMenu);

	mDiscardChangesAction = new QAction(QIcon::fromTheme(QStringLiteral("view-refresh")),
	                                    i18n("Discard Changes"), this);
	connect(mDiscardChangesAction, &QAction::triggered, this, [this] {
		auto idx = currentIndex();
		if (!idx.isValid()) {
			return;
		}
		auto item = static_cast<WorkspaceTree*>(idx.internalPointer());
		
		
		// get the module
		auto mod = item->module;
		if (!mod) { return; }

		discardChanges(*mod);
	});
	actionCollection()->addAction(QStringLiteral("discard-changes"), mDiscardChangesAction);

	connect(this, &QWidget::customContextMenuRequested, this, [this](QPoint p) {
		auto idx = indexAt(p);
		if (!idx.isValid()) { return; }

		auto item = static_cast<WorkspaceTree*>(idx.internalPointer());

		if (!item || !item->dirty) { return; }

		setCurrentIndex(idx);

		QMenu contextMenu;
		contextMenu.addAction(mDiscardChangesAction);
		contextMenu.exec(mapToGlobal(p));
	});
}

std::unordered_set<chi::GraphModule*> ModuleBrowser::dirtyModules() {
	return mDirtyModules;
}

void ModuleBrowser::loadWorkspace(chi::Context& context) {
	mContext = &context;

	auto modules = context.listModulesInWorkspace();

	auto tree = std::make_unique<WorkspaceTree>();
	
	// create the tree
	for (const fs::path& mod : modules) {
		fs::path buildingPath;
		WorkspaceTree* parent = tree.get();
		
		// for each component of mod
		for (fs::path component : mod) {
			bool isModule = component.extension() == ".chimod";
			component.replace_extension("");
			
			// make sure it exists
			bool found = false;
			for (const auto& child : parent->children) {
				if (child->name.toStdString() == component.string() && child->isModule == isModule) {
					found = true;
					parent = child.get();
					break;
				}
			}
			if (!found) {
				// insert it
				auto newChild = std::make_unique<WorkspaceTree>();
				newChild->parent = parent;
				newChild->isModule = isModule;
				newChild->row = parent->children.size();
				newChild->name = QString::fromStdString(component.string());
				parent->children.push_back(std::move(newChild));
				
				parent = parent->children[parent->children.size() - 1].get();
			}
			
		}
	}
	mTree = tree.get();
	
	setModel(new ModuleTreeModel(std::move(tree)));
}

void ModuleBrowser::moduleDirtied(chi::GraphModule& dirtied) {
	
	mDirtyModules.insert(&dirtied);
	updateDirtyStatus(dirtied, true);
}

void ModuleBrowser::moduleSaved(chi::GraphModule& saved) {
	updateDirtyStatus(saved, false);
	mDirtyModules.erase(&saved);
}

void ModuleBrowser::updateDirtyStatus(chi::GraphModule& updated, bool dirty) {
	
	// find the item
	WorkspaceTree* item = mTree;
	for (fs::path component : fs::path(updated.fullName())) {
		for (const auto& ch : item->children) {
			if (ch->name.toStdString() == component.string() && ch->isModule) {
				item = ch.get();
				break;
			}
		}
	}
	
	if (item->name.toStdString() != fs::path(updated.fullName()).filename().replace_extension("").string()) {
		return;
	}
	
	item->dirty = dirty;

}
