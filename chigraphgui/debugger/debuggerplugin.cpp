#include "debuggerplugin.hpp"

#include <KLocalizedString>
#include <KActionCollection>


DebuggerPlugin::DebuggerPlugin() {
	Q_INIT_RESOURCE(chigraphdebugger);

	
    debugAction =
        actionCollection()->addAction(QStringLiteral("start-debug"),
                                      new QAction(QIcon::fromTheme(QStringLiteral("debug-run")), i18n("Debug")));

    toggleBreakpointAction =
        actionCollection()->addAction(QStringLiteral("toggle-breakpoint"),
                                      new QAction(QIcon::fromTheme(QStringLiteral("media-playback-stop")), i18n("Toggle Breakpoint")));

    setXMLFile("chigraphguidebuggerui.rc");
}

