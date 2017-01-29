#pragma once

#ifndef CHIGGUI_SUBPROCESS_OUTPUT_VIEW_HPP
#define CHIGGUI_SUBPROCESS_OUTPUT_VIEW_HPP

#include <QPlainTextEdit>
#include <QProcess>

#include <chig/GraphModule.hpp>

class SubprocessOutputView : public QPlainTextEdit {
	Q_OBJECT
public:
	SubprocessOutputView(chig::GraphModule* module);
	void cancelProcess();

	chig::GraphModule* module() const { return mModule; }
	bool              running() const { return mProcess != nullptr && mProcess->state() == QProcess::Running; }
signals:
	void processFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
	chig::GraphModule* mModule;
	QProcess*         mProcess = nullptr;
};

#endif  // CHIGGUI_SUBPROCESS_OUTPUT_VIEW_HPP
