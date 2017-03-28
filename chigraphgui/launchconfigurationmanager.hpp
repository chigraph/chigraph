#pragma once

#ifndef CHIGRAPHGUI_LAUNCH_CONFIGURATION_HPP
#define CHIGRAPHGUI_LAUNCH_CONFIGURATION_HPP

#include <QObject>

#include <KConfigGroup>

struct LaunchConfiguration {
	
	// constructs an invalid configuration
	LaunchConfiguration() {} 
	
	LaunchConfiguration(KConfigGroup grp);
	
	QString name() const { return mConfigGroup.readEntry("name", QString()); }
	QString module() const { return mConfigGroup.readEntry("module", QString()); }
	QString workingDirectory() const { return mConfigGroup.readEntry("workingdirectory", QString()); }
	QString arguments() const { return mConfigGroup.readEntry("arguments", QString()); }
	QStringList env() const { return mConfigGroup.readEntry("environment", QStringList()); }
	
	void setName(const QString& newName) {
		mConfigGroup.writeEntry("name", newName);
	}
	void setModule(const QString& newModule) {
		mConfigGroup.writeEntry("module", newModule);
	}
	void setWorkingDirectory(const QString& newWd) {
		mConfigGroup.writeEntry("workingdirectory", newWd);
	}
	void setArguments(const QString& newArgs) {
		mConfigGroup.writeEntry("arguments", newArgs);
	}
	void setEnv(const QStringList& newEnv) {
		mConfigGroup.writeEntry("environment", newEnv);
	}
	
	bool valid() const { return mConfigGroup.isValid(); }
	
	QString id() const { return mConfigGroup.name(); }
	
private:
	
	KConfigGroup mConfigGroup;
	
};

class LaunchConfigurationManager : public QObject {
	Q_OBJECT
public:
	LaunchConfigurationManager();
	
	LaunchConfigurationManager(const LaunchConfigurationManager&) = delete;
	LaunchConfigurationManager(LaunchConfigurationManager&&) = delete;
	
	LaunchConfigurationManager& operator=(const LaunchConfigurationManager&) = delete;
	LaunchConfigurationManager& operator=(LaunchConfigurationManager&&) = delete;
	
	const std::vector<LaunchConfiguration> configurations() const { return mConfigurations; }
	
	LaunchConfiguration currentConfiguration() const { return mCurrent; }
	void setCurrentConfiguration(LaunchConfiguration config);
	
	LaunchConfiguration newConfiguration();
	
	LaunchConfiguration configByName(const QString& str) {
		for (const auto& config : configurations()) {
			if (config.name() == str) {
				return config;
			}
		}
		return {};
	}
	
private:
	
	LaunchConfiguration mCurrent;
	std::vector<LaunchConfiguration> mConfigurations;
};

#endif // CHIGRAPHGUI_LAUNCH_CONFIGURATION_HPP
