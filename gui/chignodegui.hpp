#pragma once

#ifndef CHIG_GUI_CHIGNODEGUI_HPP
#define CHIG_GUI_CHIGNODEGUI_HPP

#include <nodes/DataModelRegistry>
#include <nodes/NodeDataModel>

#include <chig/ChigModule.hpp>
#include <chig/NodeInstance.hpp>
#include <chig/NodeType.hpp>

#include <memory>

class ChigNodeGui : public NodeDataModel
{
public:
	ChigNodeGui(chig::NodeInstance* inst_) : inst{inst_} {}
	chig::NodeInstance* inst;

	QString caption() const override { return QString::fromStdString(inst->id); }
	QString name() const override
	{
		return QString::fromStdString(inst->type->module + ":" + inst->type->name);
	}

	std::unique_ptr<NodeDataModel> clone() const override
	{
		return std::unique_ptr<ChigNodeGui>(new ChigNodeGui(inst));
	}

	virtual unsigned int nPorts(PortType portType) const override
	{
		if (portType == PortType::In) {
			return inst->type->execInputs.size() + inst->type->dataInputs.size();
		} else if (portType == PortType::Out) {
			return inst->type->execOutputs.size() + inst->type->dataOutputs.size();
		}

		return 1;  // ?
	}

	virtual NodeDataType dataType(PortType pType, PortIndex pIndex) const override
	{
		if (pType == PortType::In) {
			std::pair<std::string, std::string> idandname;
			if (pIndex >= int(inst->type->execInputs.size())) {
				if (pIndex - inst->type->execInputs.size() >= inst->type->dataInputs.size())
					return {};

				idandname = {
					inst->type->context->stringifyType(
						inst->type->dataInputs[pIndex - inst->type->execInputs.size()].first),
					inst->type->dataInputs[pIndex - inst->type->execInputs.size()].second};

			} else {
				idandname = {"exec", inst->type->execInputs[pIndex]};
			}
			return {
				QString::fromStdString(idandname.first), QString::fromStdString(idandname.second)};
		} else if (pType == PortType::Out) {
			std::pair<std::string, std::string> idandname;
			if (pIndex >= int(inst->type->execOutputs.size())) {
				idandname = {
					inst->type->context->stringifyType(
						inst->type->dataOutputs[pIndex - inst->type->execOutputs.size()].first),
					inst->type->dataOutputs[pIndex - inst->type->execOutputs.size()].second};

			} else {
				idandname = {"exec", inst->type->execOutputs[pIndex]};
			}
			return {
				QString::fromStdString(idandname.first), QString::fromStdString(idandname.second)};
		}

		return {};
	}

	virtual void setInData(std::shared_ptr<NodeData>, PortIndex) override {}
	virtual std::shared_ptr<NodeData> outData(PortIndex port) override { return nullptr; }
	virtual QWidget* embeddedWidget() override { return nullptr; };
	// We don't need saving...chigraph has its own serialization
	void save(Properties&) const override {}
};

#endif  // CHIG_GUI_CHIGNODEGUI_HPP
