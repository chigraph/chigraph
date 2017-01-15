#pragma once

#ifndef CHIG_GUI_MODULE_BROWSER_HPP
#define CHIG_GUI_MODULE_BROWSER_HPP

#include <QTreeWidget>

class MainWindow;

#include <chig/Context.hpp>
#include <chig/JsonModule.hpp>

class ModuleBrowser : public QTreeWidget {
	Q_OBJECT

public:
	ModuleBrowser(QWidget* parent = nullptr);

signals:
	void moduleSelected(const QString& name);

public slots:
	void loadWorkspace(chig::Context& context);

private:
	chig::Context* mContext = nullptr;
};

#endif  // CHIG_GUI_MODULE_BROWSER_HPP
