#include <KAboutData>
#include <KCrash>
#include <KLocalizedString>
#include <KMessageBox>

#include <QApplication>
#include <QCommandLineParser>

#include "mainwindow.hpp"

int main(int argc, char** argv) {
	QApplication app(argc, argv);
	KCrash::initialize();

	KLocalizedString::setApplicationDomain("chigraphgui");

	KAboutData aboutData(QStringLiteral("chigraphgui"), i18n("Chigraph GUI"), QStringLiteral("1.0"),
	                     i18n("Chigraph Graphical User Interface"), KAboutLicense::GPL,
	                     i18n("(c) 2016 Russell Greene"), QStringLiteral(""),
	                     QStringLiteral("https://github.com/chigraph/chigraph"),
	                     QStringLiteral("https://github.com/chigraph/chigraph/issues"));

	aboutData.addAuthor(
	    i18n("Russell Greene"), i18n("Programmer"), QStringLiteral("russellgreene8@gmail.com"),
	    QStringLiteral("https://github.com/russelltg"), QStringLiteral("russelltg"));

	KAboutData::setApplicationData(aboutData);

	QCommandLineParser parser;
	parser.addHelpOption();
	parser.addVersionOption();

	aboutData.setupCommandLine(&parser);

	parser.process(app);

	aboutData.processCommandLine(&parser);

#ifdef WIN32
	QIcon::setThemeName("breeze");
#endif
	auto win = new MainWindow();
	win->show();

	return app.exec();
}
