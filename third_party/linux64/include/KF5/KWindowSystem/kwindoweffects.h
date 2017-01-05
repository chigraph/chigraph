/*
 *   Copyright 2009 Marco Martin <notmart@gmail.com>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KWINDOWEFFECTS_H
#define KWINDOWEFFECTS_H

#include "kwindowsystem_export.h"

#include <QWidget> // for WId, etc.
#include <netwm_def.h>

/**
 * Namespace for common standardized window effects
 */
namespace KWindowEffects
{
enum Effect {
    Slide = 1,
    PresentWindows = 3,
    PresentWindowsGroup = 4,
    HighlightWindows = 5,
    BlurBehind = 7,
    Dashboard = 8,
    BackgroundContrast = 9
};

enum SlideFromLocation {
    NoEdge = 0,
    TopEdge,
    RightEdge,
    BottomEdge,
    LeftEdge
};
/**
 * @return if an atom property is available
 *
 * @param effect the effect we want to check
 */
KWINDOWSYSTEM_EXPORT bool isEffectAvailable(Effect effect);

/**
 * Mark a window as sliding from screen edge
 *
 * @param id of the window on which we want to apply the effect
 * @param location edge of the screen from which we want the sliding effect.
 *               Desktop and Floating won't have effect.
 * @param offset distance in pixels from the screen edge defined by location
 */
KWINDOWSYSTEM_EXPORT void slideWindow(WId id, SlideFromLocation location, int offset);

/**
 * Mark a window as sliding from screen edge
 * This is an overloaded member function provided for convenience
 *
 * @param widget QWidget corresponding to the top level window we want to animate
 * @param location edge of the screen from which we want the sliding effect.
 *               Desktop and Floating won't have effect.
 */
KWINDOWSYSTEM_EXPORT void slideWindow(QWidget *widget, SlideFromLocation location);

/**
 * @return dimension of all the windows passed as parameter
 *
 * @param ids all the windows we want the size
 */
KWINDOWSYSTEM_EXPORT QList<QSize> windowSizes(const QList<WId> &ids);

/**
* Activate the Present Windows effect for the given groups of windows.
*
* @param controller The window which is the controller of this effect. The property
*                   will be set on this window. It will be removed by the effect
* @param ids all the windows which should be presented.
*/
KWINDOWSYSTEM_EXPORT void presentWindows(WId controller, const QList<WId> &ids);

/**
 * Activate the Present Windows effect for the windows of the given desktop.
 *
 * @param controller The window which is the controller of this effect. The property
 *                   will be set on this window. It will be removed by the effect
 * @param desktop The desktop whose windows should be presented. -1 for all desktops
 */
KWINDOWSYSTEM_EXPORT void presentWindows(WId controller, int desktop = NET::OnAllDesktops);

/**
 * Highlight the selected windows, making all the others translucent
 *
 * @param controller The window which is the controller of this effect. The property
 *                   will be set on this window. It will be removed by the effect
 * @param ids all the windows which should be highlighted.
 */
KWINDOWSYSTEM_EXPORT void highlightWindows(WId controller, const QList<WId> &ids);

/**
 * Instructs the window manager to blur the background
 * in the specified region behind the given window.
 * The given region will overwrite any previous blur-behind region.
 * Passing a null region will enable the blur effect for the whole window.
 * The region is relative to the top-left corner of the client area.
 *
 * If @a enable is @c false, blur will be disabled for the whole window
 * (@a region is ignored).
 *
 * Note that you will usually want to set the region to the shape of the window,
 * excluding any shadow or halo.
 *
 * @param window The window for which to enable the blur effect
 * @param enable Enable the effect if @c true, disable it if @c false
 * @param region The region within the window where the background will be blurred
 */
KWINDOWSYSTEM_EXPORT void enableBlurBehind(WId window, bool enable = true, const QRegion &region = QRegion());

/**
 * Instructs the window manager to modify the color of the background
 * in the specified region behind the given window,
 * in order to improve the contrast and readability of any text
 * in the translucent window.
 * The given region will overwrite any previous backgroundcontrast region.
 * Passing a null region will enable the blur effect for the whole window.
 * The region is relative to the top-left corner of the client area.
 *
 * If @a enable is @c false, blur will be disabled for the whole window
 * (@a region is ignored).
 *
 * Note that you will usually want to set the region to the shape of the window,
 * excluding any shadow or halo.
 *
 * @param window The window for which to enable the background contrast effect
 * @param enable Enable the effect if @c true, disable it if @c false
 * @param brightness How to modify the area brightness: from 0 (make it black) to 2 (make it white), 1 leaves it unchanged
 * @param region The region within the window where the background will be modified
 */
KWINDOWSYSTEM_EXPORT void enableBackgroundContrast(WId window, bool enable = true, qreal contrast = 1, qreal intensity = 1, qreal saturation = 1, const QRegion &region = QRegion());

/**
 * Instructs the window manager to handle the given window as dashboard window as
 * Dashboard windows should be handled diffrently and may have special effects
 * applied to them.
 *
 * @param window The window for which to enable the blur effect
 */
KWINDOWSYSTEM_EXPORT void markAsDashboard(WId window);
}

#endif

