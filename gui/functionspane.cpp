#include "functionspane.hpp"

#include "mainwindow.hpp"

#include <chig/GraphFunction.hpp>

#include <gsl/gsl_assert>

class FunctionListItem : public QListWidgetItem {
public:
    FunctionListItem(chig::GraphFunction* func) : QListWidgetItem(QIcon::fromTheme(QStringLiteral("code-class")), 
        QString::fromStdString(func->module().fullName() + ":" + func->name()), nullptr, QListWidgetItem::UserType), mFunc{func} {
    }
     
    chig::GraphFunction* mFunc;
};

FunctionsPane::FunctionsPane(QWidget* parent, MainWindow* win) : QListWidget(parent)
{
	connect(win, &MainWindow::openJsonModule, this, &FunctionsPane::updateModule);

	connect(this, &QListWidget::itemDoubleClicked, this, &FunctionsPane::selectItem);
}

void FunctionsPane::updateModule(chig::JsonModule* mod)
{
	Expects(mod != nullptr);

	clear();

	// go through functions
	for (auto& fun : mod->functions()) {
		addItem(new FunctionListItem(fun.get()));
	}
}

void FunctionsPane::selectItem(QListWidgetItem* newitem) { 
    
    Expects(newitem->type() == QListWidgetItem::UserType);
    
    functionSelected(static_cast<FunctionListItem*>(newitem)->mFunc);
    
}
