#include "subprocessoutputview.hpp"

#include <QApplication>
#include <QLabel>
#include <QPlainTextEdit>
#include <QVBoxLayout>

#include <KLocalizedString>
#include <KMessageBox>

#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 9
#include <llvm/Bitcode/ReaderWriter.h>
#else
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#endif

#include <llvm/Support/raw_ostream.h>

#include <boost/filesystem.hpp>

#include <chi/Result.hpp>

SubprocessOutputView::SubprocessOutputView(chi::GraphModule* module) : mModule(module) {
	// compile!
	std::unique_ptr<llvm::Module> llmod;
	chi::Result                   res = module->context().compileModule(module->fullName(), &llmod);

	if (!res) {
		KMessageBox::detailedError(this, "Failed to compile module",
		                           QString::fromStdString(res.dump()));

		return;
	}

	std::string str;
	{
		llvm::raw_string_ostream os(str);

		llvm::WriteBitcodeToFile(llmod.get(), os);
	}

	setReadOnly(true);

	boost::filesystem::path chiPath =
	    boost::filesystem::path(QApplication::applicationFilePath().toStdString()).parent_path() /
	    "chi";
#ifdef _WIN32
	chiPath.replace_extension(".exe");
#endif

	assert(boost::filesystem::is_regular_file(chiPath));

	// run in lli
	mProcess = new QProcess(this);
	mProcess->setProgram(QString::fromStdString(chiPath.string()));
	mProcess->setArguments(QStringList(QStringLiteral("interpret")));

	connect(mProcess, &QProcess::readyReadStandardOutput, this,
	        [this] { appendPlainText(mProcess->readAllStandardOutput().constData()); });

	connect(mProcess, &QProcess::readyReadStandardOutput, this, [this] {
		appendHtml("<span style='color:red'>" +
		           QString(mProcess->readAllStandardOutput().constData()).toHtmlEscaped() +
		           "</span>");
	});

	connect(mProcess,
	        static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this,
	        &SubprocessOutputView::processFinished);

	mProcess->start();
	mProcess->write(str.c_str(), str.length());
	mProcess->closeWriteChannel();
}

void SubprocessOutputView::cancelProcess() {
	if (mProcess != nullptr) { mProcess->kill(); }
}
