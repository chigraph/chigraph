#pragma once

#ifndef CHIGGUI_LOCAL_VARIABLES_HPP
#define CHIGGUI_LOCAL_VARIABLES_HPP

#include <QWidget>

#include "functionview.hpp"

class LocalVariables : public QWidget {
public:
	LocalVariables(QWidget* parent = nullptr);

	void loadFunction(FunctionView* func);

private:
	void refreshReferencingNodes(const std::string& name);
	void deleteReferencingNodes(const std::string& name);

	FunctionView* mFunctionView = nullptr;
};

#endif  // CHIGGUI_LOCAL_VARIABLES_HPP
