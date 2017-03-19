#include "modulebrowser.hpp"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QHeaderView>
#include <QMenu>
#include <QTreeWidgetItem>

#include "mainwindow.hpp"

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

constexpr static int ModuleTreeItemType = 1001;

class ModuleTreeItem : public QTreeWidgetItem {
public:
	ModuleTreeItem(QTreeWidgetItem* parent, fs::path path)
	    : QTreeWidgetItem(parent, ModuleTreeItemType), mName{std::move(path)} {
		setText(0, QString::fromStdString(mName.filename().string()));
		setIcon(0, QIcon::fromTheme(QStringLiteral("package-available")));
	}

	fs::path mName;
	bool     dirty = false;
};

ModuleBrowser::ModuleBrowser(QWidget* parent) : QTreeWidget(parent) {
	setColumnCount(1);
	setAnimated(true);
	setSortingEnabled(true);
	header()->close();
	connect(this, &QTreeWidget::itemDoubleClicked, this,
	        [this](QTreeWidgetItem* item, int /*column*/) {
		        if (item->type() != ModuleTreeItemType) {  // don't do module folders or modules
			        return;
		        }
		        ModuleTreeItem* casted = dynamic_cast<ModuleTreeItem*>(item);
		        Expects(casted != nullptr);

		        moduleSelected(QString::fromStdString(casted->mName.string()));
		    });
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &QWidget::customContextMenuRequested, this, [this](QPoint p) {
		QTreeWidgetItem* itemRaw = itemAt(p);
		if (!itemRaw) { return; }

		auto item = dynamic_cast<ModuleTreeItem*>(itemRaw);

		if (!item || !item->dirty) { return; }

		QMenu contextMenu;
		contextMenu.addAction(QIcon::fromTheme(QStringLiteral("view-refresh")),
		                      i18n("Discard Changes"), [this, item] {

			                      discardChanges(item->mName.string());

			                  });  // TODO: shortcut
		contextMenu.exec(mapToGlobal(p));
	});
}

void ModuleBrowser::loadWorkspace(chi::Context& context) {
	mContext = &context;

	// clear existing entries
	clear();
	mDirtyModules.clear();
	mItems.clear();

	auto modules = context.listModulesInWorkspace();

	std::unordered_map<std::string, QTreeWidgetItem*> topLevels;
	std::unordered_map<QTreeWidgetItem*, std::unordered_map<std::string, QTreeWidgetItem*>>
	    children;

	for (auto moduleName : modules) {
		fs::path module = moduleName;

		auto iter = module.begin();

		// consume the first
		std::string      topLevelName = module.begin()->string();
		QTreeWidgetItem* topLevel     = nullptr;
		if (topLevels.find(topLevelName) != topLevels.end()) {
			topLevel = topLevels[topLevelName];
		} else {
			// check if this is a module - ie no children
			auto iterCpy = module.begin();
			if (iterCpy == module.end()) { continue; }
			++iterCpy;
			if (iterCpy == module.end()) {
				auto newItem       = new ModuleTreeItem(nullptr, module);
				topLevel           = newItem;
				mItems[moduleName] = newItem;
			} else {
				topLevel =
				    new QTreeWidgetItem(QStringList() << QString::fromStdString(topLevelName));
			}
			addTopLevelItem(topLevel);
			topLevels[topLevelName] = topLevel;
		}

		// in each path, make sure it exists
		++iter;
		for (; iter != module.end(); ++iter) {
			if (children[topLevel].find(iter->string()) == children[topLevel].end()) {
				// see if it's a module
				bool isChigraphModule;
				{
					auto iterCpy = iter;
					std::advance(iterCpy, 1);
					isChigraphModule = iterCpy == module.end();
				}

				// convert to string
				std::string name = fs::path(*iter).string();

				QTreeWidgetItem* newTopLevel;
				if (isChigraphModule) {
					newTopLevel = mItems[moduleName] = new ModuleTreeItem(topLevel, module);
				} else {
					newTopLevel = new QTreeWidgetItem(
					    topLevel, QStringList() << QString::fromStdString(name));
				}
				// store in cache
				children[topLevel][name] = newTopLevel;

				topLevel = newTopLevel;
			} else {
				topLevel = children[topLevel][iter->string()];
			}
		}
	}
}

void ModuleBrowser::moduleDirtied(chi::ChiModule& dirtied) {
	mDirtyModules.insert(&dirtied);

	updateDirtyStatus(dirtied, true);
}

void ModuleBrowser::moduleSaved(chi::ChiModule& saved) {
	mDirtyModules.erase(&saved);

	updateDirtyStatus(saved, false);
}

void ModuleBrowser::updateDirtyStatus(chi::ChiModule& updated, bool dirty) {
	auto iter = mItems.find(updated.fullName());

	if (iter == mItems.end()) { return; }

	auto modItem = iter->second;

	if (dirty) {
		modItem->dirty = true;
		modItem->setText(0, QString::fromStdString(modItem->mName.filename().string()) + " *");

		QTreeWidgetItem* item = modItem;
		// propagate through the parents
		while (item != nullptr) {
			auto font = item->font(0);
			font.setWeight(QFont::Bold);

			item->setFont(0, font);

			item = item->parent();
		}

	} else {
		modItem->dirty = false;
		modItem->setText(0, QString::fromStdString(modItem->mName.filename().string()));

		QTreeWidgetItem* item = modItem;
		// propagate through the parents
		while (item != nullptr) {
			auto font = item->font(0);
			font.setWeight(QFont::Normal);

			item->setFont(0, font);

			item = item->parent();
		}
	}
}
