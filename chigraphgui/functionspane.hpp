#pragma once

#ifndef CHIGGUI_FUNCTIONS_PANE_HPP
#define CHIGGUI_FUNCTIONS_PANE_HPP

#include <QListWidget>

#include <chi/GraphModule.hpp>

class MainWindow;

class FunctionsPane : public QListWidget {
	Q_OBJECT
public:
	explicit FunctionsPane(QWidget* parent = nullptr);

public slots:

	void updateModule(chi::GraphModule& mod);

private slots:

	void selectItem(QListWidgetItem* newitem);
	void deleteFunction(chi::GraphFunction& func);

signals:

	void functionSelected(chi::GraphFunction& funcs);
};

#endif  // CHIGGUI_FUNCTIONS_PANE_HPP
