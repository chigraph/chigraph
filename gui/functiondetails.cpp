#include "functiondetails.hpp"

#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

#include "functioninouts.hpp"
#include "localvariables.hpp"

FunctionDetails::FunctionDetails(QWidget* parent) {
	auto layout = new QVBoxLayout;
	setLayout(layout);
	
	auto descSetterWidget = new QWidget;
	layout->addWidget(descSetterWidget);
	
	auto descHBox = new QHBoxLayout;
	descSetterWidget->setLayout(descHBox);
	
	auto descLabel = new QLabel(i18n("Description:"));
	descHBox->addWidget(descLabel);
	
	mDescEdit = new QLineEdit;
	descHBox->addWidget(mDescEdit);
	connect (mDescEdit, &QLineEdit::textChanged, this, [this](const QString& newText) {
		if (mFunction != nullptr) {
			mFunction->function()->setDescription(newText.toStdString());
		}
	});

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
	mFunction = funcView;
	
	mDescEdit->setText(QString::fromStdString(funcView->function()->description()));
	
	mInOuts->loadFunction(funcView);
	mLocals->loadFunction(funcView);
}
