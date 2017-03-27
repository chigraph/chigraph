#include "moduledetails.hpp"

#include <chi/GraphFunction.hpp>
#include <chi/GraphModule.hpp>

#include <QGroupBox>
#include <QVBoxLayout>

#include "moduledependencies.hpp"

ModuleDetails::ModuleDetails(QWidget* parent) {
	auto layout = new QVBoxLayout;
	setLayout(layout);


	auto depsbox = new QGroupBox(i18n("Dependencies"));
	layout->addWidget(depsbox);

	auto depslayout = new QVBoxLayout;
	depsbox->setLayout(depslayout);

	mModDeps = new ModuleDependencies;
	connect(mModDeps, &ModuleDependencies::dependencyAdded, this, &ModuleDetails::dependencyAdded);
	connect(mModDeps, &ModuleDependencies::dependencyAdded, this,
	        [this](auto) { this->dirtied(); });
	connect(mModDeps, &ModuleDependencies::dependencyRemoved, this,
	        &ModuleDetails::dependencyRemoved);
	connect(mModDeps, &ModuleDependencies::dependencyRemoved, this,
	        [this](auto) { this->dirtied(); });
	depslayout->addWidget(mModDeps);
}

void ModuleDetails::loadModule(chi::GraphModule& module) {
	mModDeps->setModule(module);
}
