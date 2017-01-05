/*
 *   Copyright 2011 Marco Martin <mart@kde.org>
 *   Copyright 2015 Aleix Pol Gonzalez <aleixpol@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef KLOCALIZEDCONTEXT_H
#define KLOCALIZEDCONTEXT_H

#include <QObject>
#include <QVariant>
#include <ki18n_export.h>

/**
 * This class is meant to be used to simplify integration of the KI18n framework
 * in QML.
 *
 * The way to do so, is by creating this object and setting it as a context
 * object:
 *
 * @code
 * QQuickView* view = new QQuickView;
 * view.engine()->rootContext()->setContextObject(new KLocalizedContext(view));
 * @endcode
 *
 * Then i18n*() and xi18n*() functions should be available for use from the code
 * loaded in the engine, for the view.
 *
 * @note Plural functions differ from the C/C++ version. On QML/JS we can get a
 * real value easily. To solve warnings on those cases we'll cast the first argument
 * to make sure it's taken into account for the plural.
 *
 * @since 5.17
 */

class KI18N_EXPORT KLocalizedContext : public QObject
{
    Q_OBJECT

    /**
     * This property only needs to be specified if the context is being run on a library.
     * in an application there is no need to set the translation domain as the application's
     * domain can be used.
     */
    Q_PROPERTY(QString translationDomain READ translationDomain WRITE setTranslationDomain NOTIFY translationDomainChanged)

public:
    explicit KLocalizedContext(QObject *parent = Q_NULLPTR);
    ~KLocalizedContext() Q_DECL_OVERRIDE;

    QString translationDomain() const;
    void setTranslationDomain(const QString &domain);

    Q_INVOKABLE QString i18n(const QString &message,
                             const QVariant &p1 = QVariant(), const QVariant &p2 = QVariant(), const QVariant &p3 = QVariant(), const QVariant &p4 = QVariant(), const QVariant &p5 = QVariant(),
                             const QVariant &p6 = QVariant(), const QVariant &p7 = QVariant(), const QVariant &p8 = QVariant(), const QVariant &p9 = QVariant(), const QVariant &p10 = QVariant()) const;

    Q_INVOKABLE QString i18nc(const QString &context, const QString &message,
                              const QVariant &p1 = QVariant(), const QVariant &p2 = QVariant(), const QVariant &p3 = QVariant(), const QVariant &p4 = QVariant(), const QVariant &p5 = QVariant(),
                              const QVariant &p6 = QVariant(), const QVariant &p7 = QVariant(), const QVariant &p8 = QVariant(), const QVariant &p9 = QVariant(), const QVariant &p10 = QVariant()) const;

    Q_INVOKABLE QString i18np(const QString &singular, const QString &plural,
                              const QVariant &p1 = QVariant(), const QVariant &p2 = QVariant(), const QVariant &p3 = QVariant(), const QVariant &p4 = QVariant(), const QVariant &p5 = QVariant(),
                              const QVariant &p6 = QVariant(), const QVariant &p7 = QVariant(), const QVariant &p8 = QVariant(), const QVariant &p9 = QVariant(), const QVariant &p10 = QVariant()) const;

    Q_INVOKABLE QString i18ncp(const QString &context, const QString &singular, const QString &plural,
                               const QVariant &p1 = QVariant(), const QVariant &p2 = QVariant(), const QVariant &p3 = QVariant(), const QVariant &p4 = QVariant(), const QVariant &p5 = QVariant(),
                               const QVariant &p6 = QVariant(), const QVariant &p7 = QVariant(), const QVariant &p8 = QVariant(), const QVariant &p9 = QVariant(), const QVariant &p10 = QVariant()) const;

    Q_INVOKABLE QString i18nd(const QString &domain, const QString &message,
                              const QVariant &p1 = QVariant(), const QVariant &p2 = QVariant(), const QVariant &p3 = QVariant(), const QVariant &p4 = QVariant(), const QVariant &p5 = QVariant(),
                              const QVariant &p6 = QVariant(), const QVariant &p7 = QVariant(), const QVariant &p8 = QVariant(), const QVariant &p9 = QVariant(), const QVariant &p10 = QVariant()) const;

    Q_INVOKABLE QString i18ndc(const QString &domain, const QString &context, const QString &message,
                               const QVariant &p1 = QVariant(), const QVariant &p2 = QVariant(), const QVariant &p3 = QVariant(), const QVariant &p4 = QVariant(), const QVariant &p5 = QVariant(),
                               const QVariant &p6 = QVariant(), const QVariant &p7 = QVariant(), const QVariant &p8 = QVariant(), const QVariant &p9 = QVariant(), const QVariant &p10 = QVariant()) const;

    Q_INVOKABLE QString i18ndp(const QString &domain, const QString &singular, const QString &plural,
                               const QVariant &p1 = QVariant(), const QVariant &p2 = QVariant(), const QVariant &p3 = QVariant(), const QVariant &p4 = QVariant(), const QVariant &p5 = QVariant(),
                               const QVariant &p6 = QVariant(), const QVariant &p7 = QVariant(), const QVariant &p8 = QVariant(), const QVariant &p9 = QVariant(), const QVariant &p10 = QVariant()) const;

    Q_INVOKABLE QString i18ndcp(const QString &domain, const QString &context, const QString &singular, const QString &plural,
                                const QVariant &p1 = QVariant(), const QVariant &p2 = QVariant(), const QVariant &p3 = QVariant(), const QVariant &p4 = QVariant(), const QVariant &p5 = QVariant(),
                                const QVariant &p6 = QVariant(), const QVariant &p7 = QVariant(), const QVariant &p8 = QVariant(), const QVariant &p9 = QVariant(), const QVariant &p10 = QVariant()) const;

    Q_INVOKABLE QString xi18n(const QString &message,
                             const QVariant &p1 = QVariant(), const QVariant &p2 = QVariant(), const QVariant &p3 = QVariant(), const QVariant &p4 = QVariant(), const QVariant &p5 = QVariant(),
                             const QVariant &p6 = QVariant(), const QVariant &p7 = QVariant(), const QVariant &p8 = QVariant(), const QVariant &p9 = QVariant(), const QVariant &p10 = QVariant()) const;

    Q_INVOKABLE QString xi18nc(const QString &context, const QString &message,
                              const QVariant &p1 = QVariant(), const QVariant &p2 = QVariant(), const QVariant &p3 = QVariant(), const QVariant &p4 = QVariant(), const QVariant &p5 = QVariant(),
                              const QVariant &p6 = QVariant(), const QVariant &p7 = QVariant(), const QVariant &p8 = QVariant(), const QVariant &p9 = QVariant(), const QVariant &p10 = QVariant()) const;

    Q_INVOKABLE QString xi18np(const QString &singular, const QString &plural,
                              const QVariant &p1 = QVariant(), const QVariant &p2 = QVariant(), const QVariant &p3 = QVariant(), const QVariant &p4 = QVariant(), const QVariant &p5 = QVariant(),
                              const QVariant &p6 = QVariant(), const QVariant &p7 = QVariant(), const QVariant &p8 = QVariant(), const QVariant &p9 = QVariant(), const QVariant &p10 = QVariant()) const;

    Q_INVOKABLE QString xi18ncp(const QString &context, const QString &singular, const QString &plural,
                               const QVariant &p1 = QVariant(), const QVariant &p2 = QVariant(), const QVariant &p3 = QVariant(), const QVariant &p4 = QVariant(), const QVariant &p5 = QVariant(),
                               const QVariant &p6 = QVariant(), const QVariant &p7 = QVariant(), const QVariant &p8 = QVariant(), const QVariant &p9 = QVariant(), const QVariant &p10 = QVariant()) const;

    Q_INVOKABLE QString xi18nd(const QString &domain, const QString &message,
                              const QVariant &p1 = QVariant(), const QVariant &p2 = QVariant(), const QVariant &p3 = QVariant(), const QVariant &p4 = QVariant(), const QVariant &p5 = QVariant(),
                              const QVariant &p6 = QVariant(), const QVariant &p7 = QVariant(), const QVariant &p8 = QVariant(), const QVariant &p9 = QVariant(), const QVariant &p10 = QVariant()) const;

    Q_INVOKABLE QString xi18ndc(const QString &domain, const QString &context, const QString &message,
                               const QVariant &p1 = QVariant(), const QVariant &p2 = QVariant(), const QVariant &p3 = QVariant(), const QVariant &p4 = QVariant(), const QVariant &p5 = QVariant(),
                               const QVariant &p6 = QVariant(), const QVariant &p7 = QVariant(), const QVariant &p8 = QVariant(), const QVariant &p9 = QVariant(), const QVariant &p10 = QVariant()) const;

    Q_INVOKABLE QString xi18ndp(const QString &domain, const QString &singular, const QString &plural,
                               const QVariant &p1 = QVariant(), const QVariant &p2 = QVariant(), const QVariant &p3 = QVariant(), const QVariant &p4 = QVariant(), const QVariant &p5 = QVariant(),
                               const QVariant &p6 = QVariant(), const QVariant &p7 = QVariant(), const QVariant &p8 = QVariant(), const QVariant &p9 = QVariant(), const QVariant &p10 = QVariant()) const;

    Q_INVOKABLE QString xi18ndcp(const QString &domain, const QString &context, const QString &singular, const QString &plural,
                                const QVariant &p1 = QVariant(), const QVariant &p2 = QVariant(), const QVariant &p3 = QVariant(), const QVariant &p4 = QVariant(), const QVariant &p5 = QVariant(),
                                const QVariant &p6 = QVariant(), const QVariant &p7 = QVariant(), const QVariant &p8 = QVariant(), const QVariant &p9 = QVariant(), const QVariant &p10 = QVariant()) const;

Q_SIGNALS:
    void translationDomainChanged(const QString& translationDomain);

private:
    class KLocalizedContextPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(KLocalizedContext)
};

#endif
