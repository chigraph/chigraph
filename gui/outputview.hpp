#pragma once

#ifndef CHIGGUI_OUTPUT_VIEW_HPP
#define CHIGGUI_OUTPUT_VIEW_HPP

#include <QPlainTextEdit>
#include <QProcess>
#include <QWidget>

class OutputView : public QWidget
{
public:
	OutputView();

	void setProcess(QProcess* p);
    void cancelProcess();

private:
	QPlainTextEdit* textarea;
};

#endif  // CHIGGUI_OUTPUT_VIEW_HPP
