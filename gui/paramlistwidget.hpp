#pragma once

#ifndef CHIGGUI_PARAMLISTWIDGET_HPP
#define CHIGGUI_PARAMLISTWIDGET_HPP

#include <QListWidget>
#include <QTableView>

class FunctionView;

class ParamListWidget : public QWidget {
	Q_OBJECT

public:
	enum Type { Input, Output };

	ParamListWidget(QWidget* parent = nullptr);

	void setFunction(FunctionView* func, Type ty);

private:
	void refreshEntry();
	void refreshExits();

	QListWidget*  mParamList;
	FunctionView* mFunc = nullptr;
	Type          mType;
};

#endif  // CHIGGUI_PARAMLISTWIDGET_HPP
