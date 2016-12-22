#include "functionspane.hpp"

#include "mainwindow.hpp"

#include <chig/GraphFunction.hpp>

#include <gsl/gsl_assert>

#include <QMenu>

#include <KLocalizedString>

class FunctionListItem : public QListWidgetItem
{
public:
	FunctionListItem(chig::GraphFunction* func)
		: QListWidgetItem(QIcon::fromTheme(QStringLiteral("code-class")),
			  QString::fromStdString(func->module().fullName() + ":" + func->name()), nullptr,
			  QListWidgetItem::UserType),
		  mFunc{func}
	{
	}

	chig::GraphFunction* mFunc;
};

FunctionsPane::FunctionsPane(QWidget* parent, MainWindow* win) : QListWidget(parent)
{
	connect(win, &MainWindow::openJsonModule, this, &FunctionsPane::updateModule);

	connect(this, &QListWidget::itemDoubleClicked, this, &FunctionsPane::selectItem);

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &QWidget::customContextMenuRequested, this, [this](QPoint p) {
        QPoint global = mapToGlobal(p);

        QMenu contextMenu;
        contextMenu.addAction(QIcon::fromTheme(QStringLiteral("edit-delete")), i18n("Delete"), [this, p]{
            QListWidgetItem* funcItem = item(indexAt(p).row());

            if(!funcItem) {
                return;
            }

            Expects(funcItem->type() == QListWidgetItem::UserType);


            auto casted = dynamic_cast<FunctionListItem*>(funcItem);
            if(!casted) {
                return;
            }

            chig::JsonModule* mod = &casted->mFunc->module();
            casted->mFunc->module().removeFunction(casted->mFunc);

            updateModule(mod);

        }); // TODO: shortcut
        contextMenu.exec(global);
    });
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

void FunctionsPane::selectItem(QListWidgetItem* newitem)
{
	Expects(newitem->type() == QListWidgetItem::UserType);

	functionSelected(static_cast<FunctionListItem*>(newitem)->mFunc);
}
