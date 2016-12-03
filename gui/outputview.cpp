#include "outputview.hpp"

#include <QLabel>
#include <QVBoxLayout>

#include <KLocalizedString>
#include <QPlainTextEdit>

OutputView::OutputView()
{
	auto layout = new QVBoxLayout;
	setLayout(layout);

	// create label
	auto label = new QLabel;
	label->setText(i18n("Output"));

	// create text view
	textarea = new QPlainTextEdit;
	textarea->setReadOnly(true);

	layout->addWidget(label);
	layout->addWidget(textarea);
}

void OutputView::setProcess(QProcess* p)
{
	textarea->clear();

	connect(p, &QProcess::readyReadStandardOutput, this,
		[p, this] { textarea->appendPlainText(p->readAllStandardOutput().constData()); });

	connect(p, &QProcess::readyReadStandardOutput, this, [p, this] {
		textarea->appendHtml("<span style='color:red'>" +
							 QString(p->readAllStandardOutput().constData()).toHtmlEscaped() +
							 "</span>");
	});
}
