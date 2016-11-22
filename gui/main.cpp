#include <QApplication>
#include <QCommandLineParser>
#include <KAboutData>
#include <KLocalizedString>
#include <KMessageBox>

#include "mainwindow.hpp"


int main(int argc, char** argv) {
	QApplication app(argc, argv);
	
	KLocalizedString::setApplicationDomain("chiggui");
	
	KAboutData aboutData(
		QStringLiteral("chiggui"),
		i18n("Chigraph GUI"),
		QStringLiteral("1.0"),
		i18n("Chigraph Graphical User Interface"),
		KAboutLicense::GPL,
		i18n("(c) 2016 Russell Greene"),
		i18n(""),
		QStringLiteral("https://github.com/russelltg/chigraph"),
		QStringLiteral("https://github.com/russelltg/chigraph/issues")
	);
	
	aboutData.addAuthor(i18n("Russell Greene"), i18n("Programmer"), 
						QStringLiteral("russellgreene8@gmail.com"), QStringLiteral("https://github.com/russelltg"), 
						QStringLiteral("russelltg")
				);
	
	KAboutData::setApplicationData(aboutData);
	
	QCommandLineParser parser;
	parser.addHelpOption();
	parser.addVersionOption();
	
	aboutData.setupCommandLine(&parser);
	
	parser.process(app);
	
	aboutData.processCommandLine(&parser);
	

	MainWindow* win = new MainWindow();
	win->show();
	
	return app.exec();
}
