#pragma once

#ifndef CHIGGUI_EXECPARAMLISTWIDGET_HPP
#define CHIGGUI_EXECPARAMLISTWIDGET_HPP

#include <QListWidget>
#include <QTableView>

class FunctionView;

class ExecParamListWidget : public QWidget {
	Q_OBJECT

public:
	enum Type {
		Input,
		Output
	};
	
	ExecParamListWidget(QWidget* parent = nullptr);

	void setFunction(FunctionView* func, Type ty);
	
private:
	void refreshEntry();
	void refreshExits();
	
	QListWidget*       mParamList;
	FunctionView* mFunc = nullptr;
	Type mType;
};

#endif  // CHIGGUI_EXECPARAMLISTWIDGET_HPP
