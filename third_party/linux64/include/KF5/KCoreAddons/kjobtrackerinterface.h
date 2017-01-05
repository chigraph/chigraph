/*  This file is part of the KDE project
    Copyright (C) 2007 Kevin Ottens <ervin@kde.org>

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

#ifndef KJOBTRACKERINTERFACE_H
#define KJOBTRACKERINTERFACE_H

#include <kcoreaddons_export.h>
#include <kjob.h>

#include <QtCore/QObject>
#include <QtCore/QPair>

/**
 * The interface to implement to track the progresses of a job.
 */
class KCOREADDONS_EXPORT KJobTrackerInterface : public QObject
{
    Q_OBJECT

public:
    /**
     * Creates a new KJobTrackerInterface
     *
     * @param parent the parent object
     */
    KJobTrackerInterface(QObject *parent = 0);

    /**
     * Destroys a KJobTrackerInterface
     */
    virtual ~KJobTrackerInterface();

public Q_SLOTS:
    /**
     * Register a new job in this tracker.
     *
     * @param job the job to register
     */
    virtual void registerJob(KJob *job);

    /**
     * Unregister a job from this tracker.
     *
     * @param job the job to unregister
     */
    virtual void unregisterJob(KJob *job);

protected Q_SLOTS:
    /**
     * Called when a job is finished, in any case. It is used to notify
     * that the job is terminated and that progress UI (if any) can be hidden.
     *
     * @param job the job that emitted this signal
     */
    virtual void finished(KJob *job);

    /**
     * Called when a job is suspended.
     *
     * @param job the job that emitted this signal
     */
    virtual void suspended(KJob *job);

    /**
     * Called when a job is resumed.
     *
     * @param job the job that emitted this signal
     */
    virtual void resumed(KJob *job);

    /**
     * Called to display general description of a job. A description has
     * a title and two optional fields which can be used to complete the
     * description.
     *
     * Examples of titles are "Copying", "Creating resource", etc.
     * The fields of the description can be "Source" with an URL, and,
     * "Destination" with an URL for a "Copying" description.
     * @param job the job that emitted this signal
     * @param title the general description of the job
     * @param field1 first field (localized name and value)
     * @param field2 second field (localized name and value)
     */
    virtual void description(KJob *job, const QString &title,
                             const QPair<QString, QString> &field1,
                             const QPair<QString, QString> &field2);

    /**
     * Called to display state information about a job.
     * Examples of message are "Resolving host", "Connecting to host...", etc.
     *
     * @param job the job that emitted this signal
     * @param plain the info message
     * @param rich the rich text version of the message, or QString() is none is available
     */
    virtual void infoMessage(KJob *job, const QString &plain, const QString &rich);

    /**
     * Emitted to display a warning about a job.
     *
     * @param job the job that emitted this signal
     * @param plain the warning message
     * @param rich the rich text version of the message, or QString() is none is available
     */
    virtual void warning(KJob *job, const QString &plain, const QString &rich);

    /**
     * Called when we know the amount a job will have to process. The unit of this
     * amount is provided too. It can be called several times for a given job if the job
     * manages several different units.
     *
     * @param job the job that emitted this signal
     * @param unit the unit of the total amount
     * @param amount the total amount
     */
    virtual void totalAmount(KJob *job, KJob::Unit unit, qulonglong amount);

    /**
     * Regularly called to show the progress of a job by giving the current amount.
     * The unit of this amount is provided too. It can be called several times for a given
     * job if the job manages several different units.
     *
     * @param job the job that emitted this signal
     * @param unit the unit of the processed amount
     * @param amount the processed amount
     */
    virtual void processedAmount(KJob *job, KJob::Unit unit, qulonglong amount);

    /**
     * Called to show the overall progress of the job.
     * Note that this is not called for finished jobs.
     *
     * @param job the job that emitted this signal
     * @param percent the percentage
     */
    virtual void percent(KJob *job, unsigned long percent);

    /**
     * Called to show the speed of the job.
     *
     * @param job the job that emitted this signal
     * @param value the current speed of the job
     */
    virtual void speed(KJob *job, unsigned long value);

private:
    class Private;
    Private *const d;
};

#endif
