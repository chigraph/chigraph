#pragma once

#ifndef CHIGGUI_FUNCTION_IN_OUTS_HPP
#define CHIGGUI_FUNCTION_IN_OUTS_HPP

#include <QWidget>

#include <chig/GraphFunction.hpp>

#include "execparamlistwidget.hpp"
#include "paramlistwidget.hpp"

class FunctionView;

class FunctionInOuts : public QWidget {
	Q_OBJECT
public:
	FunctionInOuts(QWidget* parent = nullptr);

public slots:
	void loadFunction(FunctionView* func);

private:
	FunctionView*        mFuncView = nullptr;
	chig::GraphFunction* mFunc     = nullptr;

	ParamListWidget* ins;
	ParamListWidget* outs;

	ExecParamListWidget* execins;
	ExecParamListWidget* execouts;
};

#endif  // CHIGGUI_FUNCTION_IN_OUTS_HPP
