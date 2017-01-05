/* -*- c++ -*- */
/* This file is part of the KDE libraries
    Copyright (C) 1998 Jörg Habenicht (j.habenicht@europemail.com)

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

#ifndef KRULER_H
#define KRULER_H

#include <kwidgetsaddons_export.h>

#include <QAbstractSlider>

/**
 * A ruler widget.
 *
 * The vertical ruler looks similar to this:
 *
 *\code
 *    meters                       inches
 *
 *    ------   <--- end mark  ---> ------
 *        --                            -
 *        --   <---little mark--->     --
 *        --                            -
 *        --                          ---
 *       ---   <---medium mark          -
 *        --                           --
 *        --        tiny mark---->      -
 *        --                         ----
 *        --                            -
 *      ----   <-----big mark          --
 *        --                            -
 *      |>--   <--ruler pointer-->   |>--
 *
 * \endcode
 *
 * There are tiny marks, little marks, medium marks, and big marks along the
 *  ruler.
 *
 * To receive mouse clicks or mouse moves, the class has to be overloaded.
 *
 * \image html kruler.png "KDE Ruler Widget"
 *
 * @short A ruler widget.
 * @author Jörg Habenicht
 */
class KWIDGETSADDONS_EXPORT KRuler : public QAbstractSlider
{
    Q_OBJECT
    Q_PROPERTY(bool showTinyMarks READ showTinyMarks WRITE setShowTinyMarks)
    Q_PROPERTY(bool showLittleMarks READ showLittleMarks WRITE setShowLittleMarks)
    Q_PROPERTY(bool showMediumMarks READ showMediumMarks WRITE setShowMediumMarks)
    Q_PROPERTY(bool showBigMarks READ showBigMarks WRITE setShowBigMarks)
    Q_PROPERTY(bool showPointer READ showPointer WRITE setShowPointer)
    Q_PROPERTY(bool showEndLabel READ showEndLabel WRITE setShowEndLabel)
    Q_PROPERTY(int tinyMarkDistance READ tinyMarkDistance WRITE setTinyMarkDistance)
    Q_PROPERTY(int littleMarkDistance READ littleMarkDistance WRITE setLittleMarkDistance)
    Q_PROPERTY(int mediumMarkDistance READ mediumMarkDistance WRITE setBigMarkDistance)
    Q_PROPERTY(int bigMarkDistance READ bigMarkDistance WRITE setBigMarkDistance)
    Q_PROPERTY(double pixelPerMark READ pixelPerMark WRITE setPixelPerMark)
    Q_PROPERTY(bool lengthFixed READ lengthFixed WRITE setLengthFixed)
    Q_PROPERTY(QString endLabel READ endLabel WRITE setEndLabel)
    Q_PROPERTY(int length READ length WRITE setLength)
    Q_PROPERTY(int offset READ offset)
    Q_PROPERTY(int endOffset READ endOffset)

public:
    /**
     * The types of units used.
     **/
    enum MetricStyle { Custom = 0, Pixel, Inch, Millimetres, Centimetres, Metres };
    Q_ENUM(MetricStyle)

    /**
     * Constructs a horizontal ruler.
     */
    explicit KRuler(QWidget *parent = 0);
    /**
     * Constructs a ruler with orientation @p orient.
     *
     * @p parent and @p f are passed to QFrame.
     * The default look is a raised widget
     * but may be changed with the inherited QFrame methods.
     *
     * @param orient     Orientation of the ruler.
     * @param parent     Will be handed over to QFrame.
     * @param f          Will be handed over to QFrame.
     *
     **/
    explicit KRuler(Qt::Orientation orient, QWidget *parent = 0, Qt::WindowFlags f = 0);

    /**
     * Constructs a ruler with orientation @p orient and initial width @p widgetWidth.
     *
     * The width sets the fixed width of the widget. This is useful if you
     * want to draw the ruler bigger or smaller than the default size.
     * Note: The size of the marks doesn't change.
     * @p parent and @p f are passed to QFrame.
     *
     * @param orient      Orientation of the ruler.
     * @param widgetWidth Fixed width of the widget.
     * @param parent      Will be handed over to QFrame.
     * @param f           Will be handed over to QFrame.
     *
     */
    KRuler(Qt::Orientation orient, int widgetWidth, QWidget *parent = 0,
           Qt::WindowFlags f = 0);

    /**
     * Destructor.
     */
    ~KRuler();

    /**
     * Sets the minimal value of the ruler pointer (default is 0).
     *
     * This method calls update() so that the widget is painted after leaving
     * to the main event loop.
     *
     **/
#ifndef KWIDGETSADDONS_NO_DEPRECATED
    KWIDGETSADDONS_DEPRECATED void setMinValue(int);
#endif

    /**
     * Returns the minimal value of the ruler pointer.
     **/
#ifndef KWIDGETSADDONS_NO_DEPRECATED
    KWIDGETSADDONS_DEPRECATED int minValue() const;
#endif

    /**
     * Sets the maximum value of the ruler pointer (default is 100).
     *
     * This method calls update() so that the widget is painted after leaving
     * to the main event loop.
     */
#ifndef KWIDGETSADDONS_NO_DEPRECATED
    KWIDGETSADDONS_DEPRECATED void setMaxValue(int);
#endif

    /**
     * Returns the maximal value of the ruler pointer.
     */
#ifndef KWIDGETSADDONS_NO_DEPRECATED
    KWIDGETSADDONS_DEPRECATED int maxValue() const;
#endif

    /**
     * Sets the distance between tiny marks.
     *
     * This is mostly used in the English system (inches) with distance of 1.
     */
    void setTinyMarkDistance(int);
    /**
     * Returns the distance between tiny marks.
     **/
    int tinyMarkDistance() const;

    /**
     * Sets the distance between little marks.
     *
     * The default value is 1 in the metric system and 2 in the English (inches) system.
     */
    void setLittleMarkDistance(int);

    /**
     * Returns the distance between little marks.
     */
    int littleMarkDistance() const;

    /**
     * Sets the distance between medium marks.
     *
     * For English (inches) styles it defaults to twice the little mark distance.
     * For metric styles it defaults to five times the little mark distance.
     **/
    void setMediumMarkDistance(int);
    int mediumMarkDistance() const;

    /**
     * Sets distance between big marks.
     *
     * For English (inches) or metric styles it is twice the medium mark distance.
     **/
    void setBigMarkDistance(int);
    /**
     * Returns the distance between big marks.
     **/
    int bigMarkDistance() const;

    /**
     * Shows/hides tiny marks.
     **/
    void setShowTinyMarks(bool);
    bool showTinyMarks() const;
    /**
     * Shows/hides little marks.
     **/
    void setShowLittleMarks(bool);
    bool showLittleMarks() const;
    /**
     * Shows/hides medium marks.
     **/
    void setShowMediumMarks(bool);
    bool showMediumMarks() const;
    /**
     * Shows/hides big marks.
     **/
    void setShowBigMarks(bool);
    bool showBigMarks() const;
    /**
     * Shows/hides end marks.
     **/
    void setShowEndMarks(bool);
    bool showEndMarks() const;
    /**
     * Shows/hides the pointer.
     */
    void setShowPointer(bool);
    bool showPointer() const;

#ifndef KWIDGETSADDONS_NO_DEPRECATED
    KWIDGETSADDONS_DEPRECATED void setFrameStyle(int);
#endif

    /**
     * Show/hide number values of the end marks.
     *
     * Default is @p false.
     **/
    void setShowEndLabel(bool);
    bool showEndLabel() const;

    /**
     * Sets the label this is drawn at the beginning of the visible part
     * of the ruler to @p label
     **/
    void setEndLabel(const QString &);
    QString endLabel() const;

    /**
     * Sets up the necessary tasks for the provided styles.
     *
     * A convenience method.
     **/
    void setRulerMetricStyle(KRuler::MetricStyle);

    /**
     * Sets the number of pixels between two base marks.
     *
     * Calling this method stretches or shrinks your ruler.
     *
     * For pixel display ( MetricStyle) the value is 10.0 marks
     * per pixel ;-)
     * For English (inches) it is 9.0, and for centimetres ~2.835 -> 3.0 .
     * If you want to magnify your part of display, you have to
     * adjust the mark distance @p here.
     * Notice: The double type is only supported to give the possibility
     *         of having some double values.
     *         It should be used with care.  Using values below 10.0
     *         shows visible jumps of markpositions (e.g. 2.345).
     *         Using whole numbers is highly recommended.
     * To use @p int values use setPixelPerMark((int)your_int_value);
     * default: 1 mark per 10 pixels
     */
    void setPixelPerMark(double rate);

    /**
     * Returns the number of pixels between two base marks.
     **/
    double pixelPerMark() const;

    /**
     * Sets the length of the ruler, i.e. the difference between
     * the begin mark and the end mark of the ruler.
     *
     * Same as (width() - offset())
     *
     * when the length is not locked, it gets adjusted with the
     * length of the widget.
     */
    void setLength(int);
    int length() const;

    /**
     * Locks the length of the ruler, i.e. the difference between
     * the two end marks doesn't change when the widget is resized.
     *
     * @param fix fixes the length, if true
     */
    void setLengthFixed(bool fix);
    bool lengthFixed() const;

    /**
     * Sets the number of pixels by which the ruler may slide up or left.
     * The number of pixels moved is realive to the previous position.
     * The Method makes sense for updating a ruler, which is working with
     * a scrollbar.
     *
     * This doesn't affect the position of the ruler pointer.
     * Only the visible part of the ruler is moved.
     *
     * @param count Number of pixel moving up or left relative to the previous position
     **/
    void slideUp(int count = 1);

    /**
     * Sets the number of pixels by which the ruler may slide down or right.
     * The number of pixels moved is realive to the previous position.
     * The Method makes sense for updating a ruler, which is working with
     * a scrollbar.
     *
     * This doesn't affect the position of the ruler pointer.
     * Only the visible part of the ruler is moved.
     *
     * @param count Number of pixel moving up or left relative to the previous position
     **/
    void slideDown(int count = 1);

    /**
     * Sets the ruler slide offset.
     *
     * This is like slideup() or slidedown() with an absolute offset
     * from the start of the ruler.
     *
     * @param offset Number of pixel to move the ruler up or left from the beginning
     **/
    void setOffset(int offset);

    /**
     * Returns the current ruler offset.
     **/
    int offset() const;

    int endOffset() const;

public Q_SLOTS:

    /**
     * Sets the pointer to a new position.
     *
     * The offset is NOT updated.
     * QWidget::repaint() is called afterwards.
     **/
    void slotNewValue(int);

    /**
     * Sets the ruler marks to a new position.
     *
     * The pointer is NOT updated.
     * QWidget::repaint() is called afterwards.
     **/
    void slotNewOffset(int);

    void slotEndOffset(int);

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

private:
    void initWidget(Qt::Orientation orientation);

private:
    class KRulerPrivate;
    KRulerPrivate *const d;
};

#endif
