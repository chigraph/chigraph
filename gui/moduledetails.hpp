#pragma once

#ifndef CHIGUI_MODULE_DETAILS_HPP
#define CHIGUI_MODULE_DETAILS_HPP

#include <QWidget>

#include <chi/Fwd.hpp>

class ModuleDependencies;
class FunctionsPane;

class ModuleDetails : public QWidget {
	Q_OBJECT
public:
	explicit ModuleDetails(QWidget* parent = nullptr);

	void loadModule(chi::GraphModule& module);

signals:
	void functionSelected(chi::GraphFunction* func);
	void dependencyAdded(const QString& name);
	void dependencyRemoved(const QString& name);
	void dirtied();

private:
	ModuleDependencies* mModDeps;
	FunctionsPane*      mFuncBrowser;
};

#endif  // CHIGUI_MODULE_DETAILS_HPP
