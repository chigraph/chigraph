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


private:
	FunctionView*        mFuncView = nullptr;
	chig::GraphFunction* mFunc     = nullptr;

	ParamListWidget* ins;
	ParamListWidget* outs;
	
	ExecParamListWidget* execins;
	ExecParamListWidget* execouts;
};

#endif  // CHIGGUI_FUNCTIONDETAILS_HPP
