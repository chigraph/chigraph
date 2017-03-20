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
	QWidget*     widget;
	while ((item = layout->takeAt(0))) {
		QLayout* sublayout = item->layout();
		if (sublayout != 0) {
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

	explicit ExecParamListWidget(QWidget* parent = nullptr);

	void setFunction(FunctionView* func, Type ty);
	
signals:
	void dirtied();

private:
	void refreshEntry();
	void refreshExits();

	FunctionView* mFunc = nullptr;
	Type          mType = Input;
};

#endif  // CHIGGUI_EXECPARAMLISTWIDGET_HPP
