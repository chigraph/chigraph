/* This file is part of the KDE libraries
   Copyright (C) 2014 Thomas Lübking <thomas.luebking@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB. If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KSTYLEEXTENSIONS_H
#define KSTYLEEXTENSIONS_H

#include <QStyle>
#include <kwidgetsaddons_export.h>

namespace KStyleExtensions
{
/**
 * Runtime style extensions
 * You can use this to have a supporting QStyle implementation paint your widget
 * This is just convenience and does /not/ require the using widgets style to inherit KStyle (i.e.
 * calling this while using cleanlooks won't segfault but just return "0")
 *
 * For simplicity, only StyleHints, ControlElements and their SubElements are supported
 * If you don't need extended SubElement functionality, just skip its usage
 *
 * The @p element string has to be of the form: "appname.(2-char-element-type)_element"
 * The 2-char-element-type is the corresponding {SH, CE, SE}
 * Widgets in KWidgetsAddons don't have to pass the appname
 *
 * Examples: "CE_CapacityBar", "amarok.CE_Analyzer"
 *
 * Important notes:
 *  1) If your string lacks the matching "SH_", "CE_" or "SE_" token the element request will be ignored (return is 0)
 *  2) Try to avoid custom elements and use default ones (if possible) to get better style support and keep UI coherency
 *  3) If you cache this value (good idea, this requires a map lookup) do not forget to catch style changes in QWidget::changeEvent()!
 */

/**
 * Resolve a dynamic QStyle::ControlElement for eg. QStyle::drawControl()
 *
 * @returns a unique QStyle::ControlElement or 0 in case the style doesn't suuport this element
 * @p element a valid element string appname.CE_element, eg. "amarok.CE_Analyzer"
 * @p widget the widget to paint this element for. This parameter is mandatory, nullptr will return 0!
 * @since 5.3
 */
KWIDGETSADDONS_EXPORT QStyle::ControlElement customControlElement(const QString &element, const QWidget *widget);
/**
 * Resolve a dynamic QStyle::StyleHint to query QStyle::styleHint()
 *
 * @returns a unique QStyle::StyleHint or 0 in case the style doesn't suuport this element
 * @p element a valid element string appname.SH_element, eg. "amarok.SH_Analyzer"
 * @p widget the widget to paint this element for. This parameter is mandatory, nullptr will return 0!
 * @since 5.3
 */
KWIDGETSADDONS_EXPORT QStyle::StyleHint customStyleHint(const QString &element, const QWidget *widget);
/**
 * Resolve a dynamic QStyle::SubElement for eg. QStyle::subElementRect()
 *
 * @returns a unique QStyle::SubElement or 0 in case the style doesn't suuport this element
 * @p element a valid element string appname.SE_element, eg. "amarok.SE_AnalyzerCanvas"
 * @p widget the widget to paint this element for. This parameter is mandatory, nullptr will return 0!
 * @since 5.3
 */
KWIDGETSADDONS_EXPORT QStyle::SubElement customSubElement(const QString &element, const QWidget *widget);
}

#endif // KSTYLEEXTENSIONS_H