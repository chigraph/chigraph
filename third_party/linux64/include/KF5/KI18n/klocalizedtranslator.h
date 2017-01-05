/*
 * Copyright 2014  Martin Gräßlin <mgraesslin@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef KLOCALIZEDTRANSLATOR_H
#define KLOCALIZEDTRANSLATOR_H

#include <ki18n_export.h>

#include <QtCore/QTranslator>

class KLocalizedTranslatorPrivate;

/**
 * @brief A QTranslator using KLocalizedString for translations.
 *
 * This class allows to translate strings in Qt's translation system with KLocalizedString.
 * An example is the translation of a dynamically loaded user interface through QUILoader.
 *
 * To use this Translator install it in the QCoreApplication and provide the translation domain
 * to be used. The Translator can operate for multiple contexts, those needs to be specified.
 *
 * Example for translating a UI loaded through QUILoader:
 * @code
 * // create translator and install in QCoreApplication
 * KLocalizedTranslator *translator = new KLocalizedTranslator(this);
 * QCoreApplication::instance()->installTranslator(translator);
 * translator->setTranslationDomain(QStringLiteral("MyAppsDomain"));
 *
 * // create the QUILoader
 * QUiLoader *loader = new QUiLoader(this);
 * loader->setLanguageChangeEnabled(true);
 *
 * // load the UI
 * QFile uiFile(QStringLiteral("/path/to/userInterface.ui"));
 * uiFile.open(QFile::ReadOnly);
 * QWidget *loadedWidget = loader->load(&uiFile, this);
 * uiFile.close();
 *
 * // the object name of the loaded UI is the context in this case
 * translator->addContextToMonitor(loadedWidget->objectName());
 *
 * // send a LanguageChange event, this will re-translate using our translator
 * QEvent le(QEvent::LanguageChange);
 * QCoreApplication::sendEvent(loadedWidget, &le);
 * @endcode
 *
 * @since 5.0
 **/
class KI18N_EXPORT KLocalizedTranslator : public QTranslator
{
    Q_OBJECT
public:
    explicit KLocalizedTranslator(QObject *parent = 0);
    virtual ~KLocalizedTranslator();
    QString translate(const char *context, const char *sourceText, const char *disambiguation = 0, int n = -1) const Q_DECL_OVERRIDE;

    /**
     * Sets the @p translationDomain to be used.
     *
     * The translation domain is required. Without the translation domain any invocation of
     * translate() will be delegated to the base class.
     *
     * @param translationDomain The translation domain to be used.
     **/
    void setTranslationDomain(const QString &translationDomain);

    /**
     * Adds a @p context for which this Translator should be active.
     *
     * The Translator only translates texts with a context matching one of the monitored contexts.
     * If the context is not monitored, the translate() method delegates to the base class.
     *
     * @param context The context for which the Translator should be active
     *
     * @see removeContextToMonitor
     **/
    void addContextToMonitor(const QString &context);

    /**
     * Stop translating for the given @p context.
     *
     * @param context The context for which the Translator should no longer be active
     *
     * @see addContextToMonitor
     **/
    void removeContextToMonitor(const QString &context);

private:
    const QScopedPointer<KLocalizedTranslatorPrivate> d;
};

#endif //KLOCALIZEDTRANSLATOR_H
