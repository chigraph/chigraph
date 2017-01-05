/* This file is part of the KDE frameworks
 *
 * Copyright (c) 2014 Aurélien Gâteau <agateau@kde.org>
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
#ifndef KCOLUMNRESIZER_H
#define KCOLUMNRESIZER_H

#include <kwidgetsaddons_export.h>

#include <QFormLayout>
#include <QObject>

class QEvent;
class QGridLayout;
class QLayout;
class QWidget;

class KColumnResizerPrivate;

/**
 * @short Maintains consistent column sizes across layouts
 *
 * KColumnResizer is a helper class which can force columns of different layouts
 * to keep the same width. It is useful to keep label columns consistent.
 *
 * It works with QGridLayout and QFormLayout.
 *
 * @image html kcolumnresizer.png "left: without KColumnResizer - right: with KColumnResizer"
 *
 * Here is a typical example:
 *
 * @code
 *  void Window::createWidgets()
 *  {
 *      QVBoxLayout *layout = new QVBoxLayout(this);
 *
 *      QGroupBox *box1 = new QGroupBox();
 *      // Fill box1
 *      // ...
 *      layout->addWidget(box1);
 *
 *      QGroupBox *box2 = new QGroupBox();
 *      // Fill box2
 *      // ...
 *      layout->addWidget(box2);
 *
 *      KColumnResizer *resizer = new KColumnResizer(this);
 *      resizer->addWidgetsFromLayout(box1->layout(), 0);
 *      resizer->addWidgetsFromLayout(box2->layout(), 0);
 *  }
 * @endcode
 *
 * In this example box1 and box2 children can be organized using QGridLayout or
 * QFormLayout, resizer will ensure the first columns of the two QGroupBox stay
 * the same width.
 *
 * @author Aurélien Gâteau <agateau@kde.org>
 *
 * @since 5.1
 */
class KWIDGETSADDONS_EXPORT KColumnResizer : public QObject
{
    Q_OBJECT
public:
    /**
     * Constructs a KColumnResizer.
     */
    explicit KColumnResizer(QObject *parent = 0);

    ~KColumnResizer();

    /**
     * Add all widgets from @p layout which are in column @p column to the list
     * of widgets to manage.
     *
     * @param layout The layout containing the widgets to add. KColumnResizer
     * supports QGridLayout and QFormLayout.
     * @param column The column number which contains the widgets. If layout is
     * a QFormLayout, column should not be higher than QFormLayout::SpanningRole
     */
    void addWidgetsFromLayout(QLayout *layout, int column = 0);

    /**
     * Add a single widget to the list of widgets whose width is monitored.
     *
     * It is more common to use addWidgetsFromLayout(), but adding single
     * widgets can be useful if you want to keep a single button the same width
     * as a column in a layout.
     *
     * @param widget The widget to add
     */
    void addWidget(QWidget *widget);

    /**
     * Remove a widget previously added by addWidget or addWidgetsFromLayout.
     *
     * @param widget The widget to remove
     */
    void removeWidget(QWidget *widget);

protected:
    bool eventFilter(QObject *, QEvent *event) Q_DECL_OVERRIDE;

private:
    KColumnResizerPrivate *const d;
    Q_DISABLE_COPY(KColumnResizer)

    Q_PRIVATE_SLOT(d, void updateWidth())
};

#endif /* KCOLUMNRESIZER_H */
