#pragma once

#ifndef CHIGGUI_FUNCTIONDETAILS_HPP
#define CHIGGUI_FUNCTIONDETAILS_HPP

#include <QWidget>

class FunctionView;
class FunctionInOuts;
class LocalVariables;

class QLineEdit;

class FunctionDetails : public QWidget {
public:
	FunctionDetails(QWidget* parent = nullptr);

	void loadFunction(FunctionView* funcView);

private:
	FunctionInOuts* mInOuts;
	LocalVariables* mLocals;
	
	QLineEdit* mDescEdit;
	
	FunctionView* mFunction = nullptr;
};

#endif  // CHIGGUI_FUNCTIONDETAILS_HPP
