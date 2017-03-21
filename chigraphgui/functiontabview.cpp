#include "functiontabview.hpp"

#include <KActionCollection>

FunctionTabView::FunctionTabView(QWidget* owner) : QTabWidget{owner} {
	auto closeAction = actionCollection()->addAction(KStandardAction::Close, QStringLiteral("close-function"));
	connect(closeAction, &QAction::triggered, this, [this]{ removeTab(currentIndex()); });
	
	setXMLFile("chigraphfunctiontabviewui.rc");
}
