#include "execparamlistwidget.hpp"

#include <QPushButton>
#include <QLabel>
#include <QInputDialog>
#include <QVBoxLayout>

#include <KLocalizedString>

ExecParamListWidget::ExecParamListWidget(QString title, QWidget* parent) : QWidget(parent) {

	
	auto layout = new QVBoxLayout;
	setLayout(layout);

	auto labelAndButtons = new QWidget;
	{
		auto buttLayout = new QHBoxLayout;
		labelAndButtons->setLayout(buttLayout);

		buttLayout->addWidget(new QLabel(std::move(title)));

		auto button = new QPushButton(QIcon::fromTheme(QStringLiteral("list-add")), QString());
		connect(button, &QPushButton::clicked, this, [this] {

			if (mMod == nullptr) { return; }

			bool ok;
			QString        name = QInputDialog::getText(this, i18n("Exec Name"), i18n("Name"),
	                                  QLineEdit::Normal, {}, &ok);
			
			if(!ok) { return; }
			
			auto selected = mParamList->selectedItems();

			if (selected.size() != 0) {
				addExec(name, mParamList->row(selected[0]));
			} else {
				addExec(name, mParamList->count() - 1);
			}

		});
		buttLayout->addWidget(button);

		button = new QPushButton(QIcon::fromTheme(QStringLiteral("list-remove")), QString());
		connect(button, &QPushButton::clicked, this, [this] {

			auto selected = mParamList->selectedItems();

			if (selected.size() != 0) { deleteExec(mParamList->row(selected[0])); }
		});
		buttLayout->addWidget(button);
	}
	layout->addWidget(labelAndButtons);

	mParamList = new QListWidget;
	layout->addWidget(mParamList);
	mParamList->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(mParamList, &QListWidget::doubleClicked, this, [this](QModelIndex idx) {
		int row = idx.row();

		bool ok;
		QString        name = QInputDialog::getText(this, i18n("Exec Name"), i18n("Name"),
	                                  QLineEdit::Normal, {}, &ok);
		if(!ok) { return; }
		
		modifyExec(row, name);

	});
}

QString ExecParamListWidget::nameForIdx(int idx) const {
	auto param = mParamList->item(idx);
	if (param == nullptr) { return ""; }

	return param->text();
}

void ExecParamListWidget::addExec(const QString& name, int after) {
	mParamList->insertItem(after + 1, name);
	mParamList->item(after + 1)->setBackgroundColor(QColor::fromRgb(20, 20, 30));
	execAdded(name);
}

void ExecParamListWidget::deleteExec(int idx) {
	execDeleted(idx);
	mParamList->takeItem(idx);	
}

void ExecParamListWidget::modifyExec(int idx, const QString& name) {
	auto param = mParamList->item(idx);
	if (param == nullptr) { return; }

	param->setText(name);

	execChanged(idx, name);
}

int ExecParamListWidget::execCount() const { return mParamList->count(); }
