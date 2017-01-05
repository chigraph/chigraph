/* This file is part of the KDE libraries
   Copyright (C) 2000 Torben Weis <weis@kde.org>
   Copyright (C) 2006 David Faure <faure@kde.org>

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

#ifndef KMIMETYPETRADER_H
#define KMIMETYPETRADER_H

#include <kservice.h>
class KMimeTypeTraderPrivate;
class KServiceOffer;
typedef QList<KServiceOffer> KServiceOfferList;

/**
 * KDE's trader for services associated to a given mimetype.
 *
 * Example: say that you want to the list of all KParts components that can handle HTML.
 * Our code would look like:
 * \code
 * KService::List lst = KMimeTypeTrader::self()->query("text/html",
 *                                                        "KParts/ReadOnlyPart");
 * \endcode
 *
 * If you want to get the preferred KParts component for text/html you could use
 * preferredService("text/html", "KParts/ReadOnlyPart"), although if this is about
 * loading that component you would use createPartInstanceFromQuery directly.
 *
 * @see KServiceTypeTrader, KService
 */
class KSERVICE_EXPORT KMimeTypeTrader
{
public:

    /**
     * Standard destructor
     */
    ~KMimeTypeTrader();

    /**
     * This method returns a list of services which are associated with a given mimetype.
     *
     * Example usage:
     * To get list of applications that can handle a given mimetype,
     * set @p genericServiceType to "Application" (which is the default).
     * To get list of embeddable components that can handle a given mimetype,
     * set @p genericServiceType to "KParts/ReadOnlyPart".
     *
     * The constraint parameter is used to limit the possible choices
     * returned based on the constraints you give it.
     *
     * The @p constraint language is rather full.  The most common
     * keywords are AND, OR, NOT, IN, and EXIST, all used in an
     * almost spoken-word form.  An example is:
     * \code
     * (Type == 'Service') and (('Browser/View' in ServiceTypes) and (exist Library))
     * \endcode
     *
     * The keys used in the query (Type, ServiceTypes, Library) are all
     * fields found in the .desktop files.
     *
     * @param mimeType A mime type like 'text/plain' or 'text/html'.
     * @param genericServiceType a basic service type, like 'KParts/ReadOnlyPart' or 'Application'
     * @param constraint  A constraint to limit the choices returned, QString() to
     *                    get all services that can handle the given @p mimetype
     *
     * @return A list of services that satisfy the query, sorted by preference
     * (preferred service first)
     * @see http://techbase.kde.org/Development/Tutorials/Services/Traders#The_KTrader_Query_Language
     */
    KService::List query(const QString &mimeType,
                         const QString &genericServiceType = QStringLiteral("Application"),
                         const QString &constraint = QString()) const;

    /**
     * Returns the preferred service for @p mimeType and @p genericServiceType
     *
     * This is almost like query().first(), except that it also checks
     * if the service is allowed as a preferred service (see KService::allowAsDefault).
     *
     * @param mimeType the mime type (see query())
     * @param genericServiceType the service type (see query())
     * @return the preferred service, or 0 if no service is available
     */
    KService::Ptr preferredService(const QString &mimeType, const QString &genericServiceType = QStringLiteral("Application"));

    /**
     * This method creates and returns a part object from the trader query for a given \p mimeType.
     *
     * Example:
     * \code
     * KParts::ReadOnlyPart* part = KMimeTypeTrader::createInstanceFromQuery<KParts::ReadOnlyPart>("text/plain", parentWidget, parentObject);
     * if (part) {
     *     part->openUrl(url);
     *     part->widget()->show();  // also insert the widget into a layout
     * }
     * \endcode
     *
     * @param mimeType the mimetype which this part is associated with
     * @param parentWidget the parent widget, will be set as the parent of the part's widget
     * @param parent the parent object for the part itself
     * @param constraint an optional constraint to pass to the trader
     * @param args A list of arguments passed to the service component
     * @param error The string passed here will contain an error description.
     * @return A pointer to the newly created object or a null pointer if the
     *         factory was unable to create an object of the given type.
     */
    template <class T>
    static T *createPartInstanceFromQuery(const QString &mimeType, QWidget *parentWidget = 0, QObject *parent = 0,
                                          const QString &constraint = QString(),
                                          const QVariantList &args = QVariantList(),
                                          QString *error = 0)
    {
        const KService::List offers = self()->query(mimeType, QStringLiteral("KParts/ReadOnlyPart"), constraint);
        Q_FOREACH (const KService::Ptr &ptr, offers) {
            T *component = ptr->template createInstance<T>(parentWidget, parent, args, error);
            if (component) {
                if (error) {
                    error->clear();
                }
                return component;
            }
        }
        if (error) {
            *error = QCoreApplication::translate("", "No service matching the requirements was found");
        }
        return 0;
    }

    /**
     * This can be used to create a service instance from a mime type query
     *
     * @param mimeType A mime type like 'text/plain' or 'text/html'.
     * @param serviceType a basic service type
     * @param parent the parent object for the plugin itself
     * @param constraint  A constraint to limit the choices returned, QString() to
     *                    get all services that can handle the given @p mimetype
     * @param args A list of arguments passed to the service component
     * @param error The string passed here will contain an error description.
     * @return A pointer to the newly created object or a null pointer if the
     *         factory was unable to create an object of the given type.
     */
    template <class T>
    static T *createInstanceFromQuery(const QString &mimeType, const QString &serviceType, QObject *parent = 0,
                                      const QString &constraint = QString(),
                                      const QVariantList &args = QVariantList(),
                                      QString *error = 0)
    {
        const KService::List offers = self()->query(mimeType, serviceType, constraint);
        Q_FOREACH (const KService::Ptr &ptr, offers) {
            T *component = ptr->template createInstance<T>(parent, args, error);
            if (component) {
                if (error) {
                    error->clear();
                }
                return component;
            }
        }
        if (error) {
            *error = QCoreApplication::translate("", "No service matching the requirements was found");
        }
        return 0;
    }

    /**
     * This is a static pointer to the KMimeTypeTrader singleton.
     *
     * You will need to use this to access the KMimeTypeTrader functionality since the
     * constructors are protected.
     *
     * @return Static KMimeTypeTrader instance
     */
    static KMimeTypeTrader *self();

private:
    /**
     * @internal
     */
    KMimeTypeTrader();

private:
    KMimeTypeTraderPrivate *const d;

    // class-static so that it can access KSycocaEntry::offset()
    static void filterMimeTypeOffers(KServiceOfferList &list, const QString &genericServiceType);
    static void filterMimeTypeOffers(KService::List &list, const QString &genericServiceType);
    friend class KMimeTypeTraderSingleton;
};

#endif /* KMIMETYPETRADER_H */
