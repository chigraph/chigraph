/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
                       1999 Waldo Bastian <bastian@kde.org>

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

#ifndef __kservicetype_h__
#define __kservicetype_h__

#include <ksycocaentry.h>

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QMap>
#include <QtCore/QVariant>

#include <kconfig.h>

class KDesktopFile;
class KServiceTypePrivate;

/**
 * A service type is, well, a type of service, where a service is an application or plugin.
 * For instance, "KOfficeFilter", which is the type of all koffice filters, is a service type.
 * In order to discover services of a given type, using KServiceTypeTrader.
 * Service types are stored as desktop files in $KDEDIR/share/servicetypes.
 * @see KService, KServiceTypeTrader
 */
class KSERVICE_EXPORT KServiceType : public KSycocaEntry // TODO KDE5: inherit kshared, but move KSycocaEntry to KServiceTypePrivate
{
public:
    typedef QExplicitlySharedDataPointer<KServiceType> Ptr;
    typedef QList<Ptr> List;

    /**
     * Construct a service type and take all information from a desktop file.
     * @param config the configuration file
     */
    explicit KServiceType(KDesktopFile *config);

    virtual ~KServiceType();

    /**
     * Returns the descriptive comment associated, if any.
     * @return the comment, or QString()
     */
    QString comment() const;

    /**
     * Checks whether this service type inherits another one.
     * @return true if this service type inherits another one
     * @see parentServiceType()
     */
    bool isDerived() const;

    /**
     * If this service type inherits from another service type,
     * return the name of the parent.
     * @return the parent service type, or QString:: null if not set
     * @see isDerived()
     */
    QString parentServiceType() const;

    /**
     * Checks whether this service type is or inherits from @p servTypeName.
     * @return true if this servicetype is or inherits from @p servTypeName
     */
    bool inherits(const QString &servTypeName) const;

    /**
     * Returns the type of the property definition with the given @p _name.
     *
     * @param _name the name of the property
     * @return the property type, or null if not found
     * @see propertyDefNames
     */
    QVariant::Type propertyDef(const QString &_name) const;

    /**
     * Returns the list of all property definitions for this servicetype.
     * Those are properties of the services implementing this servicetype.
     * For instance,
     * @code
     * [PropertyDef::X-KDevelop-Version]
     * Type=int
     * @endcode
     * means that all kdevelop plugins have in their .desktop file a line like
     * @code
     * X-KDevelop-Version=<some value>
     * @endcode
     */
    QStringList propertyDefNames() const;

    /// @internal (for KBuildServiceTypeFactory)
    QMap<QString, QVariant::Type> propertyDefs() const;

    /**
     * @internal
     * Pointer to parent service type
     */
    Ptr parentType();
    /**
     * @internal  only used by kbuildsycoca
     * Register offset into offers list
     */
    void setServiceOffersOffset(int offset);
    /**
     * @internal
     */
    int serviceOffersOffset() const;

    /**
     * Returns a pointer to the servicetype '_name' or 0L if the
     *         service type is unknown.
     * VERY IMPORTANT : don't store the result in a KServiceType * !
     * @param _name the name of the service type to search
     * @return the pointer to the service type, or 0
     */
    static Ptr serviceType(const QString &_name);

    /**
     * Returns a list of all the supported servicetypes. Useful for
     *         showing the list of available servicetypes in a listbox,
     *         for example.
     * More memory consuming than the ones above, don't use unless
     * really necessary.
     * @return the list of all services
     */
    static List allServiceTypes();

private:
    friend class KServiceTypeFactory;
    /**
     * @internal construct a service from a stream.
     * The stream must already be positionned at the correct offset
     */
    KServiceType(QDataStream &_str, int offset);

    Q_DECLARE_PRIVATE(KServiceType)
};

//QDataStream& operator>>( QDataStream& _str, KServiceType& s );
//QDataStream& operator<<( QDataStream& _str, KServiceType& s );

#endif
