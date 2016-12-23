#pragma once

#ifndef CHIG_GUI_CHIGNODEGUI_HPP
#define CHIG_GUI_CHIGNODEGUI_HPP

#include <nodes/DataModelRegistry>
#include <nodes/NodeDataModel>

#include <chig/ChigModule.hpp>
#include <chig/NodeInstance.hpp>
#include <chig/NodeType.hpp>

#include <QCheckBox>
#include <QLineEdit>
#include <QValidator>
#include <QPushButton>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QDialog>


#include <KTextEdit>
#include <KLocalizedString>

#include <memory>

class EditCodeDialog : public QDialog {
public:
    EditCodeDialog(chig::NodeInstance* inst) {
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
        connect(okButton, &QPushButton::clicked, this, [this, textEdit, lineEdit, inst] {
            std::string function = lineEdit->text().toStdString();
            std::string code = textEdit->toPlainText().toStdString();

            std::unique_ptr<chig::NodeType> ty;
            chig::Result res = inst->context().nodeTypeFromModule("c", "func", {{"code", code}, {"function", function}}, &ty);
            if(!res) {
                return; // TODO: error handling
            }
            inst->setType(std::move(ty));

            close();
        });
    }
};

class ChigNodeGui : public NodeDataModel
{
public:
	ChigNodeGui(chig::NodeInstance* inst_) : inst{inst_} {}
	chig::NodeInstance* inst;

	QString caption() const override
	{
		auto str = QString::fromStdString(inst->type().qualifiedName());
		return str;
	}
	QString name() const override { return QString::fromStdString(inst->type().qualifiedName()); }
	std::unique_ptr<NodeDataModel> clone() const override
	{
		return std::unique_ptr<ChigNodeGui>(new ChigNodeGui(inst));
	}

	virtual unsigned int nPorts(PortType portType) const override
	{
		if (portType == PortType::In) {
			return inst->type().execInputs().size() + inst->type().dataInputs().size();
		} else if (portType == PortType::Out) {
			return inst->type().execOutputs().size() + inst->type().dataOutputs().size();
		}

		return 1;  // ?
	}

	virtual NodeDataType dataType(PortType pType, PortIndex pIndex) const override
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
			return {
				QString::fromStdString(idandname.first), QString::fromStdString(idandname.second)};
		} else if (pType == PortType::Out) {
			std::pair<std::string, std::string> idandname;
			if (pIndex >= int(inst->type().execOutputs().size())) {
				auto dataOutput =
					inst->type().dataOutputs()[pIndex - inst->type().execOutputs().size()];
				idandname = {dataOutput.first.qualifiedName(), dataOutput.second};

			} else {
				idandname = {"exec", inst->type().execOutputs()[pIndex]};
			}
			return {
				QString::fromStdString(idandname.first), QString::fromStdString(idandname.second)};
		}

		return {};
	}

	virtual void setInData(std::shared_ptr<NodeData>, PortIndex) override {}
	virtual std::shared_ptr<NodeData> outData(PortIndex /*port*/) override { return nullptr; }
	virtual QWidget* embeddedWidget() override
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
            connect(butt, &QPushButton::clicked, this, [this]{
                auto dialog = new EditCodeDialog(inst);

                dialog->exec();
            });

            butt->setMaximumSize(butt->sizeHint());

            return butt;
        }

		return nullptr;
    }
	// We don't need saving...chigraph has its own serialization
	void save(Properties&) const override {}
};

#endif  // CHIG_GUI_CHIGNODEGUI_HPP
