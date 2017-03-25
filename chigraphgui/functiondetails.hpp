#pragma once

#ifndef CHIGGUI_FUNCTIONDETAILS_HPP
#define CHIGGUI_FUNCTIONDETAILS_HPP

#include <QWidget>

#include <KLocalizedString>

#include "toolview.hpp"

#include <chi/Fwd.hpp>

class FunctionView;
class FunctionInOuts;
class LocalVariables;

class QLineEdit;

class FunctionDetails : public QWidget, public ToolView {
	Q_OBJECT

	// ToolView interface
public:
	QWidget*           toolView() override { return this; }
	Qt::DockWidgetArea defaultArea() const override { return Qt::RightDockWidgetArea; }
	QString            label() override { return i18n("Function Details"); }

	chi::GraphFunction* chiFunc() const;

public:
	explicit FunctionDetails(QWidget* parent = nullptr);

	void loadFunction(FunctionView* funcView);

signals:
	void dirtied();

private:
	FunctionInOuts* mInOuts;
	LocalVariables* mLocals;

	QLineEdit* mDescEdit;

	FunctionView* mFunction = nullptr;
};

#endif  // CHIGGUI_FUNCTIONDETAILS_HPP
