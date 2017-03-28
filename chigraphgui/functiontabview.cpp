#include "functiontabview.hpp"

#include <KActionCollection>

#include <QDebug>

#include <chi/Context.hpp>
#include <chi/GraphFunction.hpp>
#include <chi/Result.hpp>

#include "functionview.hpp"

FunctionTabView::FunctionTabView(QWidget* owner) : QTabWidget{owner} {
	auto closeAction =
	    actionCollection()->addAction(KStandardAction::Close, QStringLiteral("close-function"));
	connect(closeAction, &QAction::triggered, this, &FunctionTabView::closeTab);

	connect(this, &QTabWidget::tabCloseRequested, this, &FunctionTabView::closeTab);

	setXMLFile("chigraphfunctiontabviewui.rc");
}

void FunctionTabView::selectNewFunction(chi::GraphFunction& func) {
	QString qualifiedFunctionName =
	    QString::fromStdString(func.module().fullName() + ":" + func.name());

	// see if it's already open
	auto funcViewIter = mOpenFunctions.find(qualifiedFunctionName);
	if (funcViewIter != mOpenFunctions.end()) {
		setCurrentWidget(funcViewIter->second);
		return;
	}
	// if it's not already open, we'll have to create our own

	auto view                             = new FunctionView(func, this);
	int  idx                              = addTab(view, qualifiedFunctionName);
	mOpenFunctions[qualifiedFunctionName] = view;
	setTabText(idx, qualifiedFunctionName);
	setCurrentWidget(view);

	connect(view, &FunctionView::dirtied, this, [ this, mod = &func.module() ] { dirtied(*mod); });
	connect(view, &FunctionView::functionDoubleClicked, this, &FunctionTabView::selectNewFunction);

	functionOpened(view);
}


void FunctionTabView::centerOnNode(chi::NodeInstance& inst) {
	// load the function
	selectNewFunction(inst.function());
	
	// center on the node
	currentView()->centerOnNode(inst);
}

void FunctionTabView::selectNode(chi::NodeInstance& inst) {
	auto view = viewFromFunction(inst.function());
	
	if (!view) {
		return;
	}
	view->selectNode(inst);
	
}


void FunctionTabView::refreshModule(chi::GraphModule& mod) {
	int currTabId = currentIndex();

	// close the function views
	std::vector<std::pair<std::string, int>> functionNames;
	for (const auto& pair : mOpenFunctions) {
		if (&pair.second->function()->module() == &mod) {
			auto idx = indexOf(pair.second);
			functionNames.emplace_back(pair.second->function()->name(), idx);
			removeTab(idx);
		}
	}
	std::string fullName = mod.fullName();
	mod.context().unloadModule(fullName);

	chi::ChiModule* cMod;
	mod.context().loadModule(fullName, chi::LoadSettings::Default, &cMod);
	chi::GraphModule* gMod = dynamic_cast<chi::GraphModule*>(cMod);

	// re-add the tabs in reverse order to keep the ids
	for (auto iter = functionNames.rbegin(); iter != functionNames.rend(); ++iter) {
		chi::GraphFunction* func = gMod->functionFromName(iter->first);
		QString             qualifiedFunctionName =
		    QString::fromStdString(gMod->fullName() + ":" + func->name());
		auto view = new FunctionView(*func);
		insertTab(iter->second, view, qualifiedFunctionName);
		mOpenFunctions[qualifiedFunctionName] = view;

		connect(view, &FunctionView::dirtied, this,
		        [ this, mod = &func->module() ] { dirtied(*mod); });
		connect(view, &FunctionView::functionDoubleClicked, this,
		        &FunctionTabView::selectNewFunction);
	}

	setCurrentIndex(currTabId);
}


FunctionView* FunctionTabView::viewFromFunction(chi::GraphFunction& func) {
	return viewFromName(func.module().fullName(), func.name());
}

FunctionView* FunctionTabView::viewFromName(const QString& fullName) {
	auto iter = mOpenFunctions.find(fullName);
	if (iter != mOpenFunctions.end()) { return iter->second; }
	return nullptr;
}

FunctionView* FunctionTabView::viewFromName(const boost::filesystem::path& mod,
                                            const std::string&             function) {
	return viewFromName(QString::fromStdString(mod.string() + ":" + function));
}

void FunctionTabView::closeView(FunctionView* view) { closeTab(indexOf(view)); }

FunctionView* FunctionTabView::currentView() {
	auto widget = currentWidget();
	
	auto casted = qobject_cast<FunctionView*>(widget);
	
	if (casted == nullptr) {
		return nullptr;
	}
	
	return casted;
}

void FunctionTabView::closeTab(int idx) {
	mOpenFunctions.erase(
	    std::find_if(mOpenFunctions.begin(), mOpenFunctions.end(),
	                 [this, idx](auto& p) { return p.second == this->widget(idx); }));
	removeTab(idx);
}
