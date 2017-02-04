#include "mainwindow.hpp"
#include "functiondetails.hpp"
#include "functionspane.hpp"
#include "functionview.hpp"
#include "modulebrowser.hpp"
#include "moduledependencies.hpp"
#include "subprocessoutputview.hpp"

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
#include <chig/GraphModule.hpp>
#include <chig/LangModule.hpp>
#include <chig/json.hpp>

#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Support/raw_ostream.h>

#include <fstream>

#include <KSharedConfig>
#include "chigraphnodemodel.hpp"

MainWindow::MainWindow(QWidget* parent) : KXmlGuiWindow(parent) {
	Q_INIT_RESOURCE(chiggui);

	// set icon
	setWindowIcon(QIcon(":/icons/chigraphsmall.png"));

	mChigContext = std::make_unique<chig::Context>();

	// setup functions pane
	QDockWidget* docker = new QDockWidget(i18n("Functions"), this);
	docker->setObjectName("Functions");
	auto functionsPane = new FunctionsPane(this, this);
	docker->setWidget(functionsPane);
	addDockWidget(Qt::LeftDockWidgetArea, docker);
	connect(functionsPane, &FunctionsPane::functionSelected, this,
	        &MainWindow::newFunctionSelected);
	connect(this, &MainWindow::newFunctionCreated, functionsPane,
	        [functionsPane](chig::GraphFunction* func) {
		        functionsPane->updateModule(&func->module());
		    });
	connect(this, &MainWindow::moduleOpened, docker, [docker](chig::GraphModule* mod){
		docker->setWindowTitle(i18n("Functions") + " - " + QString::fromStdString(mod->fullName()));
	});

	// setup module browser
	docker = new QDockWidget(i18n("Modules"), this);
	docker->setObjectName("Modules");
	auto moduleBrowser = new ModuleBrowser(this);
	docker->setWidget(moduleBrowser);
	addDockWidget(Qt::LeftDockWidgetArea, docker);
	connect(this, &MainWindow::workspaceOpened, moduleBrowser, &ModuleBrowser::loadWorkspace);
	connect(moduleBrowser, &ModuleBrowser::moduleSelected, this, &MainWindow::openModule);
	connect(
	    this, &MainWindow::newModuleCreated, moduleBrowser,
	    [moduleBrowser](chig::GraphModule* mod) { moduleBrowser->loadWorkspace(mod->context()); });
	connect(this, &MainWindow::workspaceOpened, docker, [docker](chig::Context& ctx){
		docker->setWindowTitle(i18n("Modules") + " - " + QString::fromStdString(ctx.workspacePath().string()));
	});

	mFunctionTabs = new QTabWidget(this);
	mFunctionTabs->setMovable(true);
	mFunctionTabs->setTabsClosable(true);
	connect(mFunctionTabs, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);

	setCentralWidget(mFunctionTabs);

	docker = new QDockWidget(i18n("Output"), this);
	docker->setObjectName("Output");
	auto outputView = new QTabWidget();
	outputView->setTabsClosable(true);
	docker->setWidget(outputView);
	addDockWidget(Qt::BottomDockWidgetArea, docker);
	connect(outputView, &QTabWidget::tabCloseRequested, outputView, &QTabWidget::removeTab);

	docker = new QDockWidget(i18n("Function Details"), this);
	docker->setObjectName("Function Details");
	auto functionDetails = new FunctionDetails;
	docker->setWidget(functionDetails);
	addDockWidget(Qt::RightDockWidgetArea, docker);
	connect(this, &MainWindow::functionOpened, docker, [docker](FunctionView* func) {
		docker->setWindowTitle(i18n("Function Details") + " - " + QString::fromStdString(func->function()->name()));
	});
	connect(mFunctionTabs, &QTabWidget::currentChanged, functionDetails, [this, docker, functionDetails](int newLoc) {
		
		auto funcView = dynamic_cast<FunctionView*>(mFunctionTabs->widget(newLoc));
		
		if (funcView != nullptr) {
			
			functionDetails->loadFunction(funcView);
			docker->setWindowTitle(i18n("Function Details") + " - " + QString::fromStdString(funcView->function()->name()));
		}
	});
	

	docker = new QDockWidget(i18n("Module Dependencies"), this);
	docker->setObjectName("Module Dependencies");
	auto mModuleDeps = new ModuleDependencies;
	docker->setWidget(mModuleDeps);
	addDockWidget(Qt::RightDockWidgetArea, docker);
	connect(this, &MainWindow::moduleOpened, mModuleDeps, &ModuleDependencies::setModule);
	connect(mModuleDeps, &ModuleDependencies::dependencyAdded, this, [this] {
		auto count = mFunctionTabs->count();
		for (auto idx = 0; idx < count; ++idx) {
			auto view = dynamic_cast<FunctionView*>(mFunctionTabs->widget(idx));

			if (view) { view->refreshRegistry(); }
		}
	});
	connect(this, &MainWindow::moduleOpened, docker, [docker](chig::GraphModule* mod){
		docker->setWindowTitle(i18n("Module Dependencies") + " - " + QString::fromStdString(mod->fullName()));
	});

	/// Setup actions
	auto actColl = this->KXmlGuiWindow::actionCollection();

	KStandardAction::quit(qApp, SLOT(quit()), actColl);

	QAction* openAction = actColl->addAction(KStandardAction::Open, QStringLiteral("open"));
	openAction->setWhatsThis(QStringLiteral("Open a chigraph workspace"));
	connect(openAction, &QAction::triggered, this, &MainWindow::openWorkspaceDialog);

	mOpenRecentAction = KStandardAction::openRecent(this, &MainWindow::openWorkspace, nullptr);
	actColl->addAction(QStringLiteral("open-recent"), mOpenRecentAction);

	mOpenRecentAction->setToolBarMode(KSelectAction::MenuMode);
	mOpenRecentAction->setToolButtonPopupMode(QToolButton::DelayedPopup);
	mOpenRecentAction->setIconText(i18nc("action, to open an archive", "Open"));
	mOpenRecentAction->setToolTip(i18n("Open an archive"));
	mOpenRecentAction->loadEntries(KSharedConfig::openConfig()->group("Recent Files"));

	auto newAction = actColl->addAction(KStandardAction::New, QStringLiteral("new"));
	newAction->setWhatsThis(QStringLiteral("Create a new chigraph module"));

	auto saveAction = actColl->addAction(KStandardAction::Save, QStringLiteral("save"));
	saveAction->setWhatsThis(QStringLiteral("Save the chigraph module"));
	connect(saveAction, &QAction::triggered, this, &MainWindow::save);

	auto cancelAction = new QAction(nullptr);
	cancelAction->setEnabled(false);
	cancelAction->setText(i18n("Cancel"));
	cancelAction->setIcon(QIcon::fromTheme("process-stop"));
	actColl->setDefaultShortcut(cancelAction, Qt::CTRL + Qt::Key_Q);
	actColl->addAction(QStringLiteral("cancel"), cancelAction);
	connect(cancelAction, &QAction::triggered, this, [outputView] {
		auto output = dynamic_cast<SubprocessOutputView*>(outputView->currentWidget());

		if (output != nullptr) { output->cancelProcess(); }
	});

	connect(outputView, &QTabWidget::currentChanged, this, [cancelAction, outputView](int) {
		auto view = dynamic_cast<SubprocessOutputView*>(outputView->currentWidget());

		if (view != nullptr) {
			cancelAction->setEnabled(view->running());
		} else {
			cancelAction->setEnabled(false);
		}
	});

	auto runAction = new QAction(nullptr);
	runAction->setText(i18n("&Run"));
	runAction->setIcon(QIcon::fromTheme(QStringLiteral("system-run")));
	actColl->setDefaultShortcut(runAction, Qt::CTRL + Qt::Key_R);
	actColl->addAction(QStringLiteral("run"), runAction);
	connect(runAction, &QAction::triggered, this, [this, outputView, cancelAction] {
		if(currentModule() == nullptr) {
			KMessageBox::error(this, i18n("Load a module first!"), i18n("run: error"));
			return;
		}
		
		auto view = new SubprocessOutputView(currentModule());
		connect(view, &SubprocessOutputView::processFinished, this,
		        [outputView, view, cancelAction](int exitCode, QProcess::ExitStatus exitStatus) {
			        QString statusStr =
			            QString(" (%1, %2)")
			                .arg(exitStatus == QProcess::NormalExit ? "exited" : "crashed",
			                     QString::number(exitCode));
			        outputView->setTabText(
			            outputView->indexOf(view),
			            QString::fromStdString(view->module()->fullName()) + statusStr);

			        // disable it
			        if (outputView->currentWidget() == view) { cancelAction->setEnabled(false); }
			    });
		// add the tab to the beginning
		int newTabID = outputView->insertTab(
		    0, view, QString::fromStdString(currentModule()->fullName()) + i18n(" (running)"));
		outputView->setCurrentIndex(newTabID);
		cancelAction->setEnabled(true);
	});

	auto newFunctionAction = new QAction(nullptr);
	newFunctionAction->setText(i18n("New Function"));
	newFunctionAction->setIcon(QIcon::fromTheme("list-add"));
	actColl->addAction(QStringLiteral("new-function"), newFunctionAction);
	connect(newFunctionAction, &QAction::triggered, this, &MainWindow::newFunction);

	auto newModuleAction = new QAction(nullptr);
	newModuleAction->setText(i18n("New Module"));
	newModuleAction->setIcon(QIcon::fromTheme("package-new"));
	actColl->addAction(QStringLiteral("new-module"), newModuleAction);
	connect(newModuleAction, &QAction::triggered, this, &MainWindow::newModule);

	setupGUI(Default, ":/share/kxmlgui5/chiggui/chigguiui.rc");
}

MainWindow::~MainWindow() {
	mOpenRecentAction->saveEntries(KSharedConfig::openConfig()->group("Recent Files"));
}

void MainWindow::save() {
	for (int idx = 0; idx < mFunctionTabs->count(); ++idx) {
		auto func = mFunctionTabs->widget(idx);
		if (func != nullptr) {
			auto castedFunc = dynamic_cast<FunctionView*>(func);
			if (castedFunc != nullptr) { castedFunc->updatePositions(); }
		}
	}

	if (mModule != nullptr) {
		chig::Result res = mModule->saveToDisk();
		if (!res) {
			KMessageBox::detailedError(this, i18n("Failed to save module!"),
			                           QString::fromStdString(res.dump()));
		}
	}
}

void MainWindow::openWorkspaceDialog() {
	QString workspace =
	    QFileDialog::getExistingDirectory(this, i18n("Chigraph Workspace"), QDir::homePath(), {});

	if (workspace == "") { return; }

	QUrl url = QUrl::fromLocalFile(workspace);

	mOpenRecentAction->addUrl(url);

	openWorkspace(url);
}

void MainWindow::openWorkspace(const QUrl& url) {
	mChigContext = std::make_unique<chig::Context>(url.toLocalFile().toStdString());

	workspaceOpened(*mChigContext);
}

void MainWindow::openModule(const QString& fullName) {
	chig::ChigModule* cmod;
	chig::Result      res = context().loadModule(fullName.toStdString(), &cmod);

	if (!res) {
		KMessageBox::detailedError(this,
		                           R"(Failed to load JsonModule from file ")" + fullName + R"(")",
		                           QString::fromStdString(res.dump()), "Error Loading");

		return;
	}

	mModule = dynamic_cast<chig::GraphModule*>(cmod);
	Expects(mModule != nullptr);

	setWindowTitle(QString::fromStdString(mModule->fullName()));

	// call signal
	moduleOpened(mModule);
}

void MainWindow::newFunctionSelected(chig::GraphFunction* func) {
	Expects(func);

	QString qualifiedFunctionName =
	    QString::fromStdString(func->module().fullName() + ":" + func->name());

	// see if it's already open
	auto funcViewIter = mOpenFunctions.find(qualifiedFunctionName);
	if (funcViewIter != mOpenFunctions.end()) {
		mFunctionTabs->setCurrentWidget(funcViewIter->second);
		return;
	}
	// if it's not already open, we'll have to create our own

	auto view                             = new FunctionView(func, mFunctionTabs);
	int  idx                              = mFunctionTabs->addTab(view, qualifiedFunctionName);
	mOpenFunctions[qualifiedFunctionName] = view;
	mFunctionTabs->setTabText(idx, qualifiedFunctionName);
	mFunctionTabs->setCurrentWidget(view);

	functionOpened(view);
}

void MainWindow::closeTab(int idx) {
	mOpenFunctions.erase(std::find_if(mOpenFunctions.begin(), mOpenFunctions.end(), [&](auto& p) {
		return p.second == mFunctionTabs->widget(idx);
	}));
	mFunctionTabs->removeTab(idx);
}

void MainWindow::newFunction() {
	if (currentModule() == nullptr) {
		KMessageBox::error(this, "Load a module before creating a new function");
		return;
	}

	QString newName = QInputDialog::getText(this, i18n("New Function Name"), i18n("Function Name"));

	if (newName == "") { return; }

	chig::GraphFunction* func = currentModule()->getOrCreateFunction(newName.toStdString(), {}, {}, {""}, {""});
	func->getOrInsertEntryNode(0, 0, "entry");

	newFunctionCreated(func);
	newFunctionSelected(func);  // open the newly created function
}

void MainWindow::newModule() {
	// can't do this without a workspace
	if (context().workspacePath().empty()) {
		KMessageBox::error(this, i18n("Cannot create a module without a workspace to place it in"),
		                   i18n("Failed to create module"));
		return;
	}

	// TODO: validator
	auto fullName = QInputDialog::getText(this, i18n("New Module"), i18n("Full Module Name"));

	auto mod = context().newGraphModule(fullName.toStdString());

	// add lang and c
	mod->addDependency("lang");
	mod->addDependency("c");

	mod->saveToDisk();

	newModuleCreated(mod);
	// then load the module
	openModule(QString::fromStdString(mod->fullName()));
}
