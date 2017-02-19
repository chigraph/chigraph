#pragma once

#ifndef CHIGGUI_THEME_MANAER_HPP
#define CHIGGUI_THEME_MANAER_HPP

#include <QObject>

#include <KActionMenu>
#include <KSharedConfig>

#include <memory>

class ThemeManager : public QObject {
	Q_OBJECT

public:
	ThemeManager(KActionMenu* menu);

	void setTheme(const QString& name);
	QString     theme();
	QStringList availableThemeFiles() const;
	QIcon createSchemePreviewIcon(const KSharedConfigPtr& ptr) const;

private:
	void changePalette();

	KActionMenu* mMenu;

	std::unique_ptr<QActionGroup> mActGroup;

	QMap<QString, QString> mThemeNameToFile;
};

#endif  // CHIGGUI_THEME_MANAER_HPP
