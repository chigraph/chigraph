/*-
 * Copyright (c) 2000 Alex Zepeda <zipzippy@sonic.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#ifndef _KEMAILSETTINGS_H
#define _KEMAILSETTINGS_H

#include <QtCore/QCoreApplication> // Q_DECLARE_TR_FUNCTIONS
#include <QtCore/QStringList>

#include <kconfigcore_export.h>

class KEMailSettingsPrivate;

/**
  * This is just a small class to facilitate accessing e-mail settings in
  * a sane way, and allowing any program to manage multiple e-mail
  * profiles effortlessly
  *
  * The default profile is automatically selected in the constructor.
  *
  * @author Alex Zepeda zipzippy@sonic.net
  **/
class KCONFIGCORE_EXPORT KEMailSettings
{
    Q_DECLARE_TR_FUNCTIONS(KEMailSettings)
public:
    /**
      * The list of settings that I thought of when I wrote this
      * class.  Any extra settings thought of later can be accessed
      * easily with getExtendedSetting and setExtendedSetting.
      * @see getSetting()
      * @see setSetting()
      * @see getExtendedSetting()
      * @see setExtendedSetting()
      **/
    enum Setting {
        ClientProgram,
        ClientTerminal,
        RealName,
        EmailAddress,
        ReplyToAddress,
        Organization,
        OutServer,
        OutServerLogin,
        OutServerPass,
#ifndef KDE_NO_DEPRECATED
        /**
         * @deprecated since Frameworks 5.0
         */
        OutServerType,
        /**
         * @deprecated since Frameworks 5.0
         */
        OutServerCommand,
        /**
         * @deprecated since Frameworks 5.0
         */
        OutServerTLS,
#endif
        InServer,
        InServerLogin,
        InServerPass,
#ifndef KDE_NO_DEPRECATED
        /**
         * @deprecated since Frameworks 5.0
         */
        InServerType,
        /**
         * @deprecated since Frameworks 5.0
         */
        InServerMBXType,
        /**
         * @deprecated since Frameworks 5.0
         */
        InServerTLS
#endif
    };

    /**
      * The various extensions allowed.
      **/
    enum Extension {
        POP3,
        SMTP,
        OTHER
    };

    /**
      * Default constructor, just sets things up and sets the default profile
      * as the current profile
      **/
    KEMailSettings();

    /**
      * Default destructor, nothing to see here.
      **/
    ~KEMailSettings();

    /**
      * List of profiles available.
      * @return the list of profiles
      **/
    QStringList profiles() const;

#ifndef KDE_NO_DEPRECATED
    /**
      * @deprecated since Frameworks 5.0
      * Returns the name of the current profile.
      * @returns what profile we're currently using
      **/
    KCONFIGCORE_DEPRECATED QString currentProfileName() const;
#endif

    /**
      * Change the current profile.
      * @param s the name of the new profile
      **/
    void setProfile(const QString &s);

    /**
     * Returns the name of the default profile.
     * @returns the name of the one that's currently default QString() if none
     **/
    QString defaultProfileName() const;

    /**
      * Sets a new default.
      * @param def the new default
      **/
    void setDefault(const QString &def);

    /**
      * Get one of the predefined "basic" settings.
      * @param s the setting to get
      * @return the value of the setting, or QString() if not
      *         set
      **/
    QString getSetting(KEMailSettings::Setting s) const;

    /**
      * Set one of the predefined "basic" settings.
      * @param s the setting to set
      * @param v the new value of the setting, or QString() to
      *         unset
      **/
    void setSetting(KEMailSettings::Setting s, const QString &v);

private:
    KEMailSettingsPrivate *const p;
};

#endif
