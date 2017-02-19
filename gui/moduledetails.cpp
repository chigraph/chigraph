#include "moduledetails.hpp"

#include <chig/GraphFunction.hpp>
#include <chig/GraphModule.hpp>

#include <QGroupBox>
#include <QVBoxLayout>

#include "functionspane.hpp"
#include "moduledependencies.hpp"

ModuleDetails::ModuleDetails(QWidget* parent) {
	auto layout = new QVBoxLayout;
	setLayout(layout);

	auto functionsbox = new QGroupBox(i18n("Functions"));
	layout->addWidget(functionsbox);

	auto funclayout = new QVBoxLayout;
	functionsbox->setLayout(funclayout);

	mFuncBrowser = new FunctionsPane;
	connect(mFuncBrowser, &FunctionsPane::functionSelected, this, &ModuleDetails::functionSelected);
	funclayout->addWidget(mFuncBrowser);

	auto depsbox = new QGroupBox(i18n("Dependencies"));
	layout->addWidget(depsbox);

	auto depslayout = new QVBoxLayout;
	depsbox->setLayout(depslayout);

	mModDeps = new ModuleDependencies;
	connect(mModDeps, &ModuleDependencies::dependencyAdded, this, &ModuleDetails::dependencyAdded);
	depslayout->addWidget(mModDeps);
}

void ModuleDetails::loadModule(chig::GraphModule& module) {
	mModDeps->setModule(module);
	mFuncBrowser->updateModule(module);
}
