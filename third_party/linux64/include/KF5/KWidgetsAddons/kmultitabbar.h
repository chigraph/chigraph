/***************************************************************************
                          kmultitabbar.h -  description
                             -------------------
    begin                :  2001
    copyright            : (C) 2001,2002,2003 by Joseph Wenninger <jowenn@kde.org>
 ***************************************************************************/

/***************************************************************************
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
 ***************************************************************************/

#ifndef _KMultitabbar_h_
#define _KMultitabbar_h_

#include <QLayout>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QPushButton>

#include <kwidgetsaddons_export.h>

class QPixmap;
class QPainter;
class QMenu;
class QStyleOptionToolButton;

class KMultiTabBarPrivate;
class KMultiTabBarTabPrivate;
class KMultiTabBarButtonPrivate;
class KMultiTabBarInternal;

/**
 * A Widget for horizontal and vertical tabs.
 * (Note that in Qt4, QTabBar can be vertical as well)
 *
 * It is possible to add normal buttons to the top/left
 * The handling if only one tab at a time or multiple tabs
 * should be raisable is left to the "user".
 *
 * \image html kmultitabbar.png "KDE Multi Tab Bar Widget"
 *
 * @author Joseph Wenninger
 */
class KWIDGETSADDONS_EXPORT KMultiTabBar: public QWidget
{
    Q_OBJECT
    Q_PROPERTY(KMultiTabBarPosition position READ position WRITE setPosition)
    Q_PROPERTY(KMultiTabBarStyle tabStyle READ tabStyle WRITE setStyle)
public:
    enum KMultiTabBarPosition { Left, Right, Top, Bottom };
    Q_ENUM(KMultiTabBarPosition)

    /**
     * The list of available styles for KMultiTabBar
     *   - VSNET - Visual Studio .Net like, always shows icon, only show the text of active tabs
     *   - KDEV3ICON - Kdevelop 3 like, always shows the text and icons
     */
    enum KMultiTabBarStyle {VSNET = 0, KDEV3ICON = 2, STYLELAST = 0xffff};
    Q_ENUM(KMultiTabBarStyle)

    /**
     * Create a KMultiTabBar with Left as KMultiTabBar position.
     * @param parent The parent of the widget.
     * @since 5.24
     */
    explicit KMultiTabBar(QWidget *parent = Q_NULLPTR);

    explicit KMultiTabBar(KMultiTabBarPosition pos, QWidget *parent = 0);
    virtual ~KMultiTabBar();

    /**
     * append  a new button to the button area. The button can later on be accessed with button(ID)
     * eg for connecting signals to it
     * @param icon a icon for the button
     * @param id an arbitraty ID value. It will be emitted in the clicked signal for identifying the button
     *  if more than one button is connected to a signals.
     * @param popup A popup menu which should be displayed if the button is clicked
     * @param not_used_yet will be used for a popup text in the future
     */
    int appendButton(const QIcon &icon, int id = -1, QMenu *popup = 0, const QString &not_used_yet = QString());

#ifndef KWIDGETSADDONS_NO_DEPRECATED
    /**
     * append  a new button to the button area. The button can later on be accessed with button(ID)
     * eg for connecting signals to it
     *
     * @deprecated since 5.13, use the appendButton() with QIcon
     *
     * @param pic a pixmap for the button
     * @param id an arbitraty ID value. It will be emitted in the clicked signal for identifying the button
     *  if more than one button is connected to a signals.
     * @param popup A popup menu which should be displayed if the button is clicked
     * @param not_used_yet will be used for a popup text in the future
     */
    KWIDGETSADDONS_DEPRECATED int appendButton(const QPixmap &pic, int id = -1, QMenu *popup = 0, const QString &not_used_yet = QString());
#endif

    /**
     * remove a button with the given ID
     */
    void removeButton(int id);

    /**
     * append a new tab to the tab area. It can be accessed lateron with tabb(id);
     * @param icon a icon for the tab
     * @param id an arbitrary ID which can be used later on to identify the tab
     * @param text if a mode with text is used it will be the tab text, otherwise a mouse over hint
     */
    int appendTab(const QIcon &icon, int id = -1, const QString &text = QString());

#ifndef KWIDGETSADDONS_NO_DEPRECATED
    /**
     * append a new tab to the tab area. It can be accessed lateron with tabb(id);
     *
     * @deprecated since 5.13, use the appendTab() with QIcon
     *
     * @param pic a bitmap for the tab
     * @param id an arbitrary ID which can be used later on to identify the tab
     * @param text if a mode with text is used it will be the tab text, otherwise a mouse over hint
     */
    KWIDGETSADDONS_DEPRECATED int appendTab(const QPixmap &pic, int id = -1, const QString &text = QString());
#endif

    /**
     * remove a tab with a given ID
     */
    void removeTab(int id);
    /**
     * set a tab to "raised"
     * @param id The ID of the tab to manipulate
     * @param state true == activated/raised, false == not active
     */
    void setTab(int id, bool state);
    /**
     * return the state of a tab, identified by its ID
     */
    bool isTabRaised(int id) const;
    /**
     * get a pointer to a button within the button area identified by its ID
     */
    class KMultiTabBarButton *button(int id) const;

    /**
     * get a pointer to a tab within the tab area, identiifed by its ID
     */
    class KMultiTabBarTab *tab(int id) const;

    /**
     * set the real position of the widget.
     * @param pos if the mode is horizontal, only use top, bottom, if it is vertical use left or right
     */
    void setPosition(KMultiTabBarPosition pos);

    /**
     * get the tabbar position.
     * @return position
     */
    KMultiTabBarPosition position() const;

    /**
     * set the display style of the tabs
     */
    void setStyle(KMultiTabBarStyle style);

    /**
     * get the display style of the tabs
     * @return display style
     */
    KMultiTabBarStyle tabStyle() const;

protected:
    friend class KMultiTabBarButton;
    virtual void fontChange(const QFont &);
    void updateSeparator();
private:
    KMultiTabBarPrivate *const d;
};

/**
 * Use KMultiTabBar::appendButton to append a button, which creates a KMultiTabBarButton instance
 */
class KWIDGETSADDONS_EXPORT KMultiTabBarButton: public QPushButton
{
    Q_OBJECT
public:
    int id() const;
    virtual ~KMultiTabBarButton();

public Q_SLOTS:
    void setText(const QString &text);

Q_SIGNALS:
    /**
     * this is emitted if  the button is clicked
     * @param id    the ID identifying the button
     */
    void clicked(int id);
protected Q_SLOTS:
    virtual void slotClicked();

protected:
    void hideEvent(class QHideEvent *) Q_DECL_OVERRIDE;
    void showEvent(class QShowEvent *) Q_DECL_OVERRIDE;
    void paintEvent(class QPaintEvent *) Q_DECL_OVERRIDE;

    /** Should not be created directly. Use KMultiTabBar::appendButton
    */
    KMultiTabBarButton(const QIcon &icon, const QString &, int id, QWidget *parent);

    /** Should not be created directly. Use KMultiTabBar::appendButton
    */
    KMultiTabBarButton(const QPixmap &pic, const QString &, int id, QWidget *parent);
private:
    friend class KMultiTabBar;

    int m_id;
    KMultiTabBarButtonPrivate *const d;
};

/**
 * Use KMultiTabBar::appendTab to append a tab, which creates a KMultiTabBarTab instance
 */
class KWIDGETSADDONS_EXPORT KMultiTabBarTab: public KMultiTabBarButton
{
    Q_OBJECT
public:
    virtual ~KMultiTabBarTab();
    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;

public Q_SLOTS:
    /**
     * this is used internaly, but can be used by the user, if (s)he wants to
     * It the according call of KMultiTabBar is invoked though this modifications will be overwritten
     */
    void setPosition(KMultiTabBar::KMultiTabBarPosition);

    /**
     * this is used internaly, but can be used by the user, if (s)he wants to
     * It the according call of KMultiTabBar is invoked though this modifications will be overwritten
     */
    void setStyle(KMultiTabBar::KMultiTabBarStyle);

    /**
     * set the active state of the tab
     * @param  state true==active false==not active
     */
    void setState(bool state);

    void setIcon(const QString &);
    void setIcon(const QPixmap &);
protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
private:
    KMultiTabBar::KMultiTabBarPosition m_position;
    KMultiTabBar::KMultiTabBarStyle m_style;

    void  computeMargins(int *hMargin, int *vMargin) const;
    QSize computeSizeHint(bool withText) const;
    bool shouldDrawText() const;
    bool isVertical()     const;
    QPixmap iconPixmap()  const;

    void initStyleOption(QStyleOptionToolButton *opt) const;

    friend class KMultiTabBarInternal;
    /**
     * This class should never be created except with the appendTab call of KMultiTabBar
     */
    KMultiTabBarTab(const QIcon &icon, const QString &, int id, QWidget *parent,
                    KMultiTabBar::KMultiTabBarPosition pos, KMultiTabBar::KMultiTabBarStyle style);
    /**
     * This class should never be created except with the appendTab call of KMultiTabBar
     */
    KMultiTabBarTab(const QPixmap &pic, const QString &, int id, QWidget *parent,
                    KMultiTabBar::KMultiTabBarPosition pos, KMultiTabBar::KMultiTabBarStyle style);
    KMultiTabBarTabPrivate *const d;
};

#endif
