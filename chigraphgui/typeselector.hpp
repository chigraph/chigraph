#pragma once

#ifndef CHIGGUI_TYPE_SELECTOR_HPP
#define CHIGGUI_TYPE_SELECTOR_HPP

#include <KComboBox>

#include <chi/Fwd.hpp>

class TypeSelector : public KComboBox {
	Q_OBJECT
public:
	explicit TypeSelector(chi::ChiModule& module, QWidget* parent = nullptr);

	void setCurrentType(const chi::DataType& ty);
	chi::DataType currentType();

signals:

	void typeSelected(const chi::DataType& type);

private:
	chi::ChiModule* mModule;
};

#endif  // CHIGGUI_TYPE_SELECTOR_HPP
