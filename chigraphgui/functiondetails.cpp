#include "functiondetails.hpp"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include "functioninouts.hpp"
#include "functionview.hpp"
#include "localvariables.hpp"

FunctionDetails::FunctionDetails(QWidget* parent) : QWidget{parent} {
	setXMLFile("chigraphfunctiondetailsui.rc");
	
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
	connect(mDescEdit, &QLineEdit::textChanged, this, [this](const QString& newText) {
		if (mFunction != nullptr) {
			auto stdStrNewText = newText.toStdString();
			if (stdStrNewText != mFunction->function()->description()) {
				mFunction->function()->setDescription(stdStrNewText);
				dirtied();
			}
		}
	});

	auto inoutsbox = new QGroupBox(i18n("Inputs/Outputs"));
	layout->addWidget(inoutsbox);

	auto inslayout = new QVBoxLayout;
	inoutsbox->setLayout(inslayout);

	mInOuts = new FunctionInOuts;
	inslayout->addWidget(mInOuts);
	connect(mInOuts, &FunctionInOuts::dirtied, this, &FunctionDetails::dirtied);

	auto localsbox = new QGroupBox(i18n("Local Variables"));
	layout->addWidget(localsbox);

	auto localslayout = new QVBoxLayout;
	localsbox->setLayout(localslayout);

	mLocals = new LocalVariables;
	localslayout->addWidget(mLocals);
	connect(mLocals, &LocalVariables::dirtied, this, &FunctionDetails::dirtied);
}

void FunctionDetails::loadFunction(FunctionView* funcView) {
	mFunction = funcView;

	mDescEdit->setText(QString::fromStdString(funcView->function()->description()));

	mInOuts->loadFunction(funcView);
	mLocals->loadFunction(funcView);
}

chi::GraphFunction* FunctionDetails::chiFunc() const {
    if (mFunction) {
        return mFunction->function();
    }
    return nullptr;
}

