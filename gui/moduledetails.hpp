#pragma once

#ifndef CHIGUI_MODULE_DETAILS_HPP
#define CHIGUI_MODULE_DETAILS_HPP

#include <QWidget>

#include <chig/Fwd.hpp>

class ModuleDependencies;
class FunctionsPane;

class ModuleDetails : public QWidget {
	Q_OBJECT
public:
	ModuleDetails(QWidget* parent = nullptr);

	void loadModule(chig::GraphModule& module);

signals:
	void functionSelected(chig::GraphFunction* func);
	void dependencyAdded(const QString& name);

private:
	ModuleDependencies* mModDeps;
	FunctionsPane*      mFuncBrowser;
};

#endif  // CHIGUI_MODULE_DETAILS_HPP
