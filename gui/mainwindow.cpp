#include "mainwindow.hpp"

#include <KActionCollection>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KMessageBox>
#include <KStandardAction>

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QDockWidget>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QPlainTextEdit>
#include <QProcess>
#include <QSplitter>
#include <QTextStream>

#include <chig/CModule.hpp>
#include <chig/Config.hpp>
#include <chig/Context.hpp>
#include <chig/GraphFunction.hpp>
#include <chig/JsonModule.hpp>
#include <chig/LangModule.hpp>
#include <chig/json.hpp>

#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Support/raw_ostream.h>

#include <fstream>

#include <KSharedConfig>
#include "chignodegui.hpp"

MainWindow::MainWindow(QWidget* parent) : KXmlGuiWindow(parent)
{
	Q_INIT_RESOURCE(chiggui);

	reg = std::make_shared<DataModelRegistry>();

	ccontext = std::make_unique<chig::Context>();

	QDockWidget* docker = new QDockWidget(i18n("Functions"), this);
	docker->setObjectName("Functions");
	functionpane = new FunctionsPane(this, this);
	docker->setWidget(functionpane);
	addDockWidget(Qt::LeftDockWidgetArea, docker);
	connect(functionpane, &FunctionsPane::functionSelected, this, &MainWindow::newFunctionSelected);

	docker = new QDockWidget(i18n("Modules"), this);
	docker->setObjectName("Modules");
	moduleBrowser = new ModuleBrowser(this);
	docker->setWidget(moduleBrowser);
	addDockWidget(Qt::LeftDockWidgetArea, docker);
	connect(this, &MainWindow::workspaceOpened, moduleBrowser, &ModuleBrowser::loadWorkspace);
	connect(moduleBrowser, &ModuleBrowser::moduleSelected, this, &MainWindow::openModule);

	functabs = new QTabWidget(this);
	functabs->setMovable(true);
	functabs->setTabsClosable(true);
	connect(functabs, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
	setCentralWidget(functabs);

	docker = new QDockWidget(i18n("Output"), this);
	docker->setObjectName("Output");
	outputView = new OutputView;
	docker->setWidget(outputView);
	addDockWidget(Qt::BottomDockWidgetArea, docker);

    docker = new QDockWidget(i18n("Function Details"), this);
    docker->setObjectName("Function Details");
    funcDetails = new FunctionDetails;
    docker->setWidget(funcDetails);
    addDockWidget(Qt::RightDockWidgetArea, docker);
    connect(this, &MainWindow::newFunctionOpened, funcDetails, &FunctionDetails::loadFunction);

	setupActions();
}

void MainWindow::setupActions()
{
	auto actColl = this->KXmlGuiWindow::actionCollection();

	KStandardAction::quit(qApp, SLOT(quit()), actColl);

	QAction* openAction = actColl->addAction(KStandardAction::Open, QStringLiteral("open"));
	openAction->setWhatsThis(QStringLiteral("Open a chigraph workspace"));
	connect(openAction, &QAction::triggered, this, &MainWindow::openWorkspaceDialog);

	openRecentAction = KStandardAction::openRecent(this, &MainWindow::openWorkspace, nullptr);
	actColl->addAction(QStringLiteral("open-recent"), openRecentAction);

	openRecentAction->setToolBarMode(KRecentFilesAction::MenuMode);
	openRecentAction->setToolButtonPopupMode(QToolButton::DelayedPopup);
	openRecentAction->setIconText(i18nc("action, to open an archive", "Open"));
	openRecentAction->setToolTip(i18n("Open an archive"));
	openRecentAction->loadEntries(KSharedConfig::openConfig()->group("Recent Files"));

	QAction* newAction = actColl->addAction(KStandardAction::New, QStringLiteral("new"));
	newAction->setWhatsThis(QStringLiteral("Create a new chigraph module"));

	QAction* saveAction = actColl->addAction(KStandardAction::Save, QStringLiteral("save"));
	saveAction->setWhatsThis(QStringLiteral("Save the chigraph module"));
	connect(saveAction, &QAction::triggered, this, &MainWindow::save);

	QAction* runAction = new QAction;
	runAction->setText(i18n("&Run"));
	runAction->setIcon(QIcon::fromTheme("system-run"));
	actColl->setDefaultShortcut(runAction, Qt::CTRL + Qt::Key_R);
	actColl->addAction(QStringLiteral("run"), runAction);
	connect(runAction, &QAction::triggered, this, &MainWindow::run);

	QAction* newFunctionAction = new QAction;
	newFunctionAction->setText(i18n("New Function"));
	newFunctionAction->setIcon(QIcon::fromTheme("list-add"));
	actColl->addAction(QStringLiteral("new-function"), newFunctionAction);
	connect(newFunctionAction, &QAction::triggered, this, &MainWindow::newFunction);

	setupGUI(Default, ":/share/kxmlgui5/chiggui/chigguiui.rc");
}

MainWindow::~MainWindow()
{
	openRecentAction->saveEntries(KSharedConfig::openConfig()->group("Recent Files"));
}

inline void MainWindow::addModule(std::unique_ptr<chig::ChigModule> toAdd)
{
	Expects(toAdd != nullptr);

	auto nodetypes = toAdd->nodeTypeNames();

	for (auto& nodetype : nodetypes) {
		std::unique_ptr<chig::NodeType> ty;
		toAdd->nodeTypeFromName(nodetype, {}, &ty);
		auto inst = new chig::NodeInstance(std::move(ty), 0, 0, nodetype);
		reg->registerModel(std::make_unique<ChigNodeGui>(inst));
	}

	ccontext->addModule(std::move(toAdd));
}

void MainWindow::save()
{
	for (int idx = 0; idx < functabs->count(); ++idx) {
		auto func = functabs->widget(idx);
		if (func != nullptr) {
			auto castedFunc = dynamic_cast<FunctionView*>(func);
			if (castedFunc != nullptr) {
				castedFunc->updatePositions();
			}
		}
	}

	if (module != nullptr) {
		std::ofstream stream(
			(ccontext->workspacePath() / "src" / (module->fullName() + ".chigmod")).string());

		nlohmann::json j = {};
		chig::Result r = module->toJSON(&j);

		stream << j;
	}
}

void MainWindow::openWorkspaceDialog()
{
	QString workspace = QFileDialog::getOpenFileName(
		this, i18n("Chigraph Workspace"), QDir::homePath(), {}, nullptr, QFileDialog::ShowDirsOnly);

	if (workspace == "") {
		return;
	}

	QUrl url = QUrl::fromLocalFile(workspace);

	openRecentAction->addUrl(url);

	openWorkspace(url);
}

void MainWindow::openWorkspace(QUrl url)
{
	ccontext = std::make_unique<chig::Context>(url.toLocalFile().toStdString());
	workspaceOpened(url.toLocalFile());
}

void MainWindow::openModule(QString path)
{
	chig::ChigModule* cmod;
	chig::Result res = ccontext->loadModule(path.toStdString(), &cmod);

	if (!res) {
		KMessageBox::detailedError(this, "Failed to load JsonModule from file \"" + path + "\"",
			QString::fromStdString(res.dump()), "Error Loading");

		return;
	}

	module = static_cast<chig::JsonModule*>(cmod);

	// call signal
	openJsonModule(module);
}

void MainWindow::newFunctionSelected(chig::GraphFunction* func)
{
	Expects(func);

	QString qualifiedFunctionName =
		QString::fromStdString(func->module().fullName() + ":" + func->name());

	auto iter = openFunctions.find(qualifiedFunctionName);
	if (iter != openFunctions.end()) {
		functabs->setCurrentWidget(iter->second);
		return;
	}

	auto view = new FunctionView(func, reg, functabs);
	int idx = functabs->addTab(view, qualifiedFunctionName);
	openFunctions[qualifiedFunctionName] = view;
	functabs->setTabText(idx, qualifiedFunctionName);
	functabs->setCurrentWidget(view);

    newFunctionOpened(func);
}

void MainWindow::closeTab(int idx)
{
	openFunctions.erase(std::find_if(openFunctions.begin(), openFunctions.end(),
		[&](auto& p) { return p.second == functabs->widget(idx); }));
	functabs->removeTab(idx);
}

void MainWindow::run()
{
	if (module == nullptr) {
		return;
	}

	// compile!
	std::unique_ptr<llvm::Module> llmod;
	chig::Result res = ccontext->compileModule(module->fullName(), &llmod);

	if (!res) {
		KMessageBox::detailedError(
			this, "Failed to compile module", QString::fromStdString(res.dump()));

		return;
	}

	std::string str;
	{
		llvm::raw_string_ostream os(str);

		llvm::WriteBitcodeToFile(llmod.get(), os);
	}

	// run in lli
	auto lliproc = new QProcess(this);
	lliproc->setProgram(QStringLiteral(CHIG_LLI_EXE));
	lliproc->start();
	lliproc->write(str.c_str(), str.length());
	lliproc->closeWriteChannel();

	outputView->setProcess(lliproc);
}

void MainWindow::newFunction()
{
	if (module == nullptr) {
		KMessageBox::error(this, "Load a module before creating a new function");
		return;
	}

	QString newName = QInputDialog::getText(this, i18n("New Function Name"), i18n("Function Name"));

	if (newName == "") {
		return;
	}

	module->createFunction(newName.toStdString(), {}, {});  // TODO: inputs
    functionpane->updateModule(module);
}
