#include "debuggerplugin.hpp"

#include <chi/Result.hpp>

#include <KActionCollection>
#include <KLocalizedString>

#include <QApplication>
#include <QDebug>

#include <thread>

#include <../mainwindow.hpp>

DebuggerPlugin::DebuggerPlugin() {
	Q_INIT_RESOURCE(chigraphdebugger);

	debugAction = actionCollection()->addAction(
	    QStringLiteral("start-debug"),
	    new QAction(QIcon::fromTheme(QStringLiteral("debug-run")), i18n("Debug")));
	actionCollection()->setDefaultShortcut(debugAction, Qt::Key_F8);
	connect(debugAction, &QAction::triggered, this, &DebuggerPlugin::debugStart);

	toggleBreakpointAction = actionCollection()->addAction(
	    QStringLiteral("toggle-breakpoint"),
	    new QAction(QIcon::fromTheme(QStringLiteral("draw-donut")), i18n("Toggle Breakpoint")));
	actionCollection()->setDefaultShortcut(toggleBreakpointAction, Qt::Key_F9);

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

	setXMLFile("chigraphguidebuggerui.rc");
}

void DebuggerPlugin::debugStart() {
	boost::filesystem::path chiPath =
	    boost::filesystem::path(QApplication::applicationFilePath().toStdString()).parent_path() /
	    "chi";
#ifdef _WIN32
	chigPath.replace_extension(".exe");
#endif

	MainWindow* window = MainWindow::instance();
	if (window->currentModule() == nullptr) { return; }

	mDebugger = std::make_unique<chi::Debugger>(chiPath.c_str(), *window->currentModule());

	std::this_thread::sleep_for(std::chrono::seconds(3));

	auto res = mDebugger->start();
	if (!res) {
		qDebug() << QString::fromStdString(res.dump());
		return;
	}
}
