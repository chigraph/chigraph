#include "launchconfigurationdialog.hpp"

#include <KLocalizedString>

#include <QSplitter>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QListWidget>

LaunchConfigurationDialog::LaunchConfigurationDialog(LaunchConfigurationManager& manager) : mManager{&manager}
{
	setWindowTitle(i18n("Launch Configurations"));
	
	// left side widget
	auto leftWidget = new QWidget;
	{
		auto layout = new QVBoxLayout;
		leftWidget->setLayout(layout);
		
		// new button
		auto newButton = new QPushButton(QIcon::fromTheme(QStringLiteral("list-add")), i18n("New Configuration"));
		layout->addWidget(newButton);
		connect(newButton, &QPushButton::pressed, this, &LaunchConfigurationDialog::addNewConfig);
		
		// config list
		mConfigList = new QListWidget;
		layout->addWidget(mConfigList);
		
		// populate it
		for (const auto& config : manager.configurations()) {
			mConfigList->addItem(config.name());
		}
		connect(mConfigList, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item){ selectConfig(item->text()); });
	}
	
	// right side widget
	auto rightWidget = new QWidget;
	{
		auto layout = new QFormLayout;
		rightWidget->setLayout(layout);
		
		// name
		{
			mNameEdit = new QLineEdit;
			connect(mNameEdit, &QLineEdit::textChanged, this, &LaunchConfigurationDialog::nameChanged);
			layout->addRow(i18n("Name"), mNameEdit);
		}
		
		// module
		{
			mModuleEdit = new QLineEdit;
			connect(mModuleEdit, &QLineEdit::textChanged, this, &LaunchConfigurationDialog::moduleChanged);
			layout->addRow(i18n("Module"), mModuleEdit);
		}
		
		// working directory
		{
			mWdEdit = new QLineEdit;
			connect(mWdEdit, &QLineEdit::textChanged, this, &LaunchConfigurationDialog::wdChanged);
			layout->addRow(i18n("Working Directory"), mWdEdit);
		}
		
		// arguments
		{
			mArgsEdit = new QLineEdit;
			connect(mArgsEdit, &QLineEdit::textChanged, this, &LaunchConfigurationDialog::argsChanged);
			layout->addRow(i18n("Arguments"), mArgsEdit);
		}
		
		
	}
	
	auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
	connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	
	
	
	auto rootSlider = new QSplitter;
	rootSlider->addWidget(leftWidget);
	rootSlider->addWidget(rightWidget);

	auto rootLayout = new QVBoxLayout;
	setLayout(rootLayout);
	rootLayout->addWidget(rootSlider);
	rootLayout->addWidget(buttonBox);
	
	// select the first one
	if (mManager->configurations().size() > 1) {
		selectConfig(mManager->configurations()[0].name());
	}
	
}


void LaunchConfigurationDialog::selectConfig(const QString& newConfig)
{
	// find it
	LaunchConfiguration config;
	for (const auto& conf : mManager->configurations()) {
		if (conf.name() == newConfig) {
			config = conf;
			break;
		}
	}
	if (!config.valid()) {
		return;
	}
	
	currentlyEditing = config;
	
	mNameEdit->setText(config.name());
	mWdEdit->setText(config.workingDirectory());
	mModuleEdit->setText(config.module());
	mArgsEdit->setText(config.arguments());
}

void LaunchConfigurationDialog::addNewConfig()
{
	auto config = mManager->newConfiguration();
	config.setName(i18n("New Configuration"));
	mConfigList->addItem(config.name());
	
	selectConfig(config.name());
}

void LaunchConfigurationDialog::argsChanged(const QString& newArgs)
{
	if (currentlyEditing.valid()) {
		currentlyEditing.setArguments(newArgs);
	}
}

void LaunchConfigurationDialog::moduleChanged(const QString& newModule)
{
	if (currentlyEditing.valid()) {
		currentlyEditing.setModule(newModule);
	}
}

void LaunchConfigurationDialog::nameChanged(const QString& newName)
{
	if (currentlyEditing.valid()) {
		// find it
		auto items = mConfigList->findItems(currentlyEditing.name(), Qt::MatchExactly);
		if (items.size() != 1) {
			return;
		}
		items[0]->setText(newName);
		
		currentlyEditing.setName(newName);
	}
	
}

void LaunchConfigurationDialog::wdChanged(const QString& newWd)
{
	if (currentlyEditing.valid()) {
		currentlyEditing.setWorkingDirectory(newWd);
	}
}



