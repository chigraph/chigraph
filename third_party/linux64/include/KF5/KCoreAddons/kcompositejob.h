/*  This file is part of the KDE project
    Copyright (C) 2006 Kevin Ottens <ervin@kde.org>

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

#ifndef KCOMPOSITEJOB_H
#define KCOMPOSITEJOB_H

#include <kcoreaddons_export.h>
#include <kjob.h>

#include <QtCore/QList>

class KCompositeJobPrivate;
/**
 * The base class for all jobs able to be composed of one
 * or more subjobs.
 */
class KCOREADDONS_EXPORT KCompositeJob : public KJob
{
    Q_OBJECT

public:
    /**
     * Creates a new KCompositeJob object.
     *
     * @param parent the parent QObject
     */
    explicit KCompositeJob(QObject *parent = 0);

    /**
     * Destroys a KCompositeJob object.
     */
    virtual ~KCompositeJob();

protected:
    /**
     * Add a job that has to be finished before a result
     * is emitted. This has obviously to be called before
     * the result has been emitted by the job.
     *
     * Note that the composite job takes ownership of @p job
     *
     * @param job the subjob to add
     * @return true if the job has been added correctly, false otherwise
     */
    virtual bool addSubjob(KJob *job);

    /**
     * Mark a sub job as being done.
     *
     * The ownership of @p job is passed on to the caller.
     *
     * @param job the subjob to remove
     * @return true if the job has been removed correctly, false otherwise
     */
    virtual bool removeSubjob(KJob *job);

    /**
     * Checks if this job has subjobs running.
     *
     * @return true if we still have subjobs running, false otherwise
     */
    bool hasSubjobs() const;

    /**
     * Retrieves the list of the subjobs.
     *
     * @return the full list of sub jobs
     */
    const QList<KJob *> &subjobs() const;

    /**
     * Clears the list of subjobs.
     *
     * Note that this will *not* delete the subjobs.
     * Ownership of the subjobs is passed on to the caller.
     */
    void clearSubjobs();

protected Q_SLOTS:
    /**
     * Called whenever a subjob finishes.
     * Default implementation checks for errors and propagates
     * to parent job, and in all cases it calls removeSubjob.
     *
     * @param job the subjob
     */
    virtual void slotResult(KJob *job);

    /**
     * Forward signal from subjob.
     *
     * @param job the subjob
     * @param plain the info message in plain text version
     * @param rich the info message in rich text version
     * @see infoMessage()
     */
    virtual void slotInfoMessage(KJob *job, const QString &plain, const QString &rich);

protected:
    KCompositeJob(KCompositeJobPrivate &dd, QObject *parent);
private:
    Q_DECLARE_PRIVATE(KCompositeJob)
};

#endif
