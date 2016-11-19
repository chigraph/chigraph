#include "mainwindow.h"

#include <QAction>
#include <KActionCollection>
#include <KStandardAction>
#include <KLocalizedString>
#include <KMessageBox>
#include <QApplication>


MainWindow::MainWindow(QWidget* parent) : KXmlGuiWindow(parent)
{
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
	
	
	setupGUI(Default, "chigguiui.rc");
}


