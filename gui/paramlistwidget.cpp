#include "paramlistwidget.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidgetItem>

#include <chig/DataType.hpp>

class ParamListItem : public QListWidgetItem {

public:
    ParamListItem(ParamListWidget* container, chig::DataType ty, QString name) :
        QListWidgetItem(nullptr, QListWidgetItem::UserType),
    mDataType{ty},
    mName{name},
    mContainer{container} {
        updateName();
    }

    chig::DataType dataType() const { return mDataType; }
    QString name() const { return mName; }

    void setDataType(chig::DataType newDataType) {
        mDataType = newDataType;
        updateName();

        mContainer->paramTypeChanged(getIdx(), newDataType);

    }

    void setName(QString newName) {
        mName = newName;
        updateName();

        mContainer->paramNameChanged(getIdx(), newName);
    }

private:

    int getIdx() const {
        return mContainer->mParamList->row(this);
    }

    void updateName() {
        setText(name() + " - " + QString::fromStdString(dataType().qualifiedName()));
    }

    chig::DataType mDataType;
    QString mName;
    ParamListWidget* mContainer;
};

ParamListItem* paramFromIdx(int idx, QListWidget* list) {
    auto notcasted = list->item(idx);
    if(notcasted == nullptr || notcasted->type() != QListWidgetItem::UserType) {
        return nullptr;
    }

    auto casted = dynamic_cast<ParamListItem*>(notcasted);

    if(casted == nullptr) {
        return nullptr;
    }

    return casted;
}

ParamListWidget::ParamListWidget(QString title, QWidget *parent) : QWidget(parent) {
    auto layout = new QVBoxLayout;
    setLayout(layout);

    auto labelAndButtons = new QWidget;
    {
        auto buttLayout = new QHBoxLayout;
        labelAndButtons->setLayout(buttLayout);

        buttLayout->addWidget(new QLabel(title));

        auto button = new QPushButton(QIcon::fromTheme(QStringLiteral("list-add")), QString());
        connect(button, &QPushButton::clicked, this, [this] {
            // TODO:
        });
        buttLayout->addWidget(button);

        button = new QPushButton(QIcon::fromTheme(QStringLiteral("list-remove")), QString());
        connect(button, &QPushButton::clicked, this, [this] {
            // TODO:
        });
        buttLayout->addWidget(button);
    }
    layout->addWidget(labelAndButtons);

    mParamList = new QListWidget;
    layout->addWidget(mParamList);



}

chig::DataType ParamListWidget::typeForIdx(int idx) const
{
    auto param = paramFromIdx(idx, mParamList);
    if(param == nullptr) {
        return {};
    }

    return param->dataType();
}

QString ParamListWidget::nameForIdx(int idx) const
{
    auto param = paramFromIdx(idx, mParamList);
    if(param == nullptr) {
        return "";
    }

    return param->name();
}

void ParamListWidget::addParam(chig::DataType type, QString name)
{
    mParamList->addItem(new ParamListItem(this, type, name));
    paramAdded(type, name);
}

void ParamListWidget::deleteParam(int idx)
{
    paramDeleted(idx);
    mParamList->removeItemWidget(mParamList->item(idx));

}

int ParamListWidget::paramCount() const
{
    return mParamList->count();
}
