#pragma once

#ifndef CHIGGUI_SUBPROCESS_OUTPUT_VIEW_HPP
#define CHIGGUI_SUBPROCESS_OUTPUT_VIEW_HPP

#include <QPlainTextEdit>
#include <QProcess>

#include <chig/JsonModule.hpp>

class SubprocessOutputView : public QPlainTextEdit {
	Q_OBJECT
public:
	SubprocessOutputView(chig::JsonModule* module);
	void cancelProcess();

	chig::JsonModule* module() const { return mModule; }
	bool			  running() const { return mProcess->state() == QProcess::Running; }
signals:
	void processFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
	chig::JsonModule* mModule;
	QProcess*		  mProcess;
};

#endif  // CHIGGUI_SUBPROCESS_OUTPUT_VIEW_HPP
