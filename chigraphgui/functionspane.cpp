#include "functionspane.hpp"

#include "mainwindow.hpp"
#include "functiontabview.hpp"

#include <chi/GraphFunction.hpp>

#include <gsl/gsl_assert>

#include <QMenu>

#include <KLocalizedString>

class FunctionListItem : public QListWidgetItem {
public:
	explicit FunctionListItem(chi::GraphFunction* func)
	    : QListWidgetItem(QIcon::fromTheme(QStringLiteral("code-class")),
	                      QString::fromStdString(func->module().fullName() + ":" + func->name()),
	                      nullptr, QListWidgetItem::UserType),
	      mFunc{func} {}

	chi::GraphFunction* mFunc;
};

FunctionsPane::FunctionsPane(QWidget* parent) : QListWidget(parent) {
	setContextMenuPolicy(Qt::CustomContextMenu);

	connect(this, &QListWidget::itemDoubleClicked, this, &FunctionsPane::selectItem);
	connect(this, &QWidget::customContextMenuRequested, this, [this](QPoint p) {
		QPoint global = mapToGlobal(p);

		QListWidgetItem* funcItem = item(indexAt(p).row());

		// if we didn't right click on an item, then don't do anything
		if (funcItem == nullptr) { return; }

		QMenu contextMenu;
		contextMenu.addAction(QIcon::fromTheme(QStringLiteral("edit-delete")), i18n("Delete"),
		                      [this, p, funcItem] {

			                      Expects(funcItem->type() == QListWidgetItem::UserType);

			                      auto casted = dynamic_cast<FunctionListItem*>(funcItem);
			                      if (casted == nullptr) { return; }

			                      chi::GraphModule& mod = casted->mFunc->module();
								  
								  deleteFunction(*casted->mFunc);


			                  });  // TODO: shortcut
		contextMenu.exec(global);
	});
}

void FunctionsPane::updateModule(chi::GraphModule& mod) {
	clear();

	// go through functions
	for (auto& fun : mod.functions()) { addItem(new FunctionListItem(fun.get())); }
}

void FunctionsPane::selectItem(QListWidgetItem* newitem) {
	Expects(newitem->type() == QListWidgetItem::UserType);

	auto casted = dynamic_cast<FunctionListItem*>(newitem);
	if (casted == nullptr) { return; }

	functionSelected(*casted->mFunc);
}

void FunctionsPane::deleteFunction(chi::GraphFunction& func)
{
	// close the tab if it's open
	auto& tabView = MainWindow::instance()->tabView();
	
	auto openView = tabView.viewFromName(func.module().fullName(), func.name());
	if (openView != nullptr) {
		tabView.closeView(openView);
	}
	
	// find references
	auto references = func.context().findInstancesOfType(func.module().fullName(), func.name());
	for (auto node : references) {
		auto view = tabView.viewFromName(node->module().fullName(), node->function().name());
		
		// if the function is loaded
		if (view != nullptr) {
			// delete it
			
			// get the guinode
			auto guiNode = view->guiNodeFromChigNode(node);
			if (guiNode != nullptr) {
				view->scene().removeNode(*guiNode);
			}
		}
	}
	
	auto& module = func.module();
	
	// delete the function
	module.removeFunction(func);
	
	updateModule(module);
}

