/**
 * Copyright 2014 Laurent Montel <montel@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#ifndef KPLURALHANDLINGSPINBOX_H
#define KPLURALHANDLINGSPINBOX_H

#include <ktextwidgets_export.h>
#include <KLocalizedString>
#include <QSpinBox>


/**
 * @brief A QSpinBox with plural handling for the suffix.
 *
 * @author Laurent Montel <montel@kde.org>
 *
 * @since 5.0
 */
class KTEXTWIDGETS_EXPORT KPluralHandlingSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    /**
     * Default constructor
     */

    explicit KPluralHandlingSpinBox(QWidget *parent = 0);
    ~KPluralHandlingSpinBox();

    /**
     * Sets the suffix to @p suffix.
     * Use this to add a plural-aware suffix, e.g. by using ki18np("singular", "plural").
     */
    void setSuffix(const KLocalizedString &suffix);
private:
    class KPluralHandlingSpinBoxPrivate;
    friend class KPluralHandlingSpinBoxPrivate;
    KPluralHandlingSpinBoxPrivate *const d;

    Q_DISABLE_COPY(KPluralHandlingSpinBox)
    Q_PRIVATE_SLOT(d, void updateSuffix(int))
};

#endif // KPLURALHANDLINGSPINBOX_H
