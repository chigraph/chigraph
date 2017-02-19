#include "functiondetails.hpp"

#include <QGroupBox>
#include <QVBoxLayout>

#include "functioninouts.hpp"
#include "localvariables.hpp"

FunctionDetails::FunctionDetails(QWidget* parent) {
	auto layout = new QVBoxLayout;
	setLayout(layout);

	auto inoutsbox = new QGroupBox(i18n("Inputs/Outputs"));
	layout->addWidget(inoutsbox);

	auto inslayout = new QVBoxLayout;
	inoutsbox->setLayout(inslayout);

	mInOuts = new FunctionInOuts;
	inslayout->addWidget(mInOuts);

	auto localsbox = new QGroupBox(i18n("Local Variables"));
	layout->addWidget(localsbox);

	auto localslayout = new QVBoxLayout;
	localsbox->setLayout(localslayout);

	mLocals = new LocalVariables;
	localslayout->addWidget(mLocals);
}

void FunctionDetails::loadFunction(FunctionView* funcView) {
	mInOuts->loadFunction(funcView);
	mLocals->loadFunction(funcView);
}
