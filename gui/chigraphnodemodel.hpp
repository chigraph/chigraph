#pragma once

#ifndef CHIG_GUI_CHIGNODEGUI_HPP
#define CHIG_GUI_CHIGNODEGUI_HPP

#include <../src/Node.hpp>
#include <nodes/DataModelRegistry>
#include <nodes/NodeDataModel>

#include <chig/ChigModule.hpp>
#include <chig/NodeInstance.hpp>
#include <chig/NodeType.hpp>

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

class ChigraphNodeModel : public NodeDataModel {
public:
	ChigraphNodeModel(chig::NodeInstance* inst_, FunctionView* fview_);

	chig::NodeInstance& instance() const { return *mInst; }
	QString             caption() const override {
		auto str = QString::fromStdString(mInst->type().qualifiedName());
		return str;
	}
	QString name() const override { return QString::fromStdString(mInst->type().qualifiedName()); }
	std::unique_ptr<NodeDataModel> clone() const override {
		auto newInst = new chig::NodeInstance(*mInst, QUuid::createUuid().toString().toStdString());
		return std::make_unique<ChigraphNodeModel>(newInst, mFunctionView);
	}

	virtual unsigned int nPorts(PortType portType) const override;

	virtual NodeDataType dataType(PortType pType, PortIndex pIndex) const override;

	virtual void                      setInData(std::shared_ptr<NodeData>, PortIndex) override {}
	virtual std::shared_ptr<NodeData> outData(PortIndex /*port*/) override { return nullptr; }
	virtual QWidget*                  embeddedWidget() override;
	// We don't need saving...chigraph has its own serialization
	void save(Properties&) const override {}

private:
	chig::NodeInstance* mInst;
	FunctionView*       mFunctionView;
	QWidget* mEmbedded = nullptr;
};

#endif  // CHIG_GUI_CHIGNODEGUI_HPP
