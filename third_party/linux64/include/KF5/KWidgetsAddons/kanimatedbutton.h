/* This file is part of the KDE libraries
   Copyright (C) 2000 Kurt Granroth <granroth@kde.org>
   Copyright (C) 2006 Hamish Rodda <rodda@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#ifndef KANIMATEDBUTTON_H
#define KANIMATEDBUTTON_H

#include <kwidgetsaddons_export.h>
#include <QToolButton>

/**
 * @short An extended version of QToolButton which can display an animation.
 *
 * This widget extends QToolButton with the ability to display an animation.
 * All you need to do is pass along a path to a file containing an animation,
 * it can be anything supported by QMovie, or a picture containing all the
 * frames of the animation next to each other (each frame being assumed of
 * having the same size).
 *
 * @author Kurt Granroth <granroth@kde.org>
 */
class KWIDGETSADDONS_EXPORT KAnimatedButton : public QToolButton
{
    Q_OBJECT
    Q_PROPERTY(QString animationPath READ animationPath WRITE setAnimationPath)

public:
    /**
     * Construct an animated tool button.
     *
     * @param parent The parent widget
     */
    explicit KAnimatedButton(QWidget *parent = 0);

    /**
     * Destructor
     */
    virtual ~KAnimatedButton();

    /**
    * Returns the path used to load the animation
    */
    QString animationPath() const;

    /**
     * Sets the path to the file which contains the animation to load.
     *
     * @param path The path of the file containing the animation
     */
    void setAnimationPath(const QString &path);

public Q_SLOTS:
    /**
     * Starts the animation from frame 1
     */
    void start();

    /**
     * Stops the animation.  This will also reset the widget to frame 1.
     */
    void stop();

private:
    class KAnimatedButtonPrivate *const d;

    Q_PRIVATE_SLOT(d, void _k_movieFrameChanged(int))
    Q_PRIVATE_SLOT(d, void _k_movieFinished())
    Q_PRIVATE_SLOT(d, void _k_timerUpdate())

    Q_DISABLE_COPY(KAnimatedButton)
};

#endif //  KANIMATEDBUTTON_H
