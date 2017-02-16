#pragma once

#ifndef CHIGGUI_FUNCTIONDETAILS_HPP
#define CHIGGUI_FUNCTIONDETAILS_HPP

#include <QWidget>

class FunctionView;
class FunctionInOuts;
class LocalVariables;

class FunctionDetails : public QWidget {
	
public:
	FunctionDetails(QWidget* parent = nullptr);
	
	void loadFunction(FunctionView* funcView);
	
private:
	FunctionInOuts* mInOuts;
	LocalVariables* mLocals;
	
};

#endif  // CHIGGUI_FUNCTIONDETAILS_HPP
