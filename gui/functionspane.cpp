#include "functionspane.hpp"

#include "mainwindow.hpp"

#include <chig/GraphFunction.hpp>

#include <gsl/gsl_assert>

#include <QMenu>

#include <KLocalizedString>

class FunctionListItem : public QListWidgetItem {
public:
	FunctionListItem(chig::GraphFunction* func)
	    : QListWidgetItem(QIcon::fromTheme(QStringLiteral("code-class")),
	                      QString::fromStdString(func->module().fullName() + ":" + func->name()),
	                      nullptr, QListWidgetItem::UserType),
	      mFunc{func} {}

	chig::GraphFunction* mFunc;
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

			                      chig::GraphModule& mod = casted->mFunc->module();
			                      mod.removeFunction(casted->mFunc);

			                      updateModule(mod);

			                  });  // TODO: shortcut
		contextMenu.exec(global);
	});
}

void FunctionsPane::updateModule(chig::GraphModule& mod) {
	clear();

	// go through functions
	for (auto& fun : mod.functions()) { addItem(new FunctionListItem(fun.get())); }
}

void FunctionsPane::selectItem(QListWidgetItem* newitem) {
	Expects(newitem->type() == QListWidgetItem::UserType);

	auto casted = dynamic_cast<FunctionListItem*>(newitem);
	if (casted == nullptr) { return; }

	functionSelected(casted->mFunc);
}
