#include "paramlistwidget.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QListWidgetItem>
#include <QPushButton>

#include <KLocalizedString>
#include <KMessageBox>
#include <QInputDialog>
#include <chig/DataType.hpp>

#include <chig/GraphFunction.hpp>

class ParamListItem : public QListWidgetItem
{
public:
	ParamListItem(ParamListWidget* container, chig::DataType ty, QString name)
		: QListWidgetItem(nullptr, QListWidgetItem::UserType),
		  mDataType{ty},
		  mName{name},
		  mContainer{container}
	{
		updateName();
	}

	chig::DataType dataType() const { return mDataType; }
	QString name() const { return mName; }
	void setDataType(chig::DataType newDataType)
	{
		mDataType = newDataType;
		updateName();
	}

	void setName(QString newName)
	{
		mName = newName;
		updateName();
	}

private:
	int getIdx() const { return mContainer->mParamList->row(this); }
	void updateName()
	{
		setText(name() + " - " + QString::fromStdString(dataType().qualifiedName()));
	}

	chig::DataType mDataType;
	QString mName;
	ParamListWidget* mContainer;
};

ParamListItem* paramFromIdx(int idx, QListWidget* list)
{
	auto notcasted = list->item(idx);
	if (notcasted == nullptr || notcasted->type() != QListWidgetItem::UserType) {
		return nullptr;
	}

	auto casted = dynamic_cast<ParamListItem*>(notcasted);

	if (casted == nullptr) {
		return nullptr;
	}

	return casted;
}

QStringList createTypeOptions(chig::JsonModule* mod)
{
	QStringList ret;

	for (auto dep : mod->dependencies()) {
		auto depMod = mod->context().moduleByFullName(dep);
		for (auto type : depMod->typeNames()) {
			ret << QString::fromStdString(depMod->name() + ":" + type);
		}
	}
	return ret;
}

std::pair<chig::DataType, QString> getDataNamePair(QWidget* parent, chig::JsonModule* mod)
{
	auto qualtype =
		QInputDialog::getItem(parent, i18n("Select Type"), i18n("Type"), createTypeOptions(mod));
	auto name = QInputDialog::getText(parent, i18n("Parameter Name"), i18n("Name"));

	std::string module, type;
	std::tie(module, type) = chig::parseColonPair(qualtype.toStdString());
	chig::DataType dtype;
	chig::Result res = mod->context().typeFromModule(module, type, &dtype);
	if (!res) {
		KMessageBox::detailedError(
			parent, "Failed to get data type", QString::fromStdString(res.dump()), "Failure!");
		return {};
	}

	return {dtype, name};
}

ParamListWidget::ParamListWidget(QString title, QWidget* parent) : QWidget(parent)
{
	auto layout = new QVBoxLayout;
	setLayout(layout);

	auto labelAndButtons = new QWidget;
	{
		auto buttLayout = new QHBoxLayout;
		labelAndButtons->setLayout(buttLayout);

		buttLayout->addWidget(new QLabel(title));

		auto button = new QPushButton(QIcon::fromTheme(QStringLiteral("list-add")), QString());
		connect(button, &QPushButton::clicked, this, [this] {

			chig::DataType type;
			QString name;
			std::tie(type, name) = getDataNamePair(this, mMod);

			auto selected = mParamList->selectedItems();

			if (selected.size() != 0) {
				addParam(type, name, mParamList->row(selected[0]));
			} else {
				addParam(type, name, mParamList->count() - 1);
			}

		});
		buttLayout->addWidget(button);

		button = new QPushButton(QIcon::fromTheme(QStringLiteral("list-remove")), QString());
		connect(button, &QPushButton::clicked, this, [this] {

			auto selected = mParamList->selectedItems();

			if (selected.size() != 0) {
				deleteParam(mParamList->row(selected[0]));
			}
		});
		buttLayout->addWidget(button);
	}
	layout->addWidget(labelAndButtons);

	mParamList = new QListWidget;
	layout->addWidget(mParamList);
	mParamList->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(mParamList, &QListWidget::doubleClicked, this, [this](QModelIndex idx) {
		int row = idx.row();

		chig::DataType type;
		QString name;
		std::tie(type, name) = getDataNamePair(this, mMod);

		modifyParam(row, type, name);

	});
}

chig::DataType ParamListWidget::typeForIdx(int idx) const
{
	auto param = paramFromIdx(idx, mParamList);
	if (param == nullptr) {
		return {};
	}

	return param->dataType();
}

QString ParamListWidget::nameForIdx(int idx) const
{
	auto param = paramFromIdx(idx, mParamList);
	if (param == nullptr) {
		return "";
	}

	return param->name();
}

void ParamListWidget::addParam(chig::DataType type, QString name, int after)
{
	auto newItem = new ParamListItem(this, type, name);
	mParamList->insertItem(after + 1, newItem);
	paramAdded(type, name);
}

void ParamListWidget::deleteParam(int idx)
{
	paramDeleted(idx);
	mParamList->removeItemWidget(mParamList->item(idx));
}

void ParamListWidget::modifyParam(int idx, chig::DataType type, QString name)
{
	auto param = paramFromIdx(idx, mParamList);
	if (param == nullptr) {
		return;
	}

	param->setName(name);
	param->setDataType(type);

	paramChanged(idx, type, name);
}

int ParamListWidget::paramCount() const { return mParamList->count(); }
