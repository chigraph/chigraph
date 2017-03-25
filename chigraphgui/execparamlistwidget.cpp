#include "execparamlistwidget.hpp"

#include <QComboBox>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>

#include <KMessageBox>

#include <chi/GraphFunction.hpp>
#include <chi/Context.hpp>

#include "functionview.hpp"

namespace {

QStringList createTypeOptions(const chi::GraphModule& mod) {
	QStringList ret;

	// add the module
	for (const auto& ty : mod.typeNames()) {
		ret << QString::fromStdString(mod.fullName() + ":" + ty);
	}

	// and its dependencies
	for (auto dep : mod.dependencies()) {
		auto depMod = mod.context().moduleByFullName(dep);
		for (const auto& type : depMod->typeNames()) {
			ret << QString::fromStdString(depMod->fullName() + ":" + type);
		}
	}
	return ret;
}

}  // anon namespace

ExecParamListWidget::ExecParamListWidget(QWidget* parent) : QWidget(parent) {}

void ExecParamListWidget::setFunction(FunctionView* func, Type ty) {
	mFunc = func;
	mType = ty;

	if (layout()) { deleteLayout(layout()); }

	auto layout = new QGridLayout;
	setLayout(layout);

	// populate it
	auto& typeVec =
	    ty == Input ? mFunc->function()->execInputs() : mFunc->function()->execOutputs();

	auto id = 0;
	for (const auto& param : typeVec) {
		auto edit = new QLineEdit;
		edit->setText(QString::fromStdString(param));
		connect(edit, &QLineEdit::textChanged, this, [this, id](const QString& newText) {
			if (mType == Input) {
				mFunc->function()->renameExecInput(id, newText.toStdString());
				refreshEntry();
				dirtied();
			} else {
				mFunc->function()->renameExecOutput(id, newText.toStdString());
				refreshExits();
				dirtied();
			}
		});
		layout->addWidget(edit, id, 0);

		auto deleteButton = new QPushButton(QIcon::fromTheme(QStringLiteral("list-remove")), {});
		connect(deleteButton, &QAbstractButton::clicked, this, [this, id](bool) {
			if (mType == Input) {
				mFunc->function()->removeExecInput(id);
				refreshEntry();
				setFunction(mFunc, mType);
				dirtied();
			} else {
				mFunc->function()->removeExecOutput(id);
				refreshExits();
				setFunction(mFunc, mType);
				dirtied();
			}
		});
		layout->addWidget(deleteButton, id, 1);

		++id;
	}

	// create the "new" button
	auto newButton = new QPushButton(QIcon::fromTheme("list-add"), {});
	newButton->setSizePolicy({QSizePolicy::Maximum, QSizePolicy::Maximum});
	connect(newButton, &QAbstractButton::clicked, this, [this](bool) {
		if (mType == Input) {
			mFunc->function()->addExecInput("");
			refreshEntry();
			dirtied();

			setFunction(mFunc, mType);  // TODO: not the most efficient way...
		} else {
			mFunc->function()->addExecOutput("");
			refreshExits();
			dirtied();

			setFunction(mFunc, mType);
		}

	});
	layout->addWidget(newButton, id, 1, Qt::AlignRight);
}

void ExecParamListWidget::refreshEntry() {
	auto entry = mFunc->function()->entryNode();
	if (entry == nullptr) { return; }
	mFunc->refreshGuiForNode(mFunc->guiNodeFromChigNode(entry));
}
void ExecParamListWidget::refreshExits() {
	for (const auto& exit : mFunc->function()->nodesWithType("lang", "exit")) {
		mFunc->refreshGuiForNode(mFunc->guiNodeFromChigNode(exit));
	}

	mFunc->refreshRegistry();
}
