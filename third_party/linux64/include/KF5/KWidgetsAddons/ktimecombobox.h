/*
    Copyright 2011 John Layt <john@layt.net>

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

#ifndef KTIMECOMBOBOX_H
#define KTIMECOMBOBOX_H

#include <kwidgetsaddons_export.h>

#include <QComboBox>
#include <QLocale>

class KTimeComboBoxPrivate;

class KWIDGETSADDONS_EXPORT KTimeComboBox : public QComboBox
{
    Q_OBJECT

    Q_PROPERTY(QTime time READ time WRITE setTime NOTIFY timeChanged USER true)
    Q_PROPERTY(QTime minimumTime READ minimumTime WRITE setMinimumTime RESET resetMinimumTime)
    Q_PROPERTY(QTime maximumTime READ maximumTime WRITE setMaximumTime RESET resetMaximumTime)
    Q_PROPERTY(int timeListInterval READ timeListInterval WRITE setTimeListInterval)
    Q_PROPERTY(Options options READ options WRITE setOptions)

public:

    /**
     * Options provided by the widget
     * @see options
     * @see setOptions
     */
    enum Option {
        EditTime         = 0x0001,  /**< Allow the user to manually edit the time in the combo line edit */
        SelectTime       = 0x0002,  /**< Allow the user to select the time from a drop-down menu */
        ForceTime        = 0x0004,  /**< Any set or entered time will be forced to one of the drop-down times */
        WarnOnInvalid    = 0x0008   /**< Show a warning box on focus out if the user enters an invalid time */
    };
    Q_DECLARE_FLAGS(Options, Option)
    Q_FLAG(Options)

    /**
     * Create a new KTimeComboBox widget
     */
    explicit KTimeComboBox(QWidget *parent = 0);

    /**
     * Destroy the widget
     */
    virtual ~KTimeComboBox();

    /**
     * Return the currently selected time
     *
     * @return the currently selected time
     */
    QTime time() const;

    /**
     * Return if the current user input is valid
     *
     * If the user input is null then it is not valid
     *
     * @see isNull()
     * @return if the current user input is valid
     */
    bool isValid() const;

    /**
     * Return if the current user input is null
     *
     * @see isValid()
     * @return if the current user input is null
     */
    bool isNull() const;

    /**
     * Return the currently set widget options
     *
     * @return the currently set widget options
     */
    Options options() const;

    /**
     * Return the currently set time format
     *
     * By default this is the Short Format
     *
     * @return the currently set time format
     */
    QLocale::FormatType displayFormat() const;

    /**
     * Return the current minimum time
     *
     * @return the current minimum time
     */
    QTime minimumTime() const;

    /**
     * Reset the minimum time to the default of 00:00:00.000
     */
    void resetMinimumTime();

    /**
     * Return the current maximum time
     *
     * @return the current maximum time
     */
    QTime maximumTime() const;

    /**
     * Reset the maximum time to the default of 23:59:59.999
     */
    void resetMaximumTime();

    /**
     * Set the minimum and maximum time range.
     *
     * If either time is invalid, or min > max then the range will not be set.
     *
     * @param minTime the minimum time
     * @param maxTime the maximum time
     * @param minWarnMsg the minimum warning message
     * @param maxWarnMsg the maximum warning message
     */
    void setTimeRange(const QTime &minTime,
                      const QTime &maxTime,
                      const QString &minWarnMsg = QString(),
                      const QString &maxWarnMsg = QString());

    /**
     * Reset the minimum and maximum time to the default values.
     */
    void resetTimeRange();

    /**
     * Return the interval between select time list entries if set by setTimeListInterval().
     *
     * Returns -1 if not set.
     *
     * @see setTimeListInterval()
     * @return the select time list interval in minutes
     */
    int timeListInterval() const;

    /**
     * Return the list of times able to be selected in the drop-down.
     *
     * @see setTimeList()
     * @see timeListInterval()
     * @see setTimeListInterval()
     * @return the select time list
     */
    QList<QTime> timeList() const;

Q_SIGNALS:

    /**
     * Signal if the time has been manually entered or selected by the user.
     *
     * The returned time may be invalid.
     *
     * @param time the new time
     */
    void timeEntered(const QTime &time);

    /**
     * Signal if the time has been changed either manually by the user
     * or programatically.
     *
     * The returned time may be invalid.
     *
     * @param time the new time
     */
    void timeChanged(const QTime &time);

    /**
     * Signal if the time is being manually edited by the user.
     *
     * The returned time may be invalid.
     *
     * @param time the new time
     */
    void timeEdited(const QTime &time);

public Q_SLOTS:

    /**
     * Set the currently selected time
     *
     * You can set an invalid time or a time outside the valid range, validity
     * checking is only done via isValid().
     *
     * @param time the new time
     */
    void setTime(const QTime &time);

    /**
     * Set the new widget options
     *
     * @param options the new widget options
     */
    void setOptions(Options options);

    /**
     * Sets the time format to display.
     *
     * By default is the Short Format.
     *
     * @param format the time format to use
     */
    void setDisplayFormat(QLocale::FormatType formatOptions);

    /**
     * Set the minimum allowed time.
     *
     * If the time is invalid, or greater than current maximum,
     * then the minimum will not be set.
     *
     * @see minimumTime()
     * @see maximumTime()
     * @see setMaximumTime()
     * @see setTimeRange()
     * @param minTime the minimum time
     * @param minWarnMsg the minimum warning message
     */
    void setMinimumTime(const QTime &minTime, const QString &minWarnMsg = QString());

    /**
     * Set the maximum allowed time.
     *
     * If the time is invalid, or less than current minimum,
     * then the maximum will not be set.
     *
     * @see minimumTime()
     * @see maximumTime()
     * @see setMaximumTime()
     * @see setTimeRange()
     * @param maxTime the maximum time
     * @param maxWarnMsg the maximum warning message
     */
    void setMaximumTime(const QTime &maxTime, const QString &maxWarnMsg = QString());

    /**
     * Set the interval between times able to be selected from the drop-down.
     *
     * The combo drop-down will be populated with times every @param minutes
     * apart, starting from the minimumTime() and ending at maximumTime().
     *
     * If the ForceInterval option is set then any time manually typed into the
     * combo line edit will be forced to the nearest interval.
     *
     * This interval must be an exact divisor of the valid time range hours.
     * For example with the default 24 hour range @p interval must divide 1440
     * minutes exactly, meaning 1, 6 and 90 are valid but 7, 31 and 91 are not.
     *
     * Setting the time list interval will override any time list previously set
     * via setTimeList().
     *
     * @see timeListInterval()
     * @param minutes the time list interval to display
     */
    void setTimeListInterval(int minutes);

    /**
     * Set the list of times able to be selected from the drop-down.
     *
     * Setting the time list will override any time interval previously set via
     * setTimeListInterval().
     *
     * Any invalid or duplicate times will be ignored, and the list will be
     * sorted.
     *
     * The minimum and maximum time will automatically be set to the earliest
     * and latest value in the list.
     *
     * @see timeList()
     * @param timeList the list of times able to be selected
     * @param minWarnMsg the minimum warning message
     * @param maxWarnMsg the maximum warning message
     */
    void setTimeList(QList<QTime> timeList,
                     const QString &minWarnMsg = QString(),
                     const QString &maxWarnMsg = QString());

protected:

    bool eventFilter(QObject *object, QEvent *event) Q_DECL_OVERRIDE;
    void showPopup() Q_DECL_OVERRIDE;
    void hidePopup() Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void focusInEvent(QFocusEvent *event) Q_DECL_OVERRIDE;
    void focusOutEvent(QFocusEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

    /**
     * Assign the time for the widget.
     *
     * Virtual to allow sub-classes to apply extra validation rules.
     *
     * @param time the new time
     */
    virtual void assignTime(const QTime &time);

private:

    friend class KTimeComboBoxPrivate;
    KTimeComboBoxPrivate *const d;

    Q_PRIVATE_SLOT(d, void selectTime(int index))
    Q_PRIVATE_SLOT(d, void editTime(const QString &))
    Q_PRIVATE_SLOT(d, void enterTime(const QTime &))
    Q_PRIVATE_SLOT(d, void parseTime())

};

Q_DECLARE_OPERATORS_FOR_FLAGS(KTimeComboBox::Options)

#endif // KTIMECOMBOBOX_H
