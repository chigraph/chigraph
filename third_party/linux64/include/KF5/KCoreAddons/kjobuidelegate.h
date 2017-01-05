/* This file is part of the KDE libraries
    Copyright (C) 2000 Stephan Kulow <coolo@kde.org>
                       David Faure <faure@kde.org>
    Copyright (C) 2006 Kevin Ottens <ervin@kde.org>

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

#ifndef KJOBUIDELEGATE_H
#define KJOBUIDELEGATE_H

#include <kcoreaddons_export.h>
#include <QtCore/QObject>

class KJob;

/**
 * The base class for all KJob UI delegate.
 *
 * A UI delegate is responsible for the events of a
 * job and provides a UI for them (an error message
 * box or warning etc.).
 *
 * @see KJob
 */
class KCOREADDONS_EXPORT KJobUiDelegate : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructs a new KJobUiDelegate.
     */
    KJobUiDelegate();

    /**
     * Destroys a KJobUiDelegate.
     */
    virtual ~KJobUiDelegate();

protected:
    /**
     * Attach this UI delegate to a job. Once attached it'll track the job events.
     *
     * @return true if the job we're correctly attached to the job, false otherwise.
     */
    virtual bool setJob(KJob *job);

protected:
    /**
     * Retrieves the current job this UI delegate is attached to.
     *
     * @return current job this UI delegate is attached to, or 0 if
     * this UI delegate is not tracking any job
     */
    KJob *job() const;

    friend class KJob;

public:
    /**
     * Display a dialog box to inform the user of the error given by
     * this job.
     * Only call if error is not 0, and only in the slot connected
     * to result.
     */
    virtual void showErrorMessage();

    /**
     * Enable or disable the automatic error handling. When automatic
     * error handling is enabled and an error occurs, then showErrorDialog()
     * is called, right before the emission of the result signal.
     *
     * The default is false.
     *
     * See also isAutoErrorHandlingEnabled , showErrorDialog
     *
     * @param enable enable or disable automatic error handling
     * @see isAutoErrorHandlingEnabled()
     */
    void setAutoErrorHandlingEnabled(bool enable);

    /**
     * Returns whether automatic error handling is enabled or disabled.
     * See also setAutoErrorHandlingEnabled .
     * @return true if automatic error handling is enabled
     * @see setAutoErrorHandlingEnabled()
     */
    bool isAutoErrorHandlingEnabled() const;

    /**
     * Enable or disable the automatic warning handling. When automatic
     * warning handling is enabled and an error occurs, then a message box
     * is displayed with the warning message
     *
     * The default is true.
     *
     * See also isAutoWarningHandlingEnabled , showErrorDialog
     *
     * @param enable enable or disable automatic warning handling
     * @see isAutoWarningHandlingEnabled()
     */
    void setAutoWarningHandlingEnabled(bool enable);

    /**
     * Returns whether automatic warning handling is enabled or disabled.
     * See also setAutoWarningHandlingEnabled .
     * @return true if automatic warning handling is enabled
     * @see setAutoWarningHandlingEnabled()
     */
    bool isAutoWarningHandlingEnabled() const;

protected Q_SLOTS:
    virtual void slotWarning(KJob *job, const QString &plain, const QString &rich);

private:
    void connectJob(KJob *job);

    Q_PRIVATE_SLOT(d, void _k_result(KJob *))

    class Private;
    Private *const d;
};

#endif // KJOBUIDELEGATE_H
