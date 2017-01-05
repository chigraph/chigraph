/* vi: ts=8 sts=4 sw=4
 *
 * This file is part of the KDE project, module kdecore.
 * Copyright (C) 2000 Geert Jansen <jansen@kde.org>
 * Copyright (C) 2007 Daniel M. Duley <daniel.duley@verizon.net>
 * with minor additions and based on ideas from
 * Torsten Rahn <torsten@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KICONEFFECT_H
#define KICONEFFECT_H

#include <kiconthemes_export.h>

#include <QImage>
#include <QPixmap>
#include <QColor>
#include <QtCore/QRect>

class KIconEffectPrivate;

/**
 * Applies effects to icons.
 *
 * This class applies effects to icons depending on their state and
 * group. For example, it can be used to make all disabled icons
 * in a toolbar gray.
 *
 * \image html kiconeffect-apply.png "Various Effects applied to an image"
 *
 * @see QIcon::fromTheme
 */
class KICONTHEMES_EXPORT KIconEffect
{
public:
    /**
     * Create a new KIconEffect.
     * You will most likely never have to use this to create a new KIconEffect
     * yourself, as you can use the KIconEffect provided by the global KIconLoader
     * (which itself is accessible by KIconLoader::global()) through its
     * iconEffect() function.
     */
    KIconEffect();
    ~KIconEffect();

    /**
     * This is the enumeration of all possible icon effects.
     * Note that 'LastEffect' is no valid icon effect but only
     * used internally to check for invalid icon effects.
     *
     * @li NoEffect: Do not apply any icon effect
     * @li ToGray: Tints the icon gray
     * @li Colorize: Tints the icon with a specific color
     * @li ToGamma: Change the gamma value of the icon
     * @li DeSaturate: Reduce the saturation of the icon
     * @li ToMonochrome: Produces a monochrome icon
     */
    enum Effects { NoEffect, ToGray, Colorize, ToGamma, DeSaturate,
                   ToMonochrome,
                   LastEffect
                 };

    /**
     * Rereads configuration.
     */
    void init();

    /**
     * Tests whether an effect has been configured for the given icon group.
     * @param group the group to check, see KIconLoader::Group
     * @param state the state to check, see KIconLoader::States
     * @returns true if an effect is configured for the given @p group
     * in @p state, otherwise false.
     * @see KIconLoader::Group
     * KIconLoader::States
     */
    bool hasEffect(int group, int state) const;

    /**
     * Returns a fingerprint for the effect by encoding
     * the given @p group and @p state into a QString. This
     * is useful for caching.
     * @param group the group, see KIconLoader::Group
     * @param state the state, see KIconLoader::States
     * @return the fingerprint of the given @p group+@p state
     */
    QString fingerprint(int group, int state) const;

    /**
     * Applies an effect to an image. The effect to apply depends on the
     * @p group and @p state parameters, and is configured by the user.
     * @param src The image.
     * @param group The group for the icon, see KIconLoader::Group
     * @param state The icon's state, see KIconLoader::States
     * @return An image with the effect applied.
     */
    QImage apply(const QImage &src, int group, int state) const;

    /**
     * Applies an effect to an image.
     * @param src The image.
     * @param effect The effect to apply, one of KIconEffect::Effects.
     * @param value Strength of the effect. 0 <= @p value <= 1.
     * @param rgb Color parameter for effects that need one.
     * @param trans Add Transparency if trans = true.
     * @return An image with the effect applied.
     */
    QImage apply(const QImage &src, int effect, float value, const QColor &rgb, bool trans) const;
    QImage apply(const QImage &src, int effect, float value, const QColor &rgb, const QColor &rgb2, bool trans) const;

    /**
     * Applies an effect to a pixmap.
     * @param src The pixmap.
     * @param group The group for the icon, see KIconLoader::Group
     * @param state The icon's state, see KIconLoader::States
     * @return A pixmap with the effect applied.
     */
    QPixmap apply(const QPixmap &src, int group, int state) const;

    /**
     * Applies an effect to a pixmap.
     * @param src The pixmap.
     * @param effect The effect to apply, one of KIconEffect::Effects.
     * @param value Strength of the effect. 0 <= @p value <= 1.
     * @param rgb Color parameter for effects that need one.
     * @param trans Add Transparency if trans = true.
     * @return A pixmap with the effect applied.
     */
    QPixmap apply(const QPixmap &src, int effect, float value, const QColor &rgb, bool trans) const;
    QPixmap apply(const QPixmap &src, int effect, float value, const QColor &rgb, const QColor &rgb2, bool trans) const;

    /**
     * Returns an image twice as large, consisting of 2x2 pixels.
     * @param src the image.
     * @return the scaled image.
     */
    QImage doublePixels(const QImage &src) const;

    /**
     * Tints an image gray.
     *
     * @param image The image
     * @param value Strength of the effect. 0 <= @p value <= 1
     */
    static void toGray(QImage &image, float value);

    /**
     * Colorizes an image with a specific color.
     *
     * @param image The image
     * @param col The color with which the @p image is tinted
     * @param value Strength of the effect. 0 <= @p value <= 1
     */
    static void colorize(QImage &image, const QColor &col, float value);

    /**
     * Produces a monochrome icon with a given foreground and background color
     *
     * @param image The image
     * @param white The color with which the white parts of @p image are painted
     * @param black The color with which the black parts of @p image are painted
     * @param value Strength of the effect. 0 <= @p value <= 1
     */
    static void toMonochrome(QImage &image, const QColor &black, const QColor &white, float value);

    /**
     * Desaturates an image.
     *
     * @param image The image
     * @param value Strength of the effect. 0 <= @p value <= 1
     */
    static void deSaturate(QImage &image, float value);

    /**
     * Changes the gamma value of an image.
     *
     * @param image The image
     * @param value Strength of the effect. 0 <= @p value <= 1
     */
    static void toGamma(QImage &image, float value);

    /**
     * Renders an image semi-transparent.
     *
     * @param image The image
     */
    static void semiTransparent(QImage &image);

    /**
     * Renders a pixmap semi-transparent.
     *
     * @param pixmap The pixmap
     */
    static void semiTransparent(QPixmap &pixmap);

    /**
     * Overlays an image with an other image.
     *
     * @param src The image
     * @param overlay The image to overlay @p src with
     */
    static void overlay(QImage &src, QImage &overlay);

private:
    KIconEffectPrivate *const d;
};

#endif
