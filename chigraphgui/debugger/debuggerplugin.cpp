#include "currentnodedecorator.hpp"
#include "debuggerplugin.hpp"

#include <chi/Result.hpp>

#include <KActionCollection>
#include <KLocalizedString>

#include <QApplication>
#include <QDebug>
#include <QToolTip>
#include <QThread>

#include <thread>

#include <../functiontabview.hpp>
#include <../mainwindow.hpp>
#include <../chigraphnodemodel.hpp>

#include <../src/Node.hpp>

#include <lldb/API/SBThread.h>

DebuggerPlugin::DebuggerPlugin() {
	Q_INIT_RESOURCE(chigraphdebugger);

	qRegisterMetaType<lldb::SBEvent>("lldb::SBEvent");

	debugAction = actionCollection()->addAction(
	    QStringLiteral("start-debug"),
	    new QAction(QIcon::fromTheme(QStringLiteral("debug-run")), i18n("Debug"), nullptr));
	actionCollection()->setDefaultShortcut(debugAction, Qt::Key_F8);
	connect(debugAction, &QAction::triggered, this, &DebuggerPlugin::debugStart);

	toggleBreakpointAction = actionCollection()->addAction(
	    QStringLiteral("toggle-breakpoint"),
	    new QAction(QIcon::fromTheme(QStringLiteral("draw-donut")), i18n("Toggle Breakpoint"), nullptr));
	actionCollection()->setDefaultShortcut(toggleBreakpointAction, Qt::Key_F9);
	connect(toggleBreakpointAction, &QAction::triggered, this, &DebuggerPlugin::toggleBreakpoint);

	stepAction = actionCollection()->addAction(
	    QStringLiteral("debug-step"),
	    new QAction(QIcon::fromTheme(QStringLiteral("debug-step-over")), i18n("Step Over"), nullptr));
	actionCollection()->setDefaultShortcut(stepAction, Qt::Key_F10);

	stepOutAction = actionCollection()->addAction(
	    QStringLiteral("debug-step-out"),
	    new QAction(QIcon::fromTheme(QStringLiteral("debug-step-out")), i18n("Step Out"), nullptr));
	actionCollection()->setDefaultShortcut(stepOutAction, Qt::Key_F12);

	stepInAction = actionCollection()->addAction(
	    QStringLiteral("debug-step-into"),
	    new QAction(QIcon::fromTheme(QStringLiteral("debug-step-into")), i18n("Step Into"), nullptr));
	actionCollection()->setDefaultShortcut(stepInAction, Qt::Key_F11);

	continueAction = actionCollection()->addAction(
	    QStringLiteral("debug-continue"),
	    new QAction(QIcon::fromTheme(QStringLiteral("media-playback-start")), i18n("Continue"), nullptr));
	actionCollection()->setDefaultShortcut(continueAction, Qt::Key_F5);
	connect(continueAction, &QAction::triggered, this, &DebuggerPlugin::continueDebugging);

	mBreakpointView = new BreakpointView();
	mVariableView   = new VariableView();
	
	connect(&MainWindow::instance()->tabView(), &FunctionTabView::functionViewChanged, this, [this](FunctionView* view, bool isNew) {
		
		// only connect to it if it was just opened
		if (!isNew)  {
			return;
		}
		
		connect(&view->scene(), &QtNodes::FlowScene::connectionHovered, this, [this, view](QtNodes::Connection& conn, const QPoint& point) {
			
			// only print value when it's stopped
			if (!stopped()) {
				return;
			}
			
			// get the node and data output ID from connection
			
			auto leftGuiNode = conn.getNode(QtNodes::PortType::Out);
			if (leftGuiNode == nullptr) {
				return;
			}
			
			auto leftChiNode = view->chiNodeFromGuiNode(leftGuiNode);
			if (leftChiNode == nullptr) {
				return;
			}
			
			// this is the abosolute ID, as used in nodeeditor. Execs first, then datas
			auto absConnID = conn.getPortIndex(QtNodes::PortType::Out);
			
			// if it's an exec output, then return
			if (!leftChiNode->type().pure() && absConnID <= leftChiNode->outputExecConnections.size()) {
				return;
			}
			
			auto dataConnID = leftChiNode->type().pure() ? absConnID : absConnID - leftChiNode->outputExecConnections.size();
			
			// get the LLDB value
			auto value = mDebugger->inspectNodeOutput(*leftChiNode, dataConnID);
			if (!value.IsValid()) {
				return;
			}
			
			// display it
			QToolTip::showText(point, QString::fromLatin1(value.GetValue()) + " : " + QString::fromLatin1(value.GetSummary()));
		});
		
		
	});

	setXMLFile("chigraphguidebuggerui.rc");
}

void DebuggerPlugin::toggleBreakpoint() {
	auto currentView = MainWindow::instance()->tabView().currentView();

	if (currentView == nullptr) { return; }

	for (auto node : currentView->selectedNodes()) { mBreakpointView->addBreakpoint(*node); }
}

void DebuggerPlugin::debugStart() {
	boost::filesystem::path chiPath =
	    boost::filesystem::path(QApplication::applicationFilePath().toStdString()).parent_path() /
	    "chi";
#ifdef _WIN32
	chiPath.replace_extension(".exe");
#endif

	MainWindow* window = MainWindow::instance();

	mDebugger = nullptr;

	// delete it
	if (mThread) { mThread->wait(); }
	mEventListener = nullptr;

	auto currentConfig = window->launchManager().currentConfiguration();
	if (!currentConfig.valid()) { return; }

	auto pair = window->loadModule(currentConfig.module());

	if (!pair.first || !pair.second) {
		KMessageBox::detailedError(window, i18n("Failed to load module"),
		                           QString::fromStdString(pair.first.dump()), i18n("run: error"));
		return;
	}

	// TODO: this really really needs a fix
	mDebugger = std::make_shared<chi::Debugger>(chiPath.string().c_str(), *pair.second);

	mThread        = new QThread;
	mEventListener = std::make_unique<DebuggerWorkerThread>(mDebugger);
	mEventListener->moveToThread(mThread);

	connect(mThread, &QThread::started, mEventListener.get(), &DebuggerWorkerThread::process);
	connect(mEventListener.get(), &DebuggerWorkerThread::eventOccured, this,
	        [this, window](lldb::SBEvent ev) {
		        if (lldb::SBProcess::GetStateFromEvent(ev) == lldb::eStateStopped) {
					variableView().setDisabled(false);
					mStopped = true;
			        variableView().setFrame(
			            mDebugger->lldbProcess().GetSelectedThread().GetSelectedFrame());

			        // get the node
			        auto node = mDebugger->nodeFromFrame(
			            mDebugger->lldbProcess().GetSelectedThread().GetSelectedFrame());
			        if (node == nullptr) {  return; }
			        
			        window->tabView().centerOnNode(*node); 
			        
					// clear the last triangle
					if (mCurrentNodeDecorator != nullptr && mCurrentNode != nullptr) {
						static_cast<ChigraphNodeModel&>(*mCurrentNode->nodeDataModel()).removeDecorator(mCurrentNodeDecorator);
					}
					
			        // draw a triangle above it
			        auto guiNode = window->tabView().currentView()->guiNodeFromChiNode(node);
					if (guiNode == nullptr) {
						return;
					}
					mCurrentNode = guiNode;
					
					
					auto& nodeModel = static_cast<ChigraphNodeModel&>(*guiNode->nodeDataModel());
					
			        mCurrentNodeDecorator = nodeModel.addDecorator(std::make_unique<CurrentNodeDecorator>());
					
		        } else {
					variableView().setDisabled(true);
					mStopped = false;
				}
		    });

	mThread->start();

	// set breakpoints
	for (const auto& bp : breakpointView().breakpoints()) { mDebugger->setBreakpoint(*bp.first); }

	auto res = mDebugger->start();
	if (!res) {
		qDebug() << QString::fromStdString(res.dump());
		return;
	}
}

void DebuggerPlugin::continueDebugging() {
	if (mDebugger != nullptr) { mDebugger->processContinue(); }
}
