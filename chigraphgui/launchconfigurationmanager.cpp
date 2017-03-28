#include "launchconfigurationmanager.hpp"

#include <KSharedConfig>
#include <KConfigGroup>

#include <QVector>
#include <QUuid>

LaunchConfiguration::LaunchConfiguration(KConfigGroup grp) : mConfigGroup{grp} {
}

LaunchConfigurationManager::LaunchConfigurationManager() {
	KConfigGroup config(KSharedConfig::openConfig(), "launchconfigurations");
	
	auto configurations = config.readEntry("configurations", QStringList());
	auto currentName = config.readEntry("current", QString());
	
	for (const auto& configName : configurations) {
		mConfigurations.emplace_back(KConfigGroup{KSharedConfig::openConfig(), configName});
		
		if (configName == currentName) {
			mCurrent = mConfigurations[mConfigurations.size() - 1];
		}
	}
}

LaunchConfiguration LaunchConfigurationManager::newConfiguration()
{
	// generate uuid for it
	auto uuid = QUuid::createUuid();
	
	// add it to the list
	KConfigGroup configs(KSharedConfig::openConfig(), "launchconfigurations");
	configs.writeEntry("configurations", configs.readEntry("configurations", QStringList()) << uuid.toString());
	
	auto group = KConfigGroup{KSharedConfig::openConfig(), uuid.toString()};
	
	
	mConfigurations.emplace_back(group);
	
	return mConfigurations[mConfigurations.size() - 1];
}


void LaunchConfigurationManager::setCurrentConfiguration(LaunchConfiguration config) {
    mCurrent = config;

    KConfigGroup kconfig(KSharedConfig::openConfig(), "launchconfigurations");
    kconfig.writeEntry("current", config.id());
}

