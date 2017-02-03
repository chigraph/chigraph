#pragma once

#ifndef CHIGGUI_EXEC_PARAM_LIST_WIDGET_HPP
#define CHIGGUI_EXEC_PARAM_LIST_WIDGET_HPP

#include <QWidget>
#include <QListWidget>

#include <chig/Fwd.hpp>

class ExecParamListWidget : public QWidget {
	Q_OBJECT
public:
	ExecParamListWidget(QString title, QWidget* parent = nullptr);

	QString nameForIdx(int idx) const;

	void addExec(const QString& name, int after);
	void deleteExec(int idx);

	void modifyExec(int idx, const QString& name);

	int execCount() const;

	void clear() { mParamList->clear(); }
	void setModule(chig::GraphModule* mod) { mMod = mod; }
signals:

	void execChanged(int idx, const QString& newName);

	void execAdded(const QString& name);
	void execDeleted(int idx);

private:
	QListWidget*       mParamList;
	chig::GraphModule* mMod = nullptr;
};

#endif // CHIGGUI_EXEC_PARAM_LIST_WIDGET_HPP
