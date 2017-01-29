#pragma once

#ifndef CHIGGUI_PARAMLISTWIDGET_HPP
#define CHIGGUI_PARAMLISTWIDGET_HPP

#include <QListWidget>
#include <QWidget>

#include <chig/DataType.hpp>
#include <chig/GraphModule.hpp>

class ParamListWidget : public QWidget {
	Q_OBJECT

	friend class ParamListItem;

public:
	ParamListWidget(QString title, QWidget* parent = nullptr);

	chig::DataType typeForIdx(int idx) const;
	QString nameForIdx(int idx) const;

	void addParam(const chig::DataType& type, const QString& name, int after);
	void deleteParam(int idx);

	void modifyParam(int idx, const chig::DataType& type, const QString& name);

	int paramCount() const;

	void clear() { mParamList->clear(); }
	void setModule(chig::GraphModule* mod) { mMod = mod; }
signals:

	void paramChanged(int idx, const chig::DataType& newType, const QString& newName);

	void paramAdded(const chig::DataType& type, const QString& name);
	void paramDeleted(int idx);

private:
	QListWidget*       mParamList;
	chig::GraphModule* mMod = nullptr;
};

#endif  // CHIGGUI_PARAMLISTWIDGET_HPP
