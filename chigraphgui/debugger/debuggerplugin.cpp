#include "debuggerplugin.hpp"

#include <chi/Result.hpp>

#include <KActionCollection>
#include <KLocalizedString>

#include <QApplication>
#include <QThread>
#include <QDebug>

#include <thread>

#include <../mainwindow.hpp>
#include <../functiontabview.hpp>

#include <lldb/API/SBThread.h>

DebuggerPlugin::DebuggerPlugin() {
	Q_INIT_RESOURCE(chigraphdebugger);
	
	qRegisterMetaType<lldb::SBEvent>("lldb::SBEvent");

	debugAction = actionCollection()->addAction(
	    QStringLiteral("start-debug"),
	    new QAction(QIcon::fromTheme(QStringLiteral("debug-run")), i18n("Debug")));
	actionCollection()->setDefaultShortcut(debugAction, Qt::Key_F8);
	connect(debugAction, &QAction::triggered, this, &DebuggerPlugin::debugStart);

	toggleBreakpointAction = actionCollection()->addAction(
	    QStringLiteral("toggle-breakpoint"),
	    new QAction(QIcon::fromTheme(QStringLiteral("draw-donut")), i18n("Toggle Breakpoint")));
	actionCollection()->setDefaultShortcut(toggleBreakpointAction, Qt::Key_F9);
	connect(toggleBreakpointAction, &QAction::triggered, this, &DebuggerPlugin::toggleBreakpoint);

	stepAction = actionCollection()->addAction(
	    QStringLiteral("debug-step"),
	    new QAction(QIcon::fromTheme(QStringLiteral("debug-step-over")), i18n("Step Over")));
	actionCollection()->setDefaultShortcut(stepAction, Qt::Key_F10);

	stepOutAction = actionCollection()->addAction(
	    QStringLiteral("debug-step-out"),
	    new QAction(QIcon::fromTheme(QStringLiteral("debug-step-out")), i18n("Step Out")));
	actionCollection()->setDefaultShortcut(stepOutAction, Qt::Key_F12);

	stepInAction = actionCollection()->addAction(
	    QStringLiteral("debug-step-into"),
	    new QAction(QIcon::fromTheme(QStringLiteral("debug-step-into")), i18n("Step Into")));
	actionCollection()->setDefaultShortcut(stepInAction, Qt::Key_F11);

	continueAction = actionCollection()->addAction(
	    QStringLiteral("debug-continue"),
	    new QAction(QIcon::fromTheme(QStringLiteral("media-playback-start")), i18n("Continue")));
	actionCollection()->setDefaultShortcut(continueAction, Qt::Key_F5);
	connect(continueAction, &QAction::triggered, this, &DebuggerPlugin::continueDebugging);
	
	mBreakpointView = new BreakpointView();
	mVariableView = new VariableView();

	setXMLFile("chigraphguidebuggerui.rc");
}

void DebuggerPlugin::toggleBreakpoint()
{
	auto currentView = MainWindow::instance()->tabView().currentView();
	
	if (currentView == nullptr) {
		return;
	}
	
	for(auto node : currentView->selectedNodes()) {
		mBreakpointView->addBreakpoint(*node);
	}
}

void DebuggerPlugin::debugStart() {
	boost::filesystem::path chiPath =
	    boost::filesystem::path(QApplication::applicationFilePath().toStdString()).parent_path() /
	    "chi";
#ifdef _WIN32
	chigPath.replace_extension(".exe");
#endif

	MainWindow* window = MainWindow::instance();

	// delete it
	if (mThread) {
		mThread->quit();
		mThread->deleteLater();
	}
	mEventListener = nullptr;
	
	// TODO: this really really needs a fix
	//mDebugger = std::make_unique<chi::Debugger>(chiPath.c_str(), *window->currentModule());
	
	mThread = new QThread;
	mEventListener = std::make_unique<DebuggerWorkerThread>(*mDebugger);
	mEventListener->moveToThread(mThread);
	
	connect(mThread, &QThread::started, mEventListener.get(), &DebuggerWorkerThread::process);
	connect(mEventListener.get(), &DebuggerWorkerThread::eventOccured, this, [this](lldb::SBEvent ev) {
		if (lldb::SBProcess::GetStateFromEvent(ev) == lldb::eStateStopped) {
			variableView().setFrame(mDebugger->lldbProcess().GetSelectedThread().GetSelectedFrame());
		}
	});
	
	mThread->start();
	
	// set breakpoints
	for (const auto& bp : breakpointView().breakpoints()) {
		mDebugger->setBreakpoint(*bp.first);
	}
	
	auto res = mDebugger->start();
	if (!res) {
		qDebug() << QString::fromStdString(res.dump());
		return;
	}
}


void DebuggerPlugin::continueDebugging() {
	if (mDebugger != nullptr) {
		mDebugger->processContinue();
	}
}
