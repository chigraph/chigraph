#pragma once

#ifndef CHIG_GUI_CHIGNODEGUI_HPP
#define CHIG_GUI_CHIGNODEGUI_HPP

#include <nodes/DataModelRegistry>
#include <nodes/NodeDataModel>

#include <chi/NodeInstance.hpp>
#include <chi/NodeType.hpp>

#include <QCheckBox>
#include <QDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QValidator>

#include <KLocalizedString>
#include <KMessageBox>
#include <KTextEdit>

#include <memory>

class FunctionView;

class ChigraphNodeModel : public QtNodes::NodeDataModel {
public:
	ChigraphNodeModel(chi::NodeInstance* inst_, FunctionView* fview_);

	chi::NodeInstance& instance() const { return *mInst; }

	void setErrorState(QtNodes::NodeValidationState state, QString message);

	// NodeDataModel interface
	QString caption() const override {
		auto str = QString::fromStdString(mInst->type().description());
		return str;
	}
	QString name() const override { return QString::fromStdString(mInst->type().qualifiedName()); }
	std::unique_ptr<NodeDataModel> clone() const override {
		auto newInst = new chi::NodeInstance(*mInst);
		return std::make_unique<ChigraphNodeModel>(newInst, mFunctionView);
	}

	virtual unsigned int nPorts(QtNodes::PortType portType) const override;

	virtual QtNodes::NodeDataType dataType(QtNodes::PortType  pType,
	                                       QtNodes::PortIndex pIndex) const override;

	virtual void setInData(std::shared_ptr<QtNodes::NodeData>, QtNodes::PortIndex) override {}
	virtual std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex /*port*/) override {
		return nullptr;
	}
	virtual QWidget* embeddedWidget() override;

	QtNodes::NodeValidationState validationState() const override;

	QString validationMessage() const override;

	// We don't need saving...chigraph has its own serialization
	QJsonObject save() const override { return {}; }

	NodeConnectionPolicy nodeConnectionPolicy(QtNodes::PortType  portType,
	                                          QtNodes::PortIndex portIndex) const override {
		// good defaults
		if (portType == QtNodes::PortType::In) {
			if (portIndex < instance().inputExecConnections.size()) {
				return NodeConnectionPolicy::Many;
			}
			return NodeConnectionPolicy::One;
		}
		if (portIndex < instance().outputExecConnections.size()) {
			return NodeConnectionPolicy::One;
		}
		return NodeConnectionPolicy::Many;
	}

private:
	QtNodes::NodeValidationState mValidationState = QtNodes::NodeValidationState::Valid;
	QString                      mValidationMessage;
	chi::NodeInstance*           mInst;
	FunctionView*                mFunctionView;
	QWidget*                     mEmbedded = nullptr;
};

#endif  // CHIG_GUI_CHIGNODEGUI_HPP
