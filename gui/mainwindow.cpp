#include "mainwindow.h"

#include <QAction>
#include <KActionCollection>
#include <KStandardAction>
#include <KLocalizedString>
#include <KMessageBox>
#include <QApplication>
#include <QFileDialog>

MainWindow::MainWindow(QWidget* parent) : KXmlGuiWindow(parent)
{
	scene = new FlowScene();
	view = new FlowView(scene);
	
	setCentralWidget(view);
	
	setupActions();
}

void MainWindow::setupActions()
{
	QAction* askAction = new QAction(this);
	askAction->setText(i18n("&Ask"));
	askAction->setIcon(QIcon::fromTheme("question"));
	
	actionCollection()->setDefaultShortcut(askAction, Qt::CTRL + Qt::Key_2);
	actionCollection()->addAction("ask", askAction);
	
	connect(askAction, &QAction::triggered, []() {
		KMessageBox::questionYesNo(0, i18n("Ask??"), i18n("Hi"));
	});
	
	KStandardAction::quit(qApp, SLOT(quit()), actionCollection());
	
	QAction* openAction = actionCollection()->addAction(KStandardAction::Open, QStringLiteral("open"));
	openAction->setWhatsThis(QStringLiteral("Open a chigraph module"));
	connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
	
	QAction* newAction = actionCollection()->addAction(KStandardAction::New, QStringLiteral("new"));
	newAction->setWhatsThis(QStringLiteral("Create a new chigraph module"));
	//newAction->setIcon(QIcon::fromTheme(QStringLiteral("new")))
	
	QAction* saveAction = actionCollection()->addAction(KStandardAction::Save, QStringLiteral("save"));
	saveAction->setWhatsThis(QStringLiteral("Save the chigraph module"));
	
	setupGUI(Default, "chigguiui.rc");
}

void MainWindow::openFile() {
	auto file = QFileDialog::getOpenFileName(this, i18n("Chig Module"), QDir::homePath(), tr("Chigraph Modules (*.chigmod)"));
	
	
}


