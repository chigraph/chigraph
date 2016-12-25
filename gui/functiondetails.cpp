#include "functiondetails.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <KLocalizedString>


FunctionDetails::FunctionDetails(QWidget *parent) : QWidget(parent)
{
    auto layout = new QVBoxLayout;
    setLayout(layout);

    ins = new ParamListWidget(i18n("Inputs"));
    layout->addWidget(ins);

    outs = new ParamListWidget(i18n("Outputs"));
    layout->addWidget(outs);
}

void FunctionDetails::loadFunction(chig::GraphFunction *func)
{
    mFunc = func;

    ins->clear();
    outs->clear();
    
    ins->setModule(&func->module());
    outs->setModule(&func->module());

    for(auto in : func->inputs()) {
        ins->addParam(in.first, QString::fromStdString(in.second), ins->paramCount() - 1);
    }

    for(auto out : func->outputs()) {
        outs->addParam(out.first, QString::fromStdString(out.second), ins->paramCount() - 1);
    }
}
