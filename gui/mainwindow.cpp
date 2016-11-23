#include "mainwindow.hpp"

#include <KActionCollection>
#include <KStandardAction>
#include <KLocalizedString>
#include <KMessageBox>

#include <QAction>
#include <QTextStream>
#include <QApplication>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QSplitter>

#include <chig/Context.hpp>
#include <chig/JsonModule.hpp>
#include <chig/json.hpp>
#include <chig/LangModule.hpp>
#include <chig/GraphFunction.hpp>
#include <chig/CModule.hpp>

#include <fstream>

#include "chignodegui.hpp"

MainWindow::MainWindow(QWidget* parent) : KXmlGuiWindow(parent)
{
	reg = std::make_shared<DataModelRegistry>();
	
	addModule(std::make_unique<chig::LangModule>(ccontext));
	addModule(std::make_unique<chig::CModule>(ccontext));
	
	
	QFrame* hb = new QFrame(this);
	QHBoxLayout* hlayout = new QHBoxLayout(hb);
	hlayout->setMargin(0);
	hlayout->setSpacing(0);
	
	setCentralWidget(hb);
	
	QSplitter* splitter = new QSplitter;
	functionpane = new FunctionsPane(splitter, this);
	connect(functionpane, &FunctionsPane::functionSelected, this, &MainWindow::newFunctionSelected);
	
	functabs = new QTabWidget(this);
	functabs->setMovable(true);
	
	splitter->addWidget(functionpane);
	splitter->addWidget(functabs);
	
	hlayout->addWidget(splitter);
	
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


inline void MainWindow::addModule(std::unique_ptr<chig::ChigModule> module) {

	
	auto nodetypes = module->getNodeTypeNames();
	
	for(auto& nodetype : nodetypes) {
		reg->registerModel(std::make_unique<ChigNodeGui>(module.get(), nodetype, nlohmann::json()));
	}
	
	ccontext.addModule(std::move(module));
	
}


void MainWindow::openFile() {
	QString filename = QFileDialog::getOpenFileName(this, i18n("Chig Module"), QDir::homePath(), tr("Chigraph Modules (*.chigmod)"));
	
	if(filename == "") return;
	
	std::ifstream stream(filename.toStdString());
	
	chig::Result res;
	
	// read the JSON
	nlohmann::json j;
	
	try {
		stream >> j;
	} catch (std::exception& e) {
		KMessageBox::detailedError(this, "Invalid JSON file \"" + filename +
			"\"", e.what(), "Error Loading");
		
		return;
	}
	
	auto mod = std::make_unique<chig::JsonModule>(j, ccontext, &res);
	
	
	
	if(!res) {
		KMessageBox::detailedError(this, "Failed to load JsonModule from file \"" + filename +
			"\"", QString::fromStdString(res.result_json.dump(2)), "Error Loading");
		
		return;
	}
	
	if(!mod) {
		KMessageBox::error(this, "Unknown error in loading JsonModule from file \"" + filename + "\"", "Error Loading");
		return;
	}
	
	module = mod.get(); // cache it because of it it invalidated after the move()
	addModule(std::move(mod));
	
	// call signal
	openJsonModule(module);
	
}

void MainWindow::newFunctionSelected(QString qstr) {
	// load graph
	auto graphfunciter = std::find_if(module->functions.begin(), module->functions.end(), [&](auto& graphptr) {
		return qstr == QString::fromStdString(graphptr->graphName);
	});
	
	if(graphfunciter == module->functions.end()) {
		KMessageBox::error(this, "Unable to find function" + qstr + " in module", "Wrong function name");
		return;
	}
	
	functabs->addTab(new FunctionView(module, graphfunciter->get(), reg, functabs), qstr);
}
