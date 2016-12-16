#pragma once

#ifndef CHIG_GUI_MODULE_BROWSER_HPP
#define CHIG_GUI_MODULE_BROWSER_HPP

#include <QTreeWidget>

class MainWindow;

class ModuleBrowser : public QTreeWidget
{
	Q_OBJECT

public:
	ModuleBrowser(QWidget* parent = nullptr);

signals:
	void moduleSelected(QString name);

public slots:
	void loadWorkspace(QString path);

private:
	MainWindow* mMainWindow;
};

#endif  // CHIG_GUI_MODULE_BROWSER_HPP
