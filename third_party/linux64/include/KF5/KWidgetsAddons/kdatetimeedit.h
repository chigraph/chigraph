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

#ifndef KDATETIMEEDIT_H
#define KDATETIMEEDIT_H

#include <kwidgetsaddons_export.h>

#include <QLocale>
#include <QTimeZone>
#include <QWidget>

class KDateTimeEditPrivate;

class KWIDGETSADDONS_EXPORT KDateTimeEdit : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QDate date READ date WRITE setDate NOTIFY dateChanged USER true)
    Q_PROPERTY(QTime time READ time WRITE setTime NOTIFY timeChanged USER true)
    Q_PROPERTY(int timeListInterval READ timeListInterval WRITE setTimeListInterval)
    Q_PROPERTY(Options options READ options WRITE setOptions)

public:

    /**
     * Options provided by the widget
     * @see options
     * @see setOptions
     */
    enum Option {
        ShowCalendar     = 0x00001,  /**< If the Calendar System edit is displayed */
        ShowDate         = 0x00002,  /**< If the Date is displayed */
        ShowTime         = 0x00004,  /**< If the Time is displayed */
        ShowTimeZone     = 0x00008,  /**< If the Time Zone is displayed */
        //EditCalendar     = 0x00010,  /**< Allow the user to manually edit the calendar */
        EditDate         = 0x00020,  /**< Allow the user to manually edit the date */
        EditTime         = 0x00040,  /**< Allow the user to manually edit the time */
        //EditTimeZone     = 0x00080,  /**< Allow the user to manually edit the time zone */
        SelectCalendar   = 0x00100,  /**< Allow the user to select a calendar */
        SelectDate       = 0x00200,  /**< Allow the user to select a date */
        SelectTime       = 0x00400,  /**< Allow the user to select a time */
        SelectTimeZone   = 0x00800,  /**< Allow the user to select a time zone */
        DatePicker       = 0x01000,  /**< Show a date picker */
        DateKeywords     = 0x02000,  /**< Show date keywords */
        ForceTime        = 0x04000,  /**< The entered time can only be a selected time */
        WarnOnInvalid    = 0x08000   /**< Show a warning on focus out if the date or time is invalid */
    };
    Q_DECLARE_FLAGS(Options, Option)
    Q_FLAG(Options)

    /**
     * Create a new KDateTimeEdit widget
     */
    explicit KDateTimeEdit(QWidget *parent = 0);

    /**
     * Destroy the widget
     */
    virtual ~KDateTimeEdit();

    /**
     * Return the currently set widget options
     *
     * @return the currently set widget options
     */
    Options options() const;

    /**
     * Return the currently selected date, time and time zone
     *
     * @return the currently selected date, time and time zone
     */
    QDateTime dateTime() const;

    /**
     * Return the currently selected date
     *
     * @return the currently selected date
     */
    QDate date() const;

    /**
     * Return the currently selected time
     *
     * @return the currently selected time
     */
    QTime time() const;

    /**
     * Return the currently selected time zone
     *
     * @return the currently selected time zone
     */
    QTimeZone timeZone() const;

    /**
     * Returns the list of Calendar Systems displayed.
     *
     * @param calendars the list of calendar systems to display
     */
    QList<QLocale> calendarLocalesList() const;

    /**
     * Return the current minimum date and time
     *
     * @return the current minimum date and time
     */
    QDateTime minimumDateTime() const;

    /**
     * Return the current maximum date and time
     *
     * @return the current maximum date and time
     */
    QDateTime maximumDateTime() const;

    /**
     * Return the currently set date display format
     *
     * By default this is the Short Format
     *
     * @return the currently set date format
     */
    QLocale::FormatType dateDisplayFormat() const;

    /**
     * Return the map of dates listed in the drop-down and their displayed
     * string forms.
     *
     * @see setDateMap()
     * @return the select date map
     */
    QMap<QDate, QString> dateMap() const;

    /**
     * Return the currently set time format
     *
     * By default this is the Short Format
     *
     * @return the currently set time format
     */
    QLocale::FormatType timeDisplayFormat() const;

    /**
     * Return the time list interval able to be selected
     *
     * @return the select time intervals in minutes
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

    /**
     * Return the list of time zones able to be selected
     *
     * @param zones the time zones to display
     */
    QList<QTimeZone> timeZones() const;

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
     * Return if the current user input date is valid
     *
     * If the user input date is null then it is not valid
     *
     * @see isNullDate()
     * @return if the current user input date is valid
     */
    bool isValidDate() const;

    /**
     * Return if the current user input date is null
     *
     * @see isValidDate()
     * @return if the current user input date is null
     */
    bool isNullDate() const;
    /**
     * Return if the current user input time is valid
     *
     * If the user input time is null then it is not valid
     *
     * @see isNullTime()
     * @return if the current user input time is valid
     */
    bool isValidTime() const;

    /**
     * Return if the current user input time is null
     *
     * @see isValidTime()
     * @return if the current user input time is null
     */
    bool isNullTime() const;

Q_SIGNALS:

    /**
     * Signal if the date or time has been manually entered by the user.
     *
     * The returned date and time may be invalid.
     *
     * @param dateTime the new date, time and time zone
     */
    void dateTimeEntered(const QDateTime &dateTime);

    /**
     * Signal if the date or time has been changed either manually by the user
     * or programatically.
     *
     * The returned date and time may be invalid.
     *
     * @param dateTime the new date, time and time zone
     */
    void dateTimeChanged(const QDateTime &dateTime);

    /**
     * Signal if the date or time is being manually edited by the user.
     *
     * The returned date and time may be invalid.
     *
     * @param dateTime the new date, time and time zone
     */
    void dateTimeEdited(const QDateTime &dateTime);

    /**
     * Signal if the Calendar Locale has been manually entered by the user.
     *
     * @param calendarLocale the new calendar locale
     */
    void calendarEntered(const QLocale &calendarLocale);

    /**
     * Signal if the Calendar Locale has been changed either manually by the user
     * or programatically.
     *
     * @param calendarLocale the new calendar locale
     */
    void calendarChanged(const QLocale &calendarLocale);

    /**
     * Signal if the date has been manually entered by the user.
     *
     * The returned date may be invalid.
     *
     * @param date the new date
     */
    void dateEntered(const QDate &date);

    /**
     * Signal if the date has been changed either manually by the user
     * or programatically.
     *
     * The returned date may be invalid.
     *
     * @param date the new date
     */
    void dateChanged(const QDate &date);

    /**
     * Signal if the date is being manually edited by the user.
     *
     * The returned date may be invalid.
     *
     * @param date the new date
     */
    void dateEdited(const QDate &date);

    /**
     * Signal if the time has been manually entered by the user.
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

    /**
     * Signal if the time zone has been changed manually by the user.
     *
     * @param timeZone the new time zone
     */
    void timeZoneEntered(const QTimeZone &zone);

    /**
     * Signal if the time zone has been changed either manually by the user
     * or programatically.
     *
     * @param timeZone the new time zone
     */
    void timeZoneChanged(const QTimeZone &zone);

public Q_SLOTS:

    /**
     * Set the new widget options
     *
     * @param options the new widget options
     */
    void setOptions(Options options);

    /**
     * Set the currently selected date, time and time zone
     *
     * @param dateTime the new date, time and time zone
     */
    void setDateTime(const QDateTime &dateTime);

    /**
     * Set the currently selected date
     *
     * @param date the new date
     */
    void setDate(const QDate &date);

    /**
     * Set the currently selected time
     *
     * @param time the new time
     */
    void setTime(const QTime &time);

    /**
     * Set the current time zone
     *
     * @param zone the new zone
     */
    void setTimeZone(const QTimeZone &zone);

    /**
     * Set the minimum and maximum date and time range
     *
     * To enable range checking provide two valid dates.
     * To disable range checking provide two invalid dates, or call
     * clearDateRange;
     *
     * @param minDateTime the minimum date and time
     * @param maxDateTime the maximum date and time
     * @param minWarnMsg the minimum warning message
     * @param maxWarnMsg the maximum warning message
     */
    void setDateTimeRange(const QDateTime &minDateTime,
                          const QDateTime &maxDateTime,
                          const QString &minWarnMsg = QString(),
                          const QString &maxWarnMsg = QString());

    /**
     * Reset the minimum and maximum date and time to the default
     */
    void resetDateTimeRange();

    /**
     * Set the minimum allowed date.
     *
     * If the date is invalid, or more than current maximum,
     * then the minimum will not be set.
     *
     * @see setMaximumDateTime()
     * @see setDateRange()
     * @param maxDate the minimum date
     * @param maxWarnMsg the minimum warning message
     */
    void setMinimumDateTime(const QDateTime &minDateTime, const QString &minWarnMsg = QString());

    /**
     * Reset the minimum date and time to the default
     */
    void resetMinimumDateTime();

    /**
     * Set the maximum allowed date.
     *
     * If the date is invalid, or less than current minimum,
     * then the maximum will not be set.
     *
     * @see setMinimumDateTime()
     * @see setDateRange()
     * @param maxDate the maximum date
     * @param maxWarnMsg the maximum warning message
     */
    void setMaximumDateTime(const QDateTime &maxDateTime, const QString &maxWarnMsg = QString());

    /**
     * Reset the minimum date and time to the default
     */
    void resetMaximumDateTime();

    /**
     * Sets the date format to display.
     *
     * By default is the Short Format.
     *
     * @param format the date format to use
     */
    void setDateDisplayFormat(QLocale::FormatType format);

    /**
     * Set the list of Calendar Locales to display.
     *
     * @param calendarLocales the list of calendar locales to display
     */
    void setCalendarLocalesList(const QList<QLocale> &calendarLocales);

    /**
     * Set the list of dates able to be selected from the drop-down and the
     * string form to display for those dates, e.g. "2010-01-01" and "Yesterday".
     *
     * Any invalid or duplicate dates will be used, the list will NOT be
     * sorted, and the minimum and maximum date will not be affected.
     *
     * The @p dateMap is keyed by the date to be listed and the value is the
     * string to be displayed.  If you want the date to be displayed in the
     * default date format then the string should be null.  If you want a
     * separator to be displayed then set the string to "separator".
     *
     * @see dateMap()
     * @param dateMap the map of dates able to be selected
     */
    void setDateMap(QMap<QDate, QString> dateMap);

    /**
     * Sets the time format to display.
     *
     * By default is the Short Format.
     *
     * @param format the time format to use
     */
    void setTimeDisplayFormat(QLocale::FormatType formatOptions);

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

    /**
     * Set the time zones able to be selected
     *
     * @param zones the time zones to display
     */
    void setTimeZones(const QList<QTimeZone> &zones);

protected:

    bool eventFilter(QObject *object, QEvent *event) Q_DECL_OVERRIDE;
    void focusInEvent(QFocusEvent *event) Q_DECL_OVERRIDE;
    void focusOutEvent(QFocusEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

    /**
     * Assign the date, time and time zone for the widget.
     *
     * Virtual to allow sub-classes to apply extra validation rules,
     * but reimplementations must call the parent method at the end.
     *
     * @param datetime the new date and time
     */
    virtual void assignDateTime(const QDateTime &dateTime);

    /**
     * Assign the date for the widget.
     *
     * Virtual to allow sub-classes to apply extra validation rules,
     * but reimplementations must call the parent method at the end.
     *
     * @param date the new date
     */
    virtual void assignDate(const QDate &date);

    /**
     * Assign the time for the widget.
     *
     * Virtual to allow sub-classes to apply extra validation rules,
     * but reimplementations must call the parent method at the end.
     *
     * @param time the new time
     */
    virtual void assignTime(const QTime &time);

    /**
     * Assign the time zone for the widget.
     *
     * Virtual to allow sub-classes to apply extra validation rules,
     * but reimplementations must call the parent method at the end.
     *
     * @param zone the new time zone
     */
    void assignTimeZone(const QTimeZone &zone);

private:

    friend class KDateTimeEditPrivate;
    KDateTimeEditPrivate *const d;

    Q_PRIVATE_SLOT(d, void selectCalendar(int))
    Q_PRIVATE_SLOT(d, void enterCalendar(const QLocale &))
    Q_PRIVATE_SLOT(d, void selectTimeZone(int))
    Q_PRIVATE_SLOT(d, void enterTimeZone(const QByteArray &))
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KDateTimeEdit::Options)

#endif // KDATETIMEEDIT_H
