/*
    Copyright (C) 2010 Klarälvdalens Datakonsult AB,
        a KDAB Group company, info@kdab.net,
        author Stephen Kelly <stephen@kdab.com>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#ifndef KVIEWSTATEMAINTAINER_H
#define KVIEWSTATEMAINTAINER_H

#include "kviewstatemaintainerbase.h"

#include <QItemSelectionModel>
#include <QAbstractItemView>

#include "kconfiggroup.h"

/**
 * @brief Encapsulates the maintenance of state between resets of QAbstractItemModel
 *
 * @code
 *   m_collectionViewStateMaintainer = new KViewStateMaintainer<Akonadi::ETMViewStateSaver>(KSharedConfig::openConfig()->group("collectionView"));
 *   m_collectionViewStateMaintainer->setView(m_collectionView);
 *
 *   m_collectionCheckStateMaintainer = new KViewStateMaintainer<Akonadi::ETMViewStateSaver>(KSharedConfig::openConfig()->group("collectionCheckState"));
 *   m_collectionCheckStateMaintainer->setSelectionModel(m_checkableProxy->selectionModel());
 * @endcode
 *
 * @see KViewStateSaver
 */
template<typename StateSaver>
class KViewStateMaintainer : public KViewStateMaintainerBase
{
    typedef StateSaver StateRestorer;
public:
    KViewStateMaintainer(const KConfigGroup &configGroup, QObject *parent = 0)
        : KViewStateMaintainerBase(parent), m_configGroup(configGroup)
    {

    }

    /* reimp */ void saveState()
    {
        StateSaver saver;
        saver.setView(view());
        saver.setSelectionModel(selectionModel());
        saver.saveState(m_configGroup);
        m_configGroup.sync();
    }

    /* reimp */ void restoreState()
    {
        StateRestorer *restorer = new StateRestorer;
        restorer->setView(view());
        restorer->setSelectionModel(selectionModel());
        restorer->restoreState(m_configGroup);
    }
private:
    KConfigGroup m_configGroup;
};

#endif
