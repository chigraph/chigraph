#include "chignodegui.hpp"

#include "functionview.hpp"

class EditCodeDialog : public QDialog
{
public:
	EditCodeDialog(chig::NodeInstance* inst, FunctionView* fview)
	{
		setWindowTitle(i18n("Edit C Call Node"));

		auto layout = new QVBoxLayout;
		setLayout(layout);

		auto lineEdit = new QLineEdit;
		layout->addWidget(lineEdit);
		lineEdit->setText(QString::fromStdString(inst->type().toJSON()["function"]));

		auto textEdit = new KTextEdit;
		textEdit->setFontFamily("sourcecodepro");
		layout->addWidget(textEdit);
		textEdit->setText(QString::fromStdString(inst->type().toJSON()["code"]));

		auto okButton = new QPushButton;
		layout->addWidget(okButton);
		okButton->setText(i18n("Ok"));
		connect(okButton, &QPushButton::clicked, this, [this, textEdit, lineEdit, inst, fview] {
			std::string function = lineEdit->text().toStdString();
			std::string code = textEdit->toPlainText().toStdString();

			std::unique_ptr<chig::NodeType> ty;
			chig::Result res = inst->context().nodeTypeFromModule(
				"c", "func", {{"code", code}, {"function", function}}, &ty);
			if (!res) {
				KMessageBox::detailedError(
					this, "Failed to compile C node", QString::fromStdString(res.dump()));

				return;
			}
			inst->setType(std::move(ty));
			fview->refreshGuiForNode(fview->nodes[inst]);

			close();
		});
	}
};

unsigned int ChigNodeGui::nPorts(PortType portType) const
{
	if (portType == PortType::In) {
		return inst->type().execInputs().size() + inst->type().dataInputs().size();
	} else if (portType == PortType::Out) {
		return inst->type().execOutputs().size() + inst->type().dataOutputs().size();
	}

	return 1;  // ?
}

NodeDataType ChigNodeGui::dataType(PortType pType, PortIndex pIndex) const
{
	if (pType == PortType::In) {
		std::pair<std::string, std::string> idandname;
		if (pIndex >= int(inst->type().execInputs().size())) {
			if (pIndex - inst->type().execInputs().size() >= inst->type().dataInputs().size())
				return {};

			idandname = {inst->type()
							 .dataInputs()[pIndex - inst->type().execInputs().size()]
							 .first.qualifiedName(),
				inst->type().dataInputs()[pIndex - inst->type().execInputs().size()].second};

		} else {
			idandname = {"exec", inst->type().execInputs()[pIndex]};
		}
		return {QString::fromStdString(idandname.first), QString::fromStdString(idandname.second)};
	} else if (pType == PortType::Out) {
		std::pair<std::string, std::string> idandname;
		if (pIndex >= int(inst->type().execOutputs().size())) {
			auto dataOutput =
				inst->type().dataOutputs()[pIndex - inst->type().execOutputs().size()];
			idandname = {dataOutput.first.qualifiedName(), dataOutput.second};

		} else {
			idandname = {"exec", inst->type().execOutputs()[pIndex]};
		}
		return {QString::fromStdString(idandname.first), QString::fromStdString(idandname.second)};
	}

	return {};
}

QWidget* ChigNodeGui::embeddedWidget()
{
	if (inst->type().name() == "const-bool") {
		QCheckBox* box = new QCheckBox("");
		bool checked = inst->type().toJSON();
		box->setCheckState(checked ? Qt::Checked : Qt::Unchecked);

		connect(box, &QCheckBox::stateChanged, this, [this](int newState) {
			std::unique_ptr<chig::NodeType> newType;

			inst->context().nodeTypeFromModule(
				"lang", "const-bool", newState == Qt::Checked, &newType);

			inst->setType(std::move(newType));
		});

		box->setMaximumSize(box->sizeHint());
		return box;
	}
	if (inst->type().name() == "strliteral") {
		QLineEdit* edit = new QLineEdit();
		std::string s = inst->type().toJSON();
		edit->setText(QString::fromStdString(s));

		edit->setMaximumSize(edit->sizeHint());

		connect(edit, &QLineEdit::textChanged, this, [this](const QString& s) {
			std::unique_ptr<chig::NodeType> newType;

			inst->context().nodeTypeFromModule(
				"lang", "strliteral", s.toUtf8().constData(), &newType);

			inst->setType(std::move(newType));

		});

		return edit;
	}
	if (inst->type().name() == "const-int") {
		QLineEdit* edit = new QLineEdit();
		edit->setValidator(new QIntValidator);
		int val = inst->type().toJSON();
		edit->setText(QString::number(val));

		edit->setMaximumSize(edit->sizeHint());

		connect(edit, &QLineEdit::textChanged, this, [this](const QString& s) {
			std::unique_ptr<chig::NodeType> newType;

			inst->context().nodeTypeFromModule("lang", "const-int", s.toInt(), &newType);

			inst->setType(std::move(newType));

		});

		return edit;
	}
	if (inst->type().name() == "func") {
		QPushButton* butt = new QPushButton(i18n("Edit code"));
		connect(butt, &QPushButton::clicked, this, [this] {
			auto dialog = new EditCodeDialog(inst, fview);

			dialog->exec();
		});

		butt->setMaximumSize(butt->sizeHint());

		return butt;
	}

	return nullptr;
}
