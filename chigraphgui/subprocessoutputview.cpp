#include "subprocessoutputview.hpp"

#include <QApplication>
#include <QLabel>
#include <QPlainTextEdit>
#include <QVBoxLayout>

#include <KLocalizedString>
#include <KMessageBox>

#include <llvm/Config/llvm-config.h>
#if LLVM_VERSION_MAJOR <= 3
#include <llvm/Bitcode/ReaderWriter.h>
#else
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#endif

#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>

#include <boost/filesystem.hpp>

#include <chi/Context.hpp>
#include <chi/Result.hpp>

#include <iostream>

namespace fs = boost::filesystem;

SubprocessOutputView::SubprocessOutputView(chi::GraphModule* module) : mModule(module) {
	// compile!
	std::unique_ptr<llvm::Module> llmod;
	chi::Result                   res = module->context().compileModule(module->fullName(), &llmod);

	if (!res) {
		KMessageBox::detailedError(this, "Failed to compile module",
		                           QString::fromStdString(res.dump()));

		return;
	}

	// write it to a temporary file
	fs::path tempBitcodeFile =
	    boost::filesystem::temp_directory_path() / fs::unique_path().replace_extension(".bc");
	{
		std::error_code      err;
		llvm::raw_fd_ostream os(tempBitcodeFile.string(), err, llvm::sys::fs::F_RW);

		llvm::WriteBitcodeToFile(llmod.get(), os);
	}

	setReadOnly(true);

	boost::filesystem::path chiPath =
	    boost::filesystem::path(QApplication::applicationFilePath().toStdString()).parent_path() /
	    "chi";
#ifdef _WIN32
	chiPath.replace_extension(".exe");
#endif

	Q_ASSERT(boost::filesystem::is_regular_file(chiPath));
	std::cout << chiPath.string() << std::endl;

	// run in lli
	mProcess = new QProcess(this);
	mProcess->setProgram(QString::fromStdString(chiPath.string()));

	auto args = QStringList() << QStringLiteral("interpret") << QStringLiteral("-i")
	                          << QString::fromStdString(tempBitcodeFile.string())
	                          << QStringLiteral("-O2");
	mProcess->setArguments(args);

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
}

void SubprocessOutputView::cancelProcess() {
	if (mProcess != nullptr) { mProcess->kill(); }
}
