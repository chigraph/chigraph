#pragma once

#ifndef CHIGGUI_EXECPARAMLISTWIDGET_HPP
#define CHIGGUI_EXECPARAMLISTWIDGET_HPP

#include <QLayout>
#include <QLayoutItem>
#include <QListWidget>
#include <QTableView>

class FunctionView;

inline void deleteLayout(QLayout* layout) {
	QLayoutItem* item;
	QLayout*     sublayout;
	QWidget*     widget;
	while ((item = layout->takeAt(0))) {
		if ((sublayout = item->layout()) != 0) {
			deleteLayout(sublayout);
		} else if ((widget = item->widget()) != 0) {
			widget->hide();
			delete widget;
		} else {
			delete item;
		}
	}

	delete layout;
}

class ExecParamListWidget : public QWidget {
	Q_OBJECT

public:
	enum Type { Input, Output };

	ExecParamListWidget(QWidget* parent = nullptr);

	void setFunction(FunctionView* func, Type ty);

private:
	void refreshEntry();
	void refreshExits();

	QListWidget*  mParamList;
	FunctionView* mFunc = nullptr;
	Type          mType;
};

#endif  // CHIGGUI_EXECPARAMLISTWIDGET_HPP
