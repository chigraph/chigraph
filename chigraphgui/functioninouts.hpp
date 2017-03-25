#pragma once

#ifndef CHIGGUI_FUNCTION_IN_OUTS_HPP
#define CHIGGUI_FUNCTION_IN_OUTS_HPP

#include <QWidget>

#include <chi/GraphFunction.hpp>

#include "execparamlistwidget.hpp"
#include "paramlistwidget.hpp"

class FunctionView;

class FunctionInOuts : public QWidget {
	Q_OBJECT
public:
	explicit FunctionInOuts(QWidget* parent = nullptr);

public slots:
	void loadFunction(FunctionView* func);

signals:
	void dirtied();

private:
	FunctionView*       mFuncView = nullptr;
	chi::GraphFunction* mFunc     = nullptr;

	ParamListWidget* ins;
	ParamListWidget* outs;

	ExecParamListWidget* execins;
	ExecParamListWidget* execouts;
};

#endif  // CHIGGUI_FUNCTION_IN_OUTS_HPP
