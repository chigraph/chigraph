#include "mainwindow.hpp"
#include "chigraphplugin.hpp"
#include "functiondetails.hpp"
#include "functiontabview.hpp"
#include "functionview.hpp"
#include "launchconfigurationdialog.hpp"
#include "localvariables.hpp"
#include "modulebrowser.hpp"
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
#include <QPluginLoader>
#include <QProcess>
#include <QScrollArea>
#include <QSplitter>
#include <QTextStream>

#include <chi/CModule.hpp>
#include <chi/Context.hpp>
#include <chi/DataType.hpp>
#include <chi/GraphFunction.hpp>
#include <chi/GraphModule.hpp>
#include <chi/LangModule.hpp>
#include <chi/Result.hpp>
#include <chi/json.hpp>

#include <llvm/Support/raw_ostream.h>

#include <fstream>

#include <KSharedConfig>
#include "chigraphnodemodel.hpp"
#include "thememanager.hpp"

MainWindow* MainWindow::mInstance = nullptr;

MainWindow::MainWindow(QWidget* parent) : KXmlGuiWindow(parent) {
	Q_INIT_RESOURCE(chigraphgui);

	mInstance = this;

	// set icon
	setWindowIcon(QIcon(":/icons/chigraphsmall.png"));

	mChigContext = std::make_unique<chi::Context>(qApp->arguments()[0].toStdString().c_str());

	mFunctionTabs = new FunctionTabView;
	mFunctionTabs->setMovable(true);
	mFunctionTabs->setTabsClosable(true);
	connect(mFunctionTabs, &FunctionTabView::dirtied, this, &MainWindow::moduleDirtied);
	insertChildClient(mFunctionTabs);

	setCentralWidget(mFunctionTabs);

	// setup module browser
	mModuleBrowser = new ModuleBrowser(this);
	auto docker    = new QDockWidget(mModuleBrowser->label(), this);
	docker->setObjectName("Modules");
	insertChildClient(mModuleBrowser);
	docker->setWidget(mModuleBrowser);
	addDockWidget(mModuleBrowser->defaultArea(), docker);
	connect(this, &MainWindow::workspaceOpened, mModuleBrowser, &ModuleBrowser::loadWorkspace);
	connect(mModuleBrowser, &ModuleBrowser::functionSelected, &tabView(),
	        &FunctionTabView::selectNewFunction);
	connect(this, &MainWindow::newModuleCreated, mModuleBrowser,
	        [this](chi::GraphModule& mod) { mModuleBrowser->loadWorkspace(mod.context()); });
	connect(this, &MainWindow::workspaceOpened, docker, [docker](chi::Context& ctx) {
		docker->setWindowTitle(i18n("Modules") + " - " +
		                       QString::fromStdString(ctx.workspacePath().string()));
	});

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
	insertChildClient(functionDetails);
	auto scroll = new QScrollArea;
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
	connect(functionDetails, &FunctionDetails::dirtied, this,
	        [this, functionDetails] { moduleDirtied(functionDetails->chiFunc()->module()); });

	/// load plugins
	for (QObject* plugin : QPluginLoader::staticInstances()) {
		if (auto chiPlugin = qobject_cast<ChigraphPlugin*>(plugin)) {
			insertChildClient(chiPlugin);

			for (auto view : chiPlugin->toolViews()) {
				docker = new QDockWidget(view->label(), this);
				docker->setWidget(view->toolView());

				insertChildClient(view);

				addDockWidget(view->defaultArea(), docker);
			}
		}
	}

	/// Setup actions
	auto actColl = actionCollection();

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
		if (!launchManager().currentConfiguration().valid()) {
			KMessageBox::error(this, i18n("Select a launch configuration"), i18n("run: error"));
			return;
		}

		// get the module
		chi::Result       res;
		chi::GraphModule* mod;
		std::tie(res, mod) = loadModule(launchManager().currentConfiguration().module());

		if (!res || !mod) {
			KMessageBox::detailedError(this, i18n("Failed to load module"),
			                           QString::fromStdString(res.dump()), i18n("run: error"));
			return;
		}

		auto view = new SubprocessOutputView(mod);
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
		    0, view, QString::fromStdString(mod->fullName()) + i18n(" (running)"));
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

	auto runConfigDialogAction = new QAction(this);
	runConfigDialogAction->setText(i18n("Configure Launches"));
	actColl->addAction(QStringLiteral("configure-launches"), runConfigDialogAction);
	connect(runConfigDialogAction, &QAction::triggered, this, [this] {
		LaunchConfigurationDialog d(mLaunchManager);

		d.exec();

		updateUsableConfigs();
	});

	mConfigSelectAction = new KSelectAction(QIcon::fromTheme(QStringLiteral("run-build-configure")),
	                                        i18n("Launch Configuration"), this);
	actColl->addAction(QStringLiteral("select-launch-configuration"), mConfigSelectAction);
	connect(mConfigSelectAction,
	        static_cast<void (KSelectAction::*)(const QString&)>(&KSelectAction::triggered), this,
	        [this](const QString& str) {
		        mLaunchManager.setCurrentConfiguration(mLaunchManager.configByName(str));
		    });
	updateUsableConfigs();
	if (mLaunchManager.currentConfiguration().valid()) {
		mConfigSelectAction->setCurrentAction(mLaunchManager.currentConfiguration().name(),
		                                      Qt::CaseSensitive);
	}

	// theme selector
	auto themeAction = new KActionMenu(i18n("Theme"), this);
	mThemeManager    = std::make_unique<ThemeManager>(themeAction);
	actColl->addAction(QStringLiteral("theme"), themeAction);

	setupGUI(Default, "chigraphguiui.rc");
}

MainWindow::~MainWindow() {
	mOpenRecentAction->saveEntries(KSharedConfig::openConfig()->group("Recent Files"));
}

std::pair<chi::Result, chi::GraphModule*> MainWindow::loadModule(const QString& name) {
	chi::ChiModule* mod;
	auto res = context().loadModule(name.toStdString(), chi::LoadSettings::Default, &mod);
	if (!res) { return {res, nullptr}; }
	return {res, dynamic_cast<chi::GraphModule*>(mod)};
}

void MainWindow::save() {
	auto currentFunc = tabView().currentView();
	if (!currentFunc) { return; }
	auto module = &currentFunc->function()->module();
	if (module != nullptr) {
		chi::Result res = module->saveToDisk();
		if (!res) {
			KMessageBox::detailedError(this, i18n("Failed to save module!"),
			                           QString::fromStdString(res.dump()));
		}
		mModuleBrowser->moduleSaved(*module);
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

void MainWindow::newFunction() {
	// TODO: reenable
	// 	if (currentModule() == nullptr) {
	// 		KMessageBox::error(this, "Load a module before creating a new function");
	// 		return;
	// 	}
	//
	// 	QString newName = QInputDialog::getText(this, i18n("New Function Name"), i18n("Function
	// Name"));
	//
	// 	if (newName == "") { return; }
	//
	// 	chi::GraphFunction* func =
	// 	    currentModule()->getOrCreateFunction(newName.toStdString(), {}, {}, {""}, {""});
	// 	func->getOrInsertEntryNode(0, 0, boost::uuids::random_generator()());
	//
	// 	newFunctionCreated(*func);
	//
	// 	tabView().selectNewFunction(*func);  // open the newly created function
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

	newModuleCreated(*mod);
	// then load the module
}

void MainWindow::moduleDirtied(chi::GraphModule& mod) { mModuleBrowser->moduleDirtied(mod); }

void MainWindow::updateUsableConfigs() {
	// save the current so we can set it back later
	QString selectedText = mConfigSelectAction->currentText();

	// repopulate
	mConfigSelectAction->clear();

	for (const auto& config : mLaunchManager.configurations()) {
		mConfigSelectAction->addAction(config.name());
	}

	// set it back
	mConfigSelectAction->setCurrentAction(selectedText, Qt::CaseSensitive);
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
			// TODO: implement discard
			break;
		case KMessageBox::Cancel: event->ignore(); return;
		default: Expects(false);
		}
	}

	KXmlGuiWindow::closeEvent(event);
}
