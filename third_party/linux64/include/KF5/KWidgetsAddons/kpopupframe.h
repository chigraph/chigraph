/*  -*- C++ -*-
    This file is part of the KDE libraries
    Copyright (C) 1997 Tim D. Gilman (tdgilman@best.org)
              (C) 1998-2001 Mirko Boehm (mirko@kde.org)
              (C) 2007 John Layt <john@layt.net>
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KPOPUPFRAME_H
#define KPOPUPFRAME_H

#include <kwidgetsaddons_export.h>

#include <QFrame>

/**
 * Frame with popup menu behavior.
 * @author Tim Gilman, Mirko Boehm
 */
class KWIDGETSADDONS_EXPORT KPopupFrame : public QFrame
{
    Q_OBJECT
protected:
    /**
     * Catch key press events.
     */
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;

    /**
     * Catch hide events.
     */
    void hideEvent(QHideEvent *e) Q_DECL_OVERRIDE;

public Q_SLOTS:
    /**
     * Close the popup window. This is called from the main widget, usually.
     * @p r is the result returned from exec().
     */
    void close(int r);

public:
    /**
     * The contructor. Creates a dialog without buttons.
     */
    KPopupFrame(QWidget *parent = 0);

    /**
     * The destructor
     */
    ~KPopupFrame();

    /**
     * Set the main widget. You cannot set the main widget from the constructor,
     * since it must be a child of the frame itselfes.
     * Be careful: the size is set to the main widgets size. It is up to you to
     * set the main widgets correct size before setting it as the main
     * widget.
     */
    void setMainWidget(QWidget *m);

    /**
     * The resize event. Simply resizes the main widget to the whole
     * widgets client size.
     */
    void resizeEvent(QResizeEvent *resize) Q_DECL_OVERRIDE;

    /**
     * Open the popup window at position pos.
     */
    void popup(const QPoint &pos);

    /**
     * Execute the popup window.
     */
    int exec(const QPoint &p);

    /**
     * Execute the popup window.
     */
    int exec(int x, int y);

Q_SIGNALS:
    void leaveModality();

private:
    class KPopupFramePrivate;
    friend class KPopupFramePrivate;
    KPopupFramePrivate *const d;

    Q_DISABLE_COPY(KPopupFrame)
};

#endif // KPOPUPFRAME_H
