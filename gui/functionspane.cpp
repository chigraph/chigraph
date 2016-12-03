#include "functionspane.hpp"

#include "mainwindow.hpp"

#include <chig/GraphFunction.hpp>

#include <gsl/gsl_assert>

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
	for (auto& fun : mod->functions) {
		new QListWidgetItem(QIcon::fromTheme(QStringLiteral("code-class")),
			QString::fromStdString(fun->graphName), this);
	}
}

void FunctionsPane::selectItem(QListWidgetItem* item) { functionSelected(item->text()); }
