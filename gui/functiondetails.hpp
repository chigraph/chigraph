#pragma once

#ifndef CHIGGUI_FUNCTIONDETAILS_HPP
#define CHIGGUI_FUNCTIONDETAILS_HPP

#include <QWidget>

#include <chig/GraphFunction.hpp>

#include "paramlistwidget.hpp"

class FunctionDetails : public QWidget
{
	Q_OBJECT
public:
	FunctionDetails(QWidget* parent = nullptr);

public slots:
	void loadFunction(chig::GraphFunction* func);

private slots:

	void inputChanged(int idx, chig::DataType newType, QString newName);
	void inputAdded(chig::DataType type, QString name);
	void inputDeleted(int idx);

	void outputChanged(int idx, chig::DataType newType, QString newName);
	void outputAdded(chig::DataType type, QString name);
	void outputDeleted(int idx);

private:
	chig::GraphFunction* mFunc = nullptr;

	ParamListWidget* ins;
	ParamListWidget* outs;
};

#endif  // CHIGGUI_FUNCTIONDETAILS_HPP
