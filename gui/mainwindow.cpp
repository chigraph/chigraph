#include "mainwindow.hpp"

#include <KActionCollection>
#include <KLocalizedString>
#include <KMessageBox>
#include <KStandardAction>
#include <KConfigGroup>

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QProcess>
#include <QSplitter>
#include <QTextStream>

#include <chig/CModule.hpp>
#include <chig/Context.hpp>
#include <chig/GraphFunction.hpp>
#include <chig/JsonModule.hpp>
#include <chig/LangModule.hpp>
#include <chig/json.hpp>
#include <chig/Config.hpp>

#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Support/raw_ostream.h>

#include <fstream>

#include "chignodegui.hpp"
#include <KSharedConfig>

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
	functabs->setTabsClosable(true);
	connect(functabs, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);

	splitter->addWidget(functionpane);
	splitter->addWidget(functabs);
	splitter->setSizes({200, 1000});
    splitter->setCollapsible(0, false);
    splitter->setCollapsible(1, false);

	hlayout->addWidget(splitter);

	setupActions();
}

void MainWindow::setupActions()
{
	KStandardAction::quit(qApp, SLOT(quit()), actionCollection());

	QAction* openAction =
		actionCollection()->addAction(KStandardAction::Open, QStringLiteral("open"));
	openAction->setWhatsThis(QStringLiteral("Open a chigraph module"));
	connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
    
    openRecentAction = KStandardAction::openRecent(this, &MainWindow::openUrl, nullptr);
    actionCollection()->addAction(QStringLiteral("open-recent"), openRecentAction);
    
    openRecentAction->setToolBarMode(KRecentFilesAction::MenuMode);
    openRecentAction->setToolButtonPopupMode(QToolButton::DelayedPopup);
    openRecentAction->setIconText(i18nc("action, to open an archive", "Open"));
    openRecentAction->setToolTip(i18n("Open an archive"));
    openRecentAction->loadEntries(KSharedConfig::openConfig()->group("Recent Files"));


	QAction* newAction = actionCollection()->addAction(KStandardAction::New, QStringLiteral("new"));
	newAction->setWhatsThis(QStringLiteral("Create a new chigraph module"));

	QAction* saveAction =
		actionCollection()->addAction(KStandardAction::Save, QStringLiteral("save"));
	saveAction->setWhatsThis(QStringLiteral("Save the chigraph module"));
	connect(saveAction, &QAction::triggered, this, &MainWindow::save);
    
    QAction* runAction = new QAction;
    runAction->setText(i18n("&Run"));
    runAction->setIcon(QIcon::fromTheme("system-run"));
    actionCollection()->setDefaultShortcut(runAction, Qt::CTRL + Qt::Key_R);
    actionCollection()->addAction(QStringLiteral("run"), runAction);
    connect(runAction, &QAction::triggered, this, &MainWindow::run);

    
	setupGUI(Default, "chigguiui.rc");
}

MainWindow::~MainWindow() {
  openRecentAction->saveEntries(KSharedConfig::openConfig()->group("Recent Files"));
}

inline void MainWindow::addModule(std::unique_ptr<chig::ChigModule> module)
{
	assert(module);

	auto nodetypes = module->getNodeTypeNames();

	for (auto& nodetype : nodetypes) {
		std::unique_ptr<chig::NodeType> ty;
		module->createNodeType(nodetype.c_str(), {}, &ty);
		auto inst = new chig::NodeInstance(std::move(ty), 0, 0, nodetype);
		reg->registerModel(std::make_unique<ChigNodeGui>(inst));
	}

	ccontext.addModule(std::move(module));
}

void MainWindow::save()
{
	auto func = functabs->currentWidget();
	if (func) {
		static_cast<FunctionView*>(func)->updatePositions();
	}

	if (module) {
		std::ofstream stream(filename.toStdString());

		nlohmann::json j;
		chig::Result r = module->toJSON(&j);

		stream << j;
	}
}

void MainWindow::openFile()
{
	filename = QFileDialog::getOpenFileName(
		this, i18n("Chig Module"), QDir::homePath(), tr("Chigraph Modules (*.chigmod)"));

	if (filename == "") return;
    
    QUrl url = QUrl::fromLocalFile(filename);
    
    openRecentAction->addUrl(url);
    openUrl(url);
}

void MainWindow::openUrl(QUrl url) {
  	std::ifstream stream(url.toLocalFile().toStdString());

	chig::Result res;

	// read the JSON
	nlohmann::json j;

	try {
		stream >> j;
	} catch (std::exception& e) {
		KMessageBox::detailedError(
			this, "Invalid JSON file \"" + filename + "\"", e.what(), "Error Loading");

		return;
	}

	auto mod = std::make_unique<chig::JsonModule>(j, ccontext, &res);

	if (!res) {
		KMessageBox::detailedError(this, "Failed to load JsonModule from file \"" + filename + "\"",
			QString::fromStdString(res.result_json.dump(2)), "Error Loading");

		return;
	}

	if (!mod) {
		KMessageBox::error(this,
			"Unknown error in loading JsonModule from file \"" + filename + "\"", "Error Loading");
		return;
	}

	module = mod.get();
	addModule(std::move(mod));

	res += module->loadGraphs();

	if (!res) {
		KMessageBox::detailedError(this, "Failed to load graphs in JsonModule \"" + filename + "\"",
			QString::fromStdString(res.result_json.dump(2)), "Error Loading");

		return;
	}

	// call signal
	openJsonModule(module);
}

void MainWindow::newFunctionSelected(QString qstr)
{
	// load graph
	auto graphfunciter = std::find_if(module->functions.begin(), module->functions.end(),
		[&](auto& graphptr) { return qstr == QString::fromStdString(graphptr->graphName); });

	if (graphfunciter == module->functions.end()) {
		KMessageBox::error(
			this, "Unable to find function" + qstr + " in module", "Wrong function name");
		return;
	}

	auto iter = openFunctions.find(qstr);
	if (iter != openFunctions.end()) {
		functabs->setCurrentWidget(iter->second);
		return;
	}

	auto view = new FunctionView(module, graphfunciter->get(), reg, functabs);
	int idx = functabs->addTab(view, qstr);
	openFunctions[qstr] = view;
	functabs->setTabText(idx, qstr);
	functabs->setCurrentWidget(view);
}

void MainWindow::closeTab(int idx)
{
	openFunctions.erase(std::find_if(openFunctions.begin(), openFunctions.end(),
		[&](auto& p) { return p.second == functabs->widget(idx); }));
	functabs->removeTab(idx);
}

void MainWindow::run() {

  if(!module) return;
  
  // compile!
  std::unique_ptr<llvm::Module> llmod;
  chig::Result res = module->compile(&llmod);
  
  if(!res) {
    KMessageBox::detailedError(this, "Failed to compile module", QString::fromStdString(res.result_json.dump(2)));
  }
  
  std::string str;
  llvm::raw_string_ostream os(str);
  
  llvm::WriteBitcodeToFile(llmod.get(), os);
  
  
  // run in lli
  auto lliproc = new QProcess(this);
  lliproc->setProgram(QStringLiteral(CHIG_LLI_EXE));
  lliproc->start();
  lliproc->write(str.c_str(), str.length());
  
  connect(lliproc, &QProcess::readyReadStandardOutput, this, [this, lliproc](){
    onSubprocessStdIn(lliproc);
  });
  
}

void MainWindow::onSubprocessStdIn(QProcess* process)
{
  std::cout << process->readAllStandardOutput().constData();
}

