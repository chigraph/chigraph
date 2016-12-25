#pragma once

#ifndef CHIGGUI_PARAMLISTWIDGET_HPP
#define CHIGGUI_PARAMLISTWIDGET_HPP

#include <QWidget>
#include <QListWidget>

#include <chig/DataType.hpp>
#include <chig/JsonModule.hpp>

class ParamListWidget : public QWidget {
    Q_OBJECT

    friend class ParamListItem;
public:

    ParamListWidget(QString title, QWidget* parent = nullptr);

    chig::DataType typeForIdx(int idx) const;
    QString nameForIdx(int idx) const;

    void addParam(chig::DataType type, QString name, int after);
    void deleteParam(int idx);

    void modifyParam(int idx, chig::DataType type, QString name);

    int paramCount() const;

    void clear() {
        mParamList->clear();
    }

    void setModule(chig::JsonModule* mod) {
        mMod = mod;
    }

signals:

    void paramChanged(int idx, chig::DataType newType, QString newName);

    void paramAdded(chig::DataType type, QString name);
    void paramDeleted(int idx);

private:
    QListWidget* mParamList;
    chig::JsonModule* mMod = nullptr;


};

#endif // CHIGGUI_PARAMLISTWIDGET_HPP
