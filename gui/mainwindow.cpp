#include "mainwindow.hpp"

#include <KActionCollection>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KMessageBox>
#include <KStandardAction>

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QHBoxLayout>
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
	reg = std::make_shared<DataModelRegistry>();

	addModule(std::make_unique<chig::LangModule>(ccontext));
	addModule(std::make_unique<chig::CModule>(ccontext));

	auto hb = new QFrame(this);
	auto hlayout = new QHBoxLayout(hb);
	hlayout->setMargin(0);
	hlayout->setSpacing(0);

	setCentralWidget(hb);

	auto splitter = new QSplitter;
	functionpane = new FunctionsPane(splitter, this);

	connect(functionpane, &FunctionsPane::functionSelected, this, &MainWindow::newFunctionSelected);

	functabs = new QTabWidget(this);
	functabs->setMovable(true);
	functabs->setTabsClosable(true);
	connect(functabs, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);

	outputView = new OutputView;

	auto middlesplitter = new QSplitter;
	middlesplitter->setOrientation(Qt::Vertical);
	middlesplitter->addWidget(functabs);
	middlesplitter->addWidget(outputView);
	middlesplitter->setSizes({1000, 200});

	splitter->addWidget(functionpane);
	splitter->addWidget(middlesplitter);
	splitter->setSizes({200, 1000});
	splitter->setCollapsible(0, false);
	splitter->setCollapsible(1, false);

	hlayout->addWidget(splitter);

	setupActions();
}

void MainWindow::setupActions()
{
	auto actColl = this->KXmlGuiWindow::actionCollection();

	KStandardAction::quit(qApp, SLOT(quit()), actColl);

	QAction* openAction = actColl->addAction(KStandardAction::Open, QStringLiteral("open"));
	openAction->setWhatsThis(QStringLiteral("Open a chigraph module"));
	connect(openAction, &QAction::triggered, this, &MainWindow::openFile);

	openRecentAction = KStandardAction::openRecent(this, &MainWindow::openUrl, nullptr);
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

	setupGUI(Default, "chigguiui.rc");
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

	ccontext.addModule(std::move(toAdd));
}

void MainWindow::save()
{
	for (size_t idx = 0; idx < functabs->count(); ++idx) {
		auto func = functabs->widget(idx);
		if (func != nullptr) {
			auto castedFunc = dynamic_cast<FunctionView*>(func);
			if (castedFunc != nullptr) {
				castedFunc->updatePositions();
			}
		}
	}

	if (module != nullptr) {
		std::ofstream stream(filename.toStdString());

		nlohmann::json j = {};
		chig::Result r = module->toJSON(&j);

		stream << j;
	}
}

void MainWindow::openFile()
{
	filename = QFileDialog::getOpenFileName(
		this, i18n("Chig Module"), QDir::homePath(), tr("Chigraph Modules (*.chigmod)"));

	if (filename == "") {
		return;
	}

	QUrl url = QUrl::fromLocalFile(filename);

	openRecentAction->addUrl(url);
	openUrl(url);
}

void MainWindow::openUrl(const QUrl& url)
{
	std::ifstream stream(url.toLocalFile().toStdString());

	chig::Result res;

	// read the JSON
	nlohmann::json j = {};

	try {
		stream >> j;
	} catch (std::exception& e) {
		KMessageBox::detailedError(
			this, R"(Invalid JSON file \")" + filename + R"(")", e.what(), "Error Loading");

		return;
	}

	// TODO: fix
	auto mod = std::make_unique<chig::JsonModule>(ccontext, "main", j, &res);

	if (!res) {
		KMessageBox::detailedError(this, "Failed to load JsonModule from file \"" + filename + "\"",
			QString::fromStdString(res.result_json.dump(2)), "Error Loading");

		return;
	}

	if (!mod) {
		KMessageBox::error(this,
			R"(Unknown error in loading JsonModule from file ")" + filename + R"(")",
			"Error Loading");
		return;
	}

	module = mod.get();
	addModule(std::move(mod));

	res += module->loadGraphs();

	if (!res) {
		KMessageBox::detailedError(this,
			R"(Failed to load graphs in JsonModule ")" + filename + R"(")",
			QString::fromStdString(res.result_json.dump(2)), "Error Loading");

		return;
	}

	// call signal
	openJsonModule(module);
}

void MainWindow::newFunctionSelected(QString name)
{
	// load graph
	auto graphfunciter = std::find_if(module->functions().begin(), module->functions().end(),
		[&](auto& graphptr) { return name == QString::fromStdString(graphptr->name()); });

	if (graphfunciter == module->functions().end()) {
		KMessageBox::error(
			this, "Unable to find function" + name + " in module", "Wrong function name");
		return;
	}

	auto iter = openFunctions.find(name);
	if (iter != openFunctions.end()) {
		functabs->setCurrentWidget(iter->second);
		return;
	}

	auto view = new FunctionView(module, graphfunciter->get(), reg, functabs);
	int idx = functabs->addTab(view, name);
	openFunctions[name] = view;
	functabs->setTabText(idx, name);
	functabs->setCurrentWidget(view);
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
	chig::Result res = ccontext.compileModule(module->fullName(), &llmod);

	if (!res) {
		KMessageBox::detailedError(
			this, "Failed to compile module", QString::fromStdString(res.result_json.dump(2)));

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
