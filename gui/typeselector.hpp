#pragma once

#ifndef CHIGGUI_TYPE_SELECTOR_HPP
#define CHIGGUI_TYPE_SELECTOR_HPP

#include <KComboBox>

#include <chig/Fwd.hpp>

class TypeSelector : public KComboBox {
	Q_OBJECT
public:
	
	TypeSelector (chig::ChigModule& module, QWidget* parent = nullptr);
	
	void setCurrentType(const chig::DataType& ty);
	chig::DataType currentType();
	
signals:
	
	void typeSelected(const chig::DataType& type);
	
private:
	
	chig::ChigModule* mModule;
	
};

#endif // CHIGGUI_TYPE_SELECTOR_HPP
