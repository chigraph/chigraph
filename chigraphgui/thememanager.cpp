#include "thememanager.hpp"

#include <QBitmap>
#include <QDir>
#include <QMenu>
#include <QPainter>
#include <QStandardPaths>

#include <KColorScheme>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KMessageBox>
#include <KSharedConfig>
#include <KToolInvocation>
#include <QApplication>

ThemeManager::ThemeManager(KActionMenu* menu) : mMenu{menu} {
	Q_ASSERT(menu != nullptr);

	// clear the menu
	mMenu->menu()->clear();

	mActGroup = std::make_unique<QActionGroup>(mMenu);

	connect(mActGroup.get(), &QActionGroup::triggered, this, &ThemeManager::changePalette);

	// create the default skin
	auto defAct = new QAction(QStringLiteral("Default"), mActGroup.get());
	defAct->setCheckable(true);
	mMenu->addAction(defAct);

	auto availThemes = availableThemeFiles();
	availThemes.sort();

	for (const auto& themeFilename : availThemes) {
		// load the config
		auto config = KSharedConfig::openConfig(themeFilename);

		// create a simple theme icon
		auto themeIcon = createSchemePreviewIcon(config);

		// read the name of the theme
		auto        group = KConfigGroup{config, "General"};
		auto        info  = QFileInfo{themeFilename};
		const auto& name  = group.readEntry("Name", info.baseName());

		// create the action
		auto action = new QAction(name, mActGroup.get());
		action->setIcon(themeIcon);
		action->setCheckable(true);

		// add it to the map
		mThemeNameToFile[name] = themeFilename;

		// add it to the menu
		mMenu->addAction(action);
	}

	// add the configuration dialog to the end
	mMenu->addSeparator();
	auto configAct = new QAction(i18n("Configuration..."), mMenu);
	configAct->setIcon(QIcon::fromTheme(QStringLiteral("preferences-desktop-theme")));
	mMenu->addAction(configAct);
	connect(configAct, &QAction::triggered, this, [] {

		// open the colors dialog
		int errCode = KToolInvocation::kdeinitExec(QStringLiteral("kcmshell5"),
		                                           QStringList(QStringLiteral("colors")));

		if (errCode > 0) {
			KMessageBox::error(0,
			                   i18n("Cannot start Colors Settings panel from KDE Control Center. "
			                        "Please check your system..."));
		}
	});
}

QIcon ThemeManager::createSchemePreviewIcon(const KSharedConfigPtr& config) const {
	const uchar   bits1[] = {0xff, 0xff, 0xff, 0x2c, 0x16, 0x0b};
	const uchar   bits2[] = {0x68, 0x34, 0x1a, 0xff, 0xff, 0xff};
	const QSize   bitsSize(24, 2);
	const QBitmap b1 = QBitmap::fromData(bitsSize, bits1);
	const QBitmap b2 = QBitmap::fromData(bitsSize, bits2);

	QPixmap pixmap(23, 16);
	pixmap.fill(Qt::black);  // FIXME use some color other than black for borders?

	KConfigGroup group(config, "WM");
	QPainter     p(&pixmap);
	KColorScheme windowScheme(QPalette::Active, KColorScheme::Window, config);
	p.fillRect(1, 1, 7, 7, windowScheme.background());
	p.fillRect(2, 2, 5, 2, QBrush(windowScheme.foreground().color(), b1));

	KColorScheme buttonScheme(QPalette::Active, KColorScheme::Button, config);
	p.fillRect(8, 1, 7, 7, buttonScheme.background());
	p.fillRect(9, 2, 5, 2, QBrush(buttonScheme.foreground().color(), b1));

	p.fillRect(15, 1, 7, 7, group.readEntry("activeBackground", QColor(96, 148, 207)));
	p.fillRect(16, 2, 5, 2, QBrush(group.readEntry("activeForeground", QColor(255, 255, 255)), b1));

	KColorScheme viewScheme(QPalette::Active, KColorScheme::View, config);
	p.fillRect(1, 8, 7, 7, viewScheme.background());
	p.fillRect(2, 12, 5, 2, QBrush(viewScheme.foreground().color(), b2));

	KColorScheme selectionScheme(QPalette::Active, KColorScheme::Selection, config);
	p.fillRect(8, 8, 7, 7, selectionScheme.background());
	p.fillRect(9, 12, 5, 2, QBrush(selectionScheme.foreground().color(), b2));

	p.fillRect(15, 8, 7, 7, group.readEntry("inactiveBackground", QColor(224, 223, 222)));
	p.fillRect(16, 12, 5, 2, QBrush(group.readEntry("inactiveForeground", QColor(20, 19, 18)), b2));

	p.end();
	return pixmap;
}

QStringList ThemeManager::availableThemeFiles() const {
	const auto colorSchemeLocs =
	    QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
	                              QStringLiteral("color-schemes"), QStandardPaths::LocateDirectory);

	QStringList ret;
	for (const auto& colorDirName : colorSchemeLocs) {
		auto dir = QDir{colorDirName};

		for (const auto& scheme : dir.entryList(QStringList("*.colors"), QDir::Files)) {
			ret << dir.absoluteFilePath(scheme);
		}
	}

	return ret;
}

void ThemeManager::changePalette() { setTheme(theme()); }

void ThemeManager::setTheme(const QString& name) {
	QString themeName = name;

	if (themeName.isEmpty()) { themeName = QStringLiteral("Default"); }

	auto filename = mThemeNameToFile.value(themeName, QStringLiteral("Default"));

	KSharedConfigPtr config = KSharedConfig::openConfig(filename);
	qApp->setPalette(KColorScheme::createApplicationPalette(config));
}
QString ThemeManager::theme() {
	auto currentAct = mActGroup->checkedAction();

	if (currentAct != nullptr) { return currentAct->text().remove('&'); }
	return QStringLiteral("Default");
}
