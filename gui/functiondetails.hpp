#pragma once

#ifndef CHIGGUI_FUNCTIONDETAILS_HPP
#define CHIGGUI_FUNCTIONDETAILS_HPP

#include <QScrollArea>

#include <chig/GraphFunction.hpp>

#include "paramlistwidget.hpp"
#include "execparamlistwidget.hpp"

class FunctionView;

class FunctionDetails : public QScrollArea {
	Q_OBJECT
public:
	FunctionDetails(QWidget* parent = nullptr);

public slots:
	void loadFunction(FunctionView* func);

private slots:

	void execInputChanged(int idx, const QString& newName);
	void execInputAdded(const QString& name);
	void execInputDeleted(int idx);

	void execOutputChanged(int idx, const QString& newName);
	void execOutputAdded(const QString& name);
	void execOutputDeleted(int idx);

private:
	FunctionView*        mFuncView = nullptr;
	chig::GraphFunction* mFunc     = nullptr;

	ParamListWidget* ins;
	ParamListWidget* outs;
	
	ExecParamListWidget* execins;
	ExecParamListWidget* execouts;
};

#endif  // CHIGGUI_FUNCTIONDETAILS_HPP
