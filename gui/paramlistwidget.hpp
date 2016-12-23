#pragma once

#ifndef CHIGGUI_PARAMLISTWIDGET_HPP
#define CHIGGUI_PARAMLISTWIDGET_HPP

#include <QWidget>
#include <QListWidget>

#include <chig/DataType.hpp>

class ParamListWidget : public QWidget {
    Q_OBJECT

    friend class ParamListItem;
public:

    ParamListWidget(QString title, QWidget* parent = nullptr);

    chig::DataType typeForIdx(int idx) const;
    QString nameForIdx(int idx) const;

    void addParam(chig::DataType type, QString name);
    void deleteParam(int idx);

    int paramCount() const;

    void clear() {
        mParamList->clear();
    }

signals:

    void paramTypeChanged(int idx, chig::DataType newType);
    void paramNameChanged(int idx, QString newName);

    void paramAdded(chig::DataType type, QString name);
    void paramDeleted(int idx);

private:
    QListWidget* mParamList;


};

#endif // CHIGGUI_PARAMLISTWIDGET_HPP
