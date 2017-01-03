#pragma once

#ifndef CHIGGUI_MODULE_DEPENDENCIES_HPP
#define CHIGGUI_MODULE_DEPENDENCIES_HPP

#include <QListWidget>
#include <QListWidgetItem>
#include <QInputDialog>

#include <KMessageBox>
#include <KLocalizedString>

#include <chig/JsonModule.hpp>

class ModuleDependencies : public QListWidget {
    
    Q_OBJECT
    
public:
    ModuleDependencies(QWidget* parent = nullptr);
    ~ModuleDependencies() = default;
    
    
    chig::JsonModule* module() const {
        return mModule;
    }
    
public slots:
    void setModule(chig::JsonModule* mod) {
        clear();
        addNewDepItem();
        mModule = mod;
    }
    
signals:
    void dependencyAdded(QString str);
    
private:
    
    void addNewDepItem();
    
    chig::JsonModule* mModule = nullptr;
    
    QListWidgetItem* mAddDepItem;
    
};

#endif // CHIGGUI_MODULE_DEPENDENCIES_HPP
