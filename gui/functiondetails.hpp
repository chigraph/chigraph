#pragma once

#ifndef CHIGGUI_FUNCTIONDETAILS_HPP
#define CHIGGUI_FUNCTIONDETAILS_HPP

#include <QWidget>

#include <chig/GraphFunction.hpp>

#include "paramlistwidget.hpp"

class FunctionDetails : public QWidget {
    Q_OBJECT
public:

    FunctionDetails(QWidget* parent = nullptr);

public slots:
    void loadFunction(chig::GraphFunction* func);

private:

    chig::GraphFunction* mFunc = nullptr;

    ParamListWidget* ins;
    ParamListWidget* outs;

};

#endif // CHIGGUI_FUNCTIONDETAILS_HPP
