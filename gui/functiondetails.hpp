#pragma once

#ifndef CHIGGUI_FUNCTIONDETAILS_HPP
#define CHIGGUI_FUNCTIONDETAILS_HPP

#include <QWidget>

#include <chig/GraphFunction.hpp>

#include "paramlistwidget.hpp"
#include "execparamlistwidget.hpp"

class FunctionView;

class FunctionDetails : public QWidget {
	Q_OBJECT
public:
	FunctionDetails(QWidget* parent = nullptr);

public slots:
	void loadFunction(FunctionView* func);

private slots:

	void inputChanged(int idx, const chig::DataType& newType, const QString& newName);
	void inputAdded(const chig::DataType& type, const QString& name);
	void inputDeleted(int idx);

	void outputChanged(int idx, const chig::DataType& newType, const QString& newName);
	void outputAdded(const chig::DataType& type, const QString& name);
	void outputDeleted(int idx);
	
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
