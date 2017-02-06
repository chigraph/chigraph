<<<<<<< HEAD
#include "chigraphnodemodel.hpp"

#include "functionview.hpp"

class EditCodeDialog : public QDialog {
public:
	EditCodeDialog(chig::NodeInstance* inst, FunctionView* fview) {
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
			std::string code     = textEdit->toPlainText().toStdString();

			std::unique_ptr<chig::NodeType> ty;
			chig::Result                    res = inst->context().nodeTypeFromModule(
			    "c", "func", {{"code", code}, {"function", function}}, &ty);
			if (!res) {
				KMessageBox::detailedError(this, "Failed to compile C node",
				                           QString::fromStdString(res.dump()));

				return;
			}
			inst->setType(std::move(ty));
			fview->refreshGuiForNode(fview->guiNodeFromChigNode(inst));

			close();
		});
	}
};

unsigned int ChigraphNodeModel::nPorts(PortType portType) const {
	if (portType == PortType::In) {
		return mInst->type().execInputs().size() + mInst->type().dataInputs().size();
	}
	if (portType == PortType::Out) {
		return mInst->type().execOutputs().size() + mInst->type().dataOutputs().size();
	}

	return 1;  // ?
}

NodeDataType ChigraphNodeModel::dataType(PortType pType, PortIndex pIndex) const {
	if (pType == PortType::In) {
		std::pair<std::string, std::string> idandname;
		if (pIndex >= int(mInst->type().execInputs().size())) {
			if (pIndex - mInst->type().execInputs().size() >= mInst->type().dataInputs().size()) {
				return {};
			}

			idandname = {
			    mInst->type()
			        .dataInputs()[pIndex - mInst->type().execInputs().size()]
			        .type.qualifiedName(),
			    mInst->type().dataInputs()[pIndex - mInst->type().execInputs().size()].name};

		} else {
			idandname = {"exec", mInst->type().execInputs()[pIndex]};
		}
		return {QString::fromStdString(idandname.first), QString::fromStdString(idandname.second)};
	}
	if (pType == PortType::Out) {
		std::pair<std::string, std::string> idandname;
		if (pIndex >= int(mInst->type().execOutputs().size())) {
			auto dataOutput =
			    mInst->type().dataOutputs()[pIndex - mInst->type().execOutputs().size()];
			idandname = {dataOutput.type.qualifiedName(), dataOutput.name};

		} else {
			idandname = {"exec", mInst->type().execOutputs()[pIndex]};
		}
		return {QString::fromStdString(idandname.first), QString::fromStdString(idandname.second)};
	}

	return {};
}

QWidget* ChigraphNodeModel::embeddedWidget() {
	if (mInst->type().name() == "const-bool") {
		QCheckBox* box     = new QCheckBox("");
		bool       checked = mInst->type().toJSON();
		box->setCheckState(checked ? Qt::Checked : Qt::Unchecked);

		connect(box, &QCheckBox::stateChanged, this, [this](int newState) {
			std::unique_ptr<chig::NodeType> newType;

			mInst->context().nodeTypeFromModule("lang", "const-bool", newState == Qt::Checked,
			                                    &newType);

			mInst->setType(std::move(newType));
		});

		box->setMaximumSize(box->sizeHint());
		return box;
	}
	if (mInst->type().name() == "strliteral") {
		auto        edit = new QLineEdit();
		std::string s    = mInst->type().toJSON();
		edit->setText(QString::fromStdString(s));

		edit->setMaximumSize(edit->sizeHint());

		connect(edit, &QLineEdit::textChanged, this, [this](const QString& s) {
			std::unique_ptr<chig::NodeType> newType;

			mInst->context().nodeTypeFromModule("lang", "strliteral", s.toUtf8().constData(),
			                                    &newType);

			mInst->setType(std::move(newType));

		});

		return edit;
	}
	if (mInst->type().name() == "const-int") {
		auto edit = new QLineEdit();
		edit->setValidator(new QIntValidator);
		int val = mInst->type().toJSON();
		edit->setText(QString::number(val));

		edit->setMaximumSize(edit->sizeHint());

		connect(edit, &QLineEdit::textChanged, this, [this](const QString& s) {
			std::unique_ptr<chig::NodeType> newType;

			mInst->context().nodeTypeFromModule("lang", "const-int", s.toInt(), &newType);

			mInst->setType(std::move(newType));

		});

		return edit;
	}
	if (mInst->type().name() == "func") {
		QPushButton* butt = new QPushButton(i18n("Edit code"));
		connect(butt, &QPushButton::clicked, this, [this] {
			auto dialog = new EditCodeDialog(mInst, mFunctionView);

			dialog->exec();
		});

		butt->setMaximumSize(butt->sizeHint());

		return butt;
	}
	if (mInst->type().name() == "const-float") {
		auto edit = new QLineEdit();
		edit->setValidator(new QDoubleValidator);
		double val = mInst->type().toJSON();
		edit->setText(QString::number(val));

		edit->setMaximumSize(edit->sizeHint());

		connect(edit, &QLineEdit::textChanged, this, [this](const QString& s) {
			std::unique_ptr<chig::NodeType> newType;

			mInst->context().nodeTypeFromModule("lang", "const-float", s.toDouble(), &newType);

			mInst->setType(std::move(newType));

		});

		return edit;
	}

	return nullptr;
}
=======
#include "chigraphnodemodel.hpp"

#include "functionview.hpp"

#include "../src/NodeGraphicsObject.hpp"

class EditCodeDialog : public QDialog {
public:
	EditCodeDialog(chig::NodeInstance* inst, FunctionView* fview) {
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
			std::string code     = textEdit->toPlainText().toStdString();

			std::unique_ptr<chig::NodeType> ty;
			chig::Result                    res = inst->context().nodeTypeFromModule(
			    "c", "func", {{"code", code}, {"function", function}}, &ty);
			if (!res) {
				KMessageBox::detailedError(this, "Failed to compile C node",
				                           QString::fromStdString(res.dump()));

				return;
			}
			inst->setType(std::move(ty));

			close();

		});

		connect(this, &QDialog::accepted, this,
		        [fview, inst] { fview->refreshGuiForNode(fview->guiNodeFromChigNode(inst)); });
	}
};

ChigraphNodeModel::ChigraphNodeModel(chig::NodeInstance* inst_, FunctionView* fview_)
    : mInst{inst_}, mFunctionView{fview_} {
	if (mInst->type().name() == "const-bool") {
		QCheckBox* box     = new QCheckBox("");
		bool       checked = mInst->type().toJSON();
		box->setCheckState(checked ? Qt::Checked : Qt::Unchecked);

		connect(box, &QCheckBox::stateChanged, this, [this](int newState) {
			std::unique_ptr<chig::NodeType> newType;

			mInst->context().nodeTypeFromModule("lang", "const-bool", newState == Qt::Checked,
			                                    &newType);

			mInst->setType(std::move(newType));
		});

		box->setMaximumSize(box->sizeHint());
		mEmbedded = box;
	} else if (mInst->type().name() == "strliteral") {
		auto        edit = new QLineEdit();
		std::string s    = mInst->type().toJSON();
		edit->setText(QString::fromStdString(s));

		edit->setMaximumSize(edit->sizeHint());

		connect(edit, &QLineEdit::textChanged, this, [this](const QString& s) {
			std::unique_ptr<chig::NodeType> newType;

			mInst->context().nodeTypeFromModule("lang", "strliteral", s.toUtf8().constData(),
			                                    &newType);

			mInst->setType(std::move(newType));

		});

		mEmbedded = edit;
	} else if (mInst->type().name() == "const-int") {
		auto edit = new QLineEdit();
		edit->setValidator(new QIntValidator);
		int val = mInst->type().toJSON();
		edit->setText(QString::number(val));

		edit->setMaximumSize(edit->sizeHint());

		connect(edit, &QLineEdit::textChanged, this, [this](const QString& s) {
			std::unique_ptr<chig::NodeType> newType;

			mInst->context().nodeTypeFromModule("lang", "const-int", s.toInt(), &newType);

			mInst->setType(std::move(newType));

		});

		mEmbedded = edit;
	} else if (mInst->type().name() == "func") {
		QPushButton* butt = new QPushButton(i18n("Edit code"));
		connect(butt, &QPushButton::clicked, this, [this] {
			auto dialog = new EditCodeDialog(mInst, mFunctionView);

			dialog->exec();
		});

		butt->setMaximumSize(butt->sizeHint());

		mEmbedded = butt;
	} else if (mInst->type().name() == "const-float") {
		auto edit = new QLineEdit();
		edit->setValidator(new QDoubleValidator);
		double val = mInst->type().toJSON();
		edit->setText(QString::number(val));

		edit->setMaximumSize(edit->sizeHint());

		connect(edit, &QLineEdit::textChanged, this, [this](const QString& s) {
			std::unique_ptr<chig::NodeType> newType;

			mInst->context().nodeTypeFromModule("lang", "const-float", s.toDouble(), &newType);

			mInst->setType(std::move(newType));

		});

		mEmbedded = edit;
	}
}

unsigned int ChigraphNodeModel::nPorts(PortType portType) const {
	if (portType == PortType::In) {
		return mInst->type().execInputs().size() + mInst->type().dataInputs().size();
	}
	if (portType == PortType::Out) {
		return mInst->type().execOutputs().size() + mInst->type().dataOutputs().size();
	}

	return 1;  // ?
}

NodeDataType ChigraphNodeModel::dataType(PortType pType, PortIndex pIndex) const {
	if (pType == PortType::In) {
		std::pair<std::string, std::string> idandname;
		if (pIndex >= int(mInst->type().execInputs().size())) {
			if (pIndex - mInst->type().execInputs().size() >= mInst->type().dataInputs().size()) {
				return {};
			}

			idandname = {
			    mInst->type()
			        .dataInputs()[pIndex - mInst->type().execInputs().size()]
			        .type.qualifiedName(),
			    mInst->type().dataInputs()[pIndex - mInst->type().execInputs().size()].name};

		} else {
			idandname = {"exec", mInst->type().execInputs()[pIndex]};
		}
		return {QString::fromStdString(idandname.first), QString::fromStdString(idandname.second)};
	}
	if (pType == PortType::Out) {
		std::pair<std::string, std::string> idandname;
		if (pIndex >= int(mInst->type().execOutputs().size())) {
			auto dataOutput =
			    mInst->type().dataOutputs()[pIndex - mInst->type().execOutputs().size()];
			idandname = {dataOutput.type.qualifiedName(), dataOutput.name};

		} else {
			idandname = {"exec", mInst->type().execOutputs()[pIndex]};
		}
		return {QString::fromStdString(idandname.first), QString::fromStdString(idandname.second)};
	}

	return {};
}

QWidget* ChigraphNodeModel::embeddedWidget() { return mEmbedded; }
>>>>>>> 72bc2fd15dc647c84b035a98883bb0d8e86be593
