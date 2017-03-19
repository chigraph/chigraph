#include "mainwindow.hpp"
#include "functiondetails.hpp"
#include "functionspane.hpp"
#include "functionview.hpp"
#include "localvariables.hpp"
#include "modulebrowser.hpp"
#include "moduledependencies.hpp"
#include "moduledetails.hpp"
#include "subprocessoutputview.hpp"

#include <KActionCollection>
#include <KActionMenu>
#include <KColorScheme>
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
#include <QScrollArea>
#include <QSplitter>
#include <QTextStream>

#include <chi/CModule.hpp>
#include <chi/Context.hpp>
#include <chi/GraphFunction.hpp>
#include <chi/GraphModule.hpp>
#include <chi/LangModule.hpp>
#include <chi/json.hpp>

#include <llvm/Support/raw_ostream.h>

#include <fstream>

#include <KSharedConfig>
#include "chigraphnodemodel.hpp"
#include "thememanager.hpp"

MainWindow::MainWindow(QWidget* parent) : KXmlGuiWindow(parent) {
	Q_INIT_RESOURCE(chiggui);

	// set icon
	setWindowIcon(QIcon(":/icons/chigraphsmall.png"));

	mChigContext = std::make_unique<chi::Context>(qApp->arguments()[0].toStdString().c_str());

	// setup module details
	auto docker = new QDockWidget(i18n("Module Details"));
	docker->setObjectName("Module Details");
	auto modDetails = new ModuleDetails;
	docker->setWidget(modDetails);
	addDockWidget(Qt::LeftDockWidgetArea, docker);
	connect(modDetails, &ModuleDetails::functionSelected, this, &MainWindow::newFunctionSelected);
	connect(this, &MainWindow::moduleOpened, modDetails, &ModuleDetails::loadModule);
	connect(this, &MainWindow::newFunctionCreated, modDetails,
	        [modDetails](chi::GraphFunction* func) { modDetails->loadModule(func->module()); });
	auto dependencyUpdatedSlot = [this] {
		auto count = mFunctionTabs->count();
		for (auto idx = 0; idx < count; ++idx) {
			auto view = dynamic_cast<FunctionView*>(mFunctionTabs->widget(idx));

			if (view) { view->refreshRegistry(); }
		}
	};
	connect(modDetails, &ModuleDetails::dependencyAdded, this, dependencyUpdatedSlot);
	connect(modDetails, &ModuleDetails::dependencyRemoved, this, dependencyUpdatedSlot);
	connect(this, &MainWindow::moduleOpened, docker, [docker](chi::GraphModule& mod) {
		docker->setWindowTitle(i18n("Module Details") + " - " +
		                       QString::fromStdString(mod.fullName()));
	});
	connect(modDetails, &ModuleDetails::dirtied, this, &MainWindow::moduleDirtied);

	// setup module browser
	docker = new QDockWidget(i18n("Modules"), this);
	docker->setObjectName("Modules");
	mModuleBrowser = new ModuleBrowser(this);
	docker->setWidget(mModuleBrowser);
	addDockWidget(Qt::LeftDockWidgetArea, docker);
	connect(this, &MainWindow::workspaceOpened, mModuleBrowser, &ModuleBrowser::loadWorkspace);
	connect(mModuleBrowser, &ModuleBrowser::moduleSelected, this, &MainWindow::openModule);
	connect(this, &MainWindow::newModuleCreated, mModuleBrowser,
	        [this](chi::GraphModule* mod) { mModuleBrowser->loadWorkspace(mod->context()); });
	connect(this, &MainWindow::workspaceOpened, docker, [docker](chi::Context& ctx) {
		docker->setWindowTitle(i18n("Modules") + " - " +
		                       QString::fromStdString(ctx.workspacePath().string()));
	});
	connect(mModuleBrowser, &ModuleBrowser::discardChanges, this,
	        [this](const std::string& moduleName) {
		        discardChangesInModule(*context().moduleByFullName(moduleName));
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
	auto scroll          = new QScrollArea;
	scroll->setWidget(functionDetails);
	scroll->setWidgetResizable(true);
	docker->setWidget(scroll);
	addDockWidget(Qt::RightDockWidgetArea, docker);
	connect(mFunctionTabs, &QTabWidget::currentChanged, functionDetails,
	        [this, docker, functionDetails](int newLoc) {

		        auto funcView = dynamic_cast<FunctionView*>(mFunctionTabs->widget(newLoc));

		        if (funcView != nullptr) {
			        functionDetails->loadFunction(funcView);
			        docker->setWindowTitle(i18n("Function Details") + " - " +
			                               QString::fromStdString(funcView->function()->name()));
		        }
		    });
	connect(functionDetails, &FunctionDetails::dirtied, this, &MainWindow::moduleDirtied);

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
		if (currentModule() == nullptr) {
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

	// theme selector
	auto themeAction = new KActionMenu(i18n("Theme"), this);
	mThemeManager    = std::make_unique<ThemeManager>(themeAction);
	actColl->addAction(QStringLiteral("theme"), themeAction);

	setupGUI(Default, ":/share/kxmlgui5/chiggui/chigguiui.rc");
}

MainWindow::~MainWindow() {
	mOpenRecentAction->saveEntries(KSharedConfig::openConfig()->group("Recent Files"));
}

void MainWindow::save() {
	if (mModule != nullptr) {
		chi::Result res = mModule->saveToDisk();
		if (!res) {
			KMessageBox::detailedError(this, i18n("Failed to save module!"),
			                           QString::fromStdString(res.dump()));
		}
		mModuleBrowser->moduleSaved(*mModule);
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
	mChigContext = std::make_unique<chi::Context>(url.toLocalFile().toStdString());

	workspaceOpened(*mChigContext);
}

void MainWindow::openModule(const QString& fullName) {
	chi::ChiModule* cmod;
	chi::Result      res = context().loadModule(fullName.toStdString(), &cmod);

	if (!res) {
		KMessageBox::detailedError(this,
		                           R"(Failed to load JsonModule from file ")" + fullName + R"(")",
		                           QString::fromStdString(res.dump()), "Error Loading");

		return;
	}

	mModule = dynamic_cast<chi::GraphModule*>(cmod);
	Expects(mModule != nullptr);

	setWindowTitle(QString::fromStdString(mModule->fullName()));

	// call signal
	moduleOpened(*mModule);
}

void MainWindow::newFunctionSelected(chi::GraphFunction* func) {
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

	auto view                             = new FunctionView(this, func, mFunctionTabs);
	int  idx                              = mFunctionTabs->addTab(view, qualifiedFunctionName);
	mOpenFunctions[qualifiedFunctionName] = view;
	mFunctionTabs->setTabText(idx, qualifiedFunctionName);
	mFunctionTabs->setCurrentWidget(view);
	connect(view, &FunctionView::dirtied, this, &MainWindow::moduleDirtied);

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

	chi::GraphFunction* func =
	    currentModule()->getOrCreateFunction(newName.toStdString(), {}, {}, {""}, {""});
	func->getOrInsertEntryNode(0, 0, boost::uuids::string_generator()("entry"));

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
	if (fullName.isEmpty()) {
		KMessageBox::error(this, i18n("Cannot create a module wth an empty name"));

		return;
	}

	auto mod = context().newGraphModule(fullName.toStdString());

	// add lang and c
	mod->addDependency("lang");
	mod->addDependency("c");

	mod->saveToDisk();

	newModuleCreated(mod);
	// then load the module
	openModule(QString::fromStdString(mod->fullName()));
}

void MainWindow::moduleDirtied() {
	if (!currentModule()) { return; }

	mModuleBrowser->moduleDirtied(*currentModule());
}

void MainWindow::closeEvent(QCloseEvent* event) {
	// check through dirty modules
	if (mModuleBrowser->dirtyModules().empty()) {
		KXmlGuiWindow::closeEvent(event);
		return;
	}

	// see if they want to save them
	for (auto mod : mModuleBrowser->dirtyModules()) {
		auto bc = KMessageBox::questionYesNoCancel(
		    this,
		    i18n("Module <b>") + QString::fromStdString(mod->fullName()) +
		        i18n("</b> has unsaved changes. Do you want to save your changes or discard them?"),
		    i18n("Close"), KStandardGuiItem::save(), KStandardGuiItem::discard());

		switch (bc) {
		case KMessageBox::Yes:
			// this means save
			save();
			break;
		case KMessageBox::No:
			// this means discard
			discardChangesInModule(*mod);
			break;
		case KMessageBox::Cancel: event->ignore(); return;
		default: Expects(false);
		}
	}

	KXmlGuiWindow::closeEvent(event);
}

void MainWindow::discardChangesInModule(chi::ChiModule& mod) {
	// mark it as clean
	mModuleBrowser->moduleSaved(mod);

	int currTabId = mFunctionTabs->currentIndex();

	// close the function views
	std::vector<std::pair<std::string, int>> functionNames;
	for (const auto& pair : mOpenFunctions) {
		if (&pair.second->function()->module() == &mod) {
			auto idx = mFunctionTabs->indexOf(pair.second);
			functionNames.emplace_back(pair.second->function()->name(), idx);
			mFunctionTabs->removeTab(idx);
		}
	}

	bool isCurrentModule = &mod == currentModule();

	std::string fullName = mod.fullName();
	context().unloadModule(fullName);

	chi::ChiModule* cMod;
	context().loadModule(fullName, &cMod);
	chi::GraphModule* gMod = dynamic_cast<chi::GraphModule*>(cMod);

	if (isCurrentModule) { openModule(QString::fromStdString(fullName)); }

	// re-add the tabs in reverse order to keep the ids
	for (auto iter = functionNames.rbegin(); iter != functionNames.rend(); ++iter) {
		chi::GraphFunction* func = gMod->functionFromName(iter->first);
		QString              qualifiedFunctionName =
		    QString::fromStdString(gMod->fullName() + ":" + func->name());
		auto view = new FunctionView(this, func);
		mFunctionTabs->insertTab(iter->second, view, qualifiedFunctionName);
		mOpenFunctions[qualifiedFunctionName] = view;
		connect(view, &FunctionView::dirtied, this, &MainWindow::moduleDirtied);
	}

	mFunctionTabs->setCurrentIndex(currTabId);
}
