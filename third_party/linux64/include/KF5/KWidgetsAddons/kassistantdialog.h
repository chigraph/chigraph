/* This file is part of the KDE libraries
    Copyright (C) 2006 Olivier Goffart <ogoffart at kde.org>

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

#ifndef KASSISTANTDIALOG_H
#define KASSISTANTDIALOG_H

#include <kpagedialog.h>

#include <kwidgetsaddons_export.h>

/**
 * This class provides a framework for assistant dialogs.
 *
 * An assistant dialog consists of a sequence of pages.
 * Its purpose is to guide the user (assist) through a process step by step.
 * Assistant dialogs are useful for complex or infrequently occurring tasks
 * that people may find difficult to learn or do.
 * Sometimes a task requires too many input fields to fit them on a single dialog.
 *
 * Create and populate dialog pages that inherit from QWidget and add them
 * to the assistant dialog using addPage().
 *
 * The functions next() and back() are virtual and may be reimplemented to
 * override the default actions of the next and back buttons.
 *
 * \image html kassistantdialog.png "KDE Assistant Dialog"
 *
 * @author Olivier Goffart <ogoffart at kde.org>
 */
class KWIDGETSADDONS_EXPORT KAssistantDialog : public KPageDialog
{
    Q_OBJECT
public:
    /**
     * Construct a new assistant dialog with @p parent as parent.
     * @param parent is the parent of the widget.
     * @flags the window flags to give to the assistant dialog. The
     * default of zero is usually what you want.
     */
    explicit KAssistantDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    virtual ~KAssistantDialog();

    /**
     * Specify if the content of the page is valid, and if the next button may be enabled on this page.
     * By default all pages are valid.
     *
     * This will disable or enable the next button on the specified page
     *
     * @param page the page on which the next button will be enabled/disable
     * @param enable if true the next button will be enabled, if false it will be disabled
     */
    void setValid(KPageWidgetItem *page, bool enable);

    /**
     * return if a page is valid
     * @see setValid
     * @param page the page to check the validity of
     */
    bool isValid(KPageWidgetItem *page) const;

    /**
     * Specify whether a page is appropriate.
     *
     * A page is considered inappropriate if it should not be shown due to
     * the contents of other pages making it inappropriate.
     *
     * A page which is inappropriate will not be shown.
     *
     * The last page in an assistant dialog should always be appropriate
     * @param page the page to set as appropriate
     * @param appropriate flag indicating the appropriateness of the page.
     * If @p appropriate is true, then @p page is appropriate and will be
     * shown in the assistant dialog. If false, @p page will not be shown.
     */
    void setAppropriate(KPageWidgetItem *page, bool appropriate);

    /**
     * Check if a page is appropriate for use in the assistant dialog.
     * @param page is the page to check the appropriateness of.
     * @return true if @p page is appropriate, false if it is not
     */
    bool isAppropriate(KPageWidgetItem *page) const;

    /**
     * @returns the next button
     */
    QPushButton* nextButton() const;

    /**
     * @returns the finish button
     */
    QPushButton* backButton() const;

    /**
     * @returns the finish button
     */
    QPushButton* finishButton() const;

public Q_SLOTS:
    /**
     * Called when the user clicks the Back button.
     *
     * This function will show the preceding relevant page in the sequence.
     * Do nothing if the current page is the first page in the sequence.
     */
    virtual void back();

    /**
     * Called when the user clicks the Next/Finish button.
     *
     * This function will show the next relevant page in the sequence.
     * If the current page is the last page, it will call accept()
     */
    virtual void next();

protected:
    /**
     * Construct an assistant dialog from a single widget.
     * @param widget the widget to construct the dialog with
     * @param parent the parent of the assistant dialog
     * @flags the window flags to use when creating the widget. The default
     * of zero is usually fine.
     *
     * Calls the KPageDialog(KPageWidget *widget, QWidget *parent, Qt::WindowFlags flags) constructor
     */
    explicit KAssistantDialog(KPageWidget *widget, QWidget *parent = 0, Qt::WindowFlags flags = 0);

    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void _k_slotUpdateButtons())

    Q_DISABLE_COPY(KAssistantDialog)
};

#endif
