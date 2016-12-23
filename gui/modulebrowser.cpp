#include "modulebrowser.hpp"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QTreeWidgetItem>

#include "mainwindow.hpp"

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

ModuleBrowser::ModuleBrowser(QWidget* parent) : QTreeWidget(parent)
{
	setColumnCount(1);
	setAnimated(true);
	connect(
		this, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem* item, int /*column*/) {
            if (item->childCount() != 0) { // don't do module folders
                return;
            }
			QString text = item->text(0);
			while (item->parent()) {
				item = item->parent();
				text = item->text(0) + "/" + text;
			}

			moduleSelected(text);
		});
}

void ModuleBrowser::loadWorkspace(QString path)
{
	fs::path srcDir = fs::path(path.toStdString()) / "src";
	QDirIterator srcIter(path + "/src", QStringList("*.chigmod"), QDir::Files,
		QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);

	std::unordered_map<std::string, QTreeWidgetItem*> topLevels;
	std::unordered_map<QTreeWidgetItem*, std::unordered_map<std::string, QTreeWidgetItem*>>
		children;

	while (srcIter.hasNext()) {
		fs::path module = srcIter.next().toStdString();

		fs::path relpath = fs::relative(module, srcDir);

		std::string topLevelName = relpath.begin()->string();
		QTreeWidgetItem* topLevel = nullptr;
		if (topLevels.find(topLevelName) != topLevels.end()) {
			topLevel = topLevels["topLevelName"];
		} else {
			topLevel = new QTreeWidgetItem(QStringList() << QString::fromStdString(topLevelName));
			addTopLevelItem(topLevel);
			topLevels[topLevelName] = topLevel;
		}

		// in each path, make sure it exists
		auto iter = relpath.begin();
		++iter;
		for (; iter != relpath.end(); ++iter) {
			if (children[topLevel].find(iter->string()) == children[topLevel].end()) {
				std::string name = fs::path(*iter).replace_extension("").string();
				auto newTopLevel =
					new QTreeWidgetItem(topLevel, QStringList() << QString::fromStdString(name));
				children[topLevel][name] = newTopLevel;
				topLevel = newTopLevel;
			}
		}
	}
}
