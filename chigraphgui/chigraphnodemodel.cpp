#include "chigraphnodemodel.hpp"

#include "functionview.hpp"

#include "../src/NodeGraphicsObject.hpp"

#include <chi/Context.hpp>
#include <chi/DataType.hpp>
#include <chi/Result.hpp>

#include <KActionCollection>
#include <KTextEditor/Document>
#include <KTextEditor/Editor>
#include <KTextEditor/View>

using namespace QtNodes;

class EditCodeDialog : public QDialog {
public:
	EditCodeDialog(chi::NodeInstance* inst, FunctionView* fview) {
		setWindowTitle(i18n("Edit C Call Node"));

		auto layout = new QVBoxLayout;
		setLayout(layout);

		auto lineEdit = new QLineEdit;
		layout->addWidget(lineEdit);
		lineEdit->setText(QString::fromStdString(inst->type().toJSON()["function"]));

		// get KTextEditor stuff
		KTextEditor::Editor* editor = KTextEditor::Editor::instance();
		// create a new document
		KTextEditor::Document* doc = editor->createDocument(this);
		doc->setText(QString::fromStdString(inst->type().toJSON()["code"]));
		doc->setHighlightingMode("C");
		connect(doc, &KTextEditor::Document::highlightingModeChanged, this, [](auto* doc) {
			std::cout << "Changed to " << doc->highlightingMode().toStdString() << std::endl;
		});
		// create a widget to display the document
		KTextEditor::View* textEdit = doc->createView(nullptr);
		// delete save and saveAs actions
		{
			for (const auto& action : textEdit->actionCollection()->actions()) {
				QString name = action->text();
			}
			auto saveAction = textEdit->actionCollection()->action("file_save");
			textEdit->actionCollection()->removeAction(saveAction);
			auto saveAsAction = textEdit->actionCollection()->action("file_save_as");
			textEdit->actionCollection()->removeAction(saveAsAction);
		}
		textEdit->setMinimumHeight(200);
		layout->addWidget(textEdit);

		auto okButton = new QPushButton;
		layout->addWidget(okButton);
		okButton->setText(i18n("Ok"));
		connect(okButton, &QPushButton::clicked, this, [this, doc, lineEdit, inst, fview] {
			std::string function = lineEdit->text().toStdString();
			std::string code     = doc->text().toStdString();

			std::unique_ptr<chi::NodeType> ty;
			auto res = static_cast<chi::GraphModule&>(inst->type().module()).createNodeTypeFromCCode(code, function, {}, &ty);
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

ChigraphNodeModel::ChigraphNodeModel(chi::NodeInstance* inst_, FunctionView* fview_)
    : mInst{inst_}, mFunctionView{fview_} {
	if (mInst->type().name() == "const-bool") {
		QCheckBox* box     = new QCheckBox("");
		bool       checked = mInst->type().toJSON();
		box->setCheckState(checked ? Qt::Checked : Qt::Unchecked);

		connect(box, &QCheckBox::stateChanged, this, [this](int newState) {
			std::unique_ptr<chi::NodeType> newType;

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
			std::unique_ptr<chi::NodeType> newType;

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
			std::unique_ptr<chi::NodeType> newType;

			mInst->context().nodeTypeFromModule("lang", "const-int", s.toInt(), &newType);

			mInst->setType(std::move(newType));

		});

		mEmbedded = edit;
	} else if (mInst->type().name() == "c-call") {
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
			std::unique_ptr<chi::NodeType> newType;

			mInst->context().nodeTypeFromModule("lang", "const-float", s.toDouble(), &newType);

			mInst->setType(std::move(newType));

		});

		mEmbedded = edit;
	}
}

void ChigraphNodeModel::setErrorState(QtNodes::NodeValidationState state, QString message) {
	mValidationState   = state;
	mValidationMessage = message;
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

QtNodes::NodeValidationState ChigraphNodeModel::validationState() const { return mValidationState; }

QString ChigraphNodeModel::validationMessage() const { return mValidationMessage; }

QWidget* ChigraphNodeModel::embeddedWidget() { return mEmbedded; }
