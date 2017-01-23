#pragma once

#ifndef CHIGGUI_MODULE_DEPENDENCIES_HPP
#define CHIGGUI_MODULE_DEPENDENCIES_HPP

#include <QInputDialog>
#include <QListWidget>
#include <QListWidgetItem>

#include <KLocalizedString>
#include <KMessageBox>

#include <chig/JsonModule.hpp>

class ModuleDependencies : public QListWidget {
	Q_OBJECT

public:
	ModuleDependencies(QWidget* parent = nullptr);
	~ModuleDependencies()              = default;

	chig::JsonModule* module() const { return mModule; }
public slots:
	void setModule(chig::JsonModule* mod) {
		clear();
		for(const auto& dep : mod->dependencies()) {
			if(dep == "c" || dep == "lang") {
				continue;
			}
			addItem(QString::fromStdString(dep));
		}
		addNewDepItem();
		mModule = mod;
	}

signals:
	void dependencyAdded(const QString& str);

private:
	void addNewDepItem();

	chig::JsonModule* mModule = nullptr;

	QListWidgetItem* mAddDepItem = nullptr;
};

#endif  // CHIGGUI_MODULE_DEPENDENCIES_HPP
