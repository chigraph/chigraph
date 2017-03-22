#include "debuggerplugin.hpp"

#include <KLocalizedString>
#include <KActionCollection>


DebuggerPlugin::DebuggerPlugin() {
	Q_INIT_RESOURCE(chigraphdebugger);

	
	debugAction =
		actionCollection()->addAction(QStringLiteral("start-debug"),
									new QAction(QIcon::fromTheme(QStringLiteral("debug-run")), i18n("Debug")));
	debugAction->setShortcut(Qt::Key_F8);

	toggleBreakpointAction =
		actionCollection()->addAction(QStringLiteral("toggle-breakpoint"),
									new QAction(QIcon::fromTheme(QStringLiteral("draw-donut")), i18n("Toggle Breakpoint")));
	toggleBreakpointAction->setShortcut(Qt::Key_F9);
		
	stepAction = 
		actionCollection()->addAction(QStringLiteral("debug-step"),
			new QAction(QIcon::fromTheme(QStringLiteral("debug-step-over")), i18n("Step Over")));
	stepAction->setShortcut(Qt::Key_F10);
		
	stepOutAction =
		actionCollection()->addAction(QStringLiteral("debug-step-out"),
			new QAction(QIcon::fromTheme(QStringLiteral("debug-step-out")), i18n("Step Out")));
	stepOutAction->setShortcut(Qt::Key_F12);
	
	stepInAction =
		actionCollection()->addAction(QStringLiteral("debug-step-into"),
									  new QAction(QIcon::fromTheme(QStringLiteral("debug-step-into")), i18n("Step Into")));
	stepInAction->setShortcut(Qt::Key_F11);
	
	continueAction =
		actionCollection()->addAction(QStringLiteral("debug-continue"),
			new QAction(QIcon::fromTheme(QStringLiteral("media-playback-start")), i18n("Continue")));
	continueAction->setShortcut(Qt::Key_F5);
	
	
	setXMLFile("chigraphguidebuggerui.rc");
	
	
}

