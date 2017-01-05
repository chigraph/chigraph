/* vi: ts=8 sts=4 sw=4
 *
 * This file is part of the KDE project, module kdecore.
 * Copyright (C) 2000 Geert Jansen <jansen@kde.org>
 *                    Antonio Larrosa <larrosa@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KICONLOADER_H
#define KICONLOADER_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QObject>

#include <kiconthemes_export.h>

class QIcon;
class QMovie;
class QPixmap;

class KIconLoaderPrivate;
class KIconEffect;
class KIconTheme;

class KPixmapSequence;

/**
 * Iconloader for KDE.
 *
 * KIconLoader will load the current icon theme and all its base themes.
 * Icons will be searched in any of these themes. Additionally, it caches
 * icons and applies effects according to the user's preferences.
 *
 * In KDE, it is encouraged to load icons by "Group". An icon group is a
 * location on the screen where icons are being used. Standard groups are:
 * Desktop, Toolbar, MainToolbar, Small and Panel. Each group has some
 * centrally configured properties bound to it, including the icon size
 * and effects. This makes it possible to offer a consistent icon look in
 * all KDE applications.
 *
 * The standard groups are defined below.
 *
 * @li KIconLoader::Desktop: Icons in the iconview of konqueror, kdesktop and similar apps.
 * @li KIconLoader::Toolbar: Icons in toolbars.
 * @li KIconLoader::MainToolbar: Icons in the main toolbars.
 * @li KIconLoader::Small: Various small (typical 16x16) places: titlebars, listviews
 * and menu entries.
 * @li KIconLoader::Panel: Icons in kicker's panel
 *
 * The icons are stored on disk in an icon theme or in a standalone
 * directory. The icon theme directories contain multiple sizes and/or
 * depths for the same icon. The iconloader will load the correct one based
 * on the icon group and the current theme. Icon themes are stored globally
 * in share/icons, or, application specific in share/apps/$appdir/icons.
 *
 * The standalone directories contain just one version of an icon. The
 * directories that are searched are: $appdir/pics and $appdir/toolbar.
 * Icons in these directories can be loaded by using the special group
 * "User".
 *
 */
class KICONTHEMES_EXPORT KIconLoader : public QObject
{
    Q_OBJECT
    Q_ENUMS(Context)
    Q_ENUMS(Type)
    Q_ENUMS(MatchType)
    Q_ENUMS(Group)
    Q_ENUMS(StdSizes)
    Q_ENUMS(States)

public:
    /**
    * Defines the context of the icon.
    */
    enum Context {
        Any,           ///< Some icon with unknown purpose.
        Action,        ///< An action icon (e.g. 'save', 'print').
        Application,   ///< An icon that represents an application.
        Device,        ///< An icon that represents a device.
        FileSystem,    ///< An icon that represents a file system. @deprecated Use Place instead.
        MimeType,      ///< An icon that represents a mime type (or file type).
        Animation,     ///< An icon that is animated.
        Category,      ///< An icon that represents a category.
        Emblem,        ///< An icon that adds information to an existing icon.
        Emote,         ///< An icon that expresses an emotion.
        International, ///< An icon that represents a country's flag.
        Place,         ///< An icon that represents a location (e.g. 'home', 'trash').
        StatusIcon     ///< An icon that represents an event.
    };

    /**
     * The type of the icon.
     */
    enum Type {
        Fixed,    ///< Fixed-size icon.
        Scalable, ///< Scalable-size icon.
        Threshold ///< A threshold icon.
    };

    /**
     * The type of a match.
     */
    enum MatchType {
        MatchExact, ///< Only try to find an exact match.
        MatchBest   ///< Take the best match if there is no exact match.
    };

    /**
     * The group of the icon.
     */
    enum Group {
        /// No group
        NoGroup = -1,
        /// Desktop icons
        Desktop = 0,
        /// First group
        FirstGroup = 0,
        /// Toolbar icons
        Toolbar,
        /// Main toolbar icons
        MainToolbar,
        /// Small icons, e.g. for buttons
        Small,
        /// Panel (Plasma Taskbar) icons
        Panel,
        /// Icons for use in dialog titles, page lists, etc
        Dialog,
        /// Last group
        LastGroup,
        /// User icons
        User
    };

    /**
     * These are the standard sizes for icons.
     */
    enum StdSizes {
        /// small icons for menu entries
        SizeSmall = 16,
        /// slightly larger small icons for toolbars, panels, etc
        SizeSmallMedium = 22,
        /// medium sized icons for the desktop
        SizeMedium = 32,
        /// large sized icons for the panel
        SizeLarge = 48,
        /// huge sized icons for iconviews
        SizeHuge = 64,
        /// enormous sized icons for iconviews
        SizeEnormous = 128
    };

    /**
     * Defines the possible states of an icon.
     */
    enum States {
        DefaultState,  ///< The default state.
        ActiveState,   ///< Icon is active.
        DisabledState, ///< Icon is disabled.
        SelectedState, ///< Icon is selected. @since 5.22
        LastState      ///< Last state (last constant)
    };

    /**
     * Constructs an iconloader.
     * @param appname Add the data directories of this application to the
     * icon search path for the "User" group. The default argument adds the
     * directories of the current application.
     * @param extraSearchPaths additional search paths, either absolute or relative to GenericDataLocation
     *
     * Usually, you use the default iconloader, which can be accessed via
     * KIconLoader::global(), so you hardly ever have to create an
     * iconloader object yourself. That one is the application's iconloader.
     */
    explicit KIconLoader(const QString &appname = QString(), const QStringList &extraSearchPaths = QStringList(), QObject *parent = 0);

    /**
     * Cleanup
     */
    ~KIconLoader();

    /**
     * Returns the global icon loader initialized with the application name.
     * @return global icon loader
     */
    static KIconLoader *global();

    /**
     * Adds @p appname to the list of application specific directories with @p themeBaseDir as its base directory.
     * Assume the icons are in /home/user/app/icons/hicolor/48x48/my_app.png, the base directory would be
     * /home/user/app/icons; KIconLoader automatically searches @p themeBaseDir + "/hicolor"
     * This directory must contain a dir structure as defined by the XDG icons specification
     * @param appname The application name.
     * @param themeBaseDir The base directory of the application's theme (eg. "/home/user/app/icons")
     */
    void addAppDir(const QString &appname, const QString &themeBaseDir = QString());

    /**
     * Loads an icon. It will try very hard to find an icon which is
     * suitable. If no exact match is found, a close match is searched.
     * If neither an exact nor a close match is found, a null pixmap or
     * the "unknown" pixmap is returned, depending on the value of the
     * @p canReturnNull parameter.
     *
     * @param name The name of the icon, without extension.
     * @param group The icon group. This will specify the size of and effects to
     * be applied to the icon.
     * @param size If nonzero, this overrides the size specified by @p group.
     *             See KIconLoader::StdSizes.
     * @param state The icon state: @p DefaultState, @p ActiveState or
     * @p DisabledState. Depending on the user's preferences, the iconloader
     * may apply a visual effect to hint about its state.
     * @param overlays a list of emblem icons to overlay, by name
     *                 @see drawOverlays
     * @param path_store If not null, the path of the icon is stored here,
     *        if the icon was found. If the icon was not found @p path_store
     *        is unaltered even if the "unknown" pixmap was returned.
     * @param canReturnNull Can return a null pixmap? If false, the
     *        "unknown" pixmap is returned when no appropriate icon has been
     *        found. <em>Note:</em> a null pixmap can still be returned in the
     *        event of invalid parameters, such as empty names, negative sizes,
     *        and etc.
     * @return the QPixmap. Can be null when not found, depending on
     *         @p canReturnNull.
     */
    QPixmap loadIcon(const QString &name, KIconLoader::Group group, int size = 0,
                     int state = KIconLoader::DefaultState, const QStringList &overlays = QStringList(),
                     QString *path_store = 0L,
                     bool canReturnNull = false) const;

    /**
     * Loads an icon for a mimetype.
     * This is basically like loadIcon except that extra desktop themes are loaded if necessary.
     *
     * @param iconName The name of the icon, without extension, usually from KMimeType.
     * @param group The icon group. This will specify the size of and effects to
     * be applied to the icon.
     * @param size If nonzero, this overrides the size specified by @p group.
     *             See KIconLoader::StdSizes.
     * @param state The icon state: @p DefaultState, @p ActiveState or
     * @p DisabledState. Depending on the user's preferences, the iconloader
     * may apply a visual effect to hint about its state.
     * @param path_store If not null, the path of the icon is stored here.
     * @param overlays a list of emblem icons to overlay, by name
     *                 @see drawOverlays
     * @return the QPixmap. Can not be null, the
     * "unknown" pixmap is returned when no appropriate icon has been found.
     */
    QPixmap loadMimeTypeIcon(const QString &iconName, KIconLoader::Group group, int size = 0,
                             int state = KIconLoader::DefaultState, const QStringList &overlays = QStringList(),
                             QString *path_store = 0) const;

    /**
     * Loads a pixmapSequence given the xdg icon name
     *
     * @param name The name of the icon, without extension.
     * @param size the size/group to be used
     * @since 5.0
     */
    KPixmapSequence loadPixmapSequence(const QString &iconName, int size = SizeSmall) const;

    /**
     * Creates an icon set, that will do on-demand loading of the icon.
     * Loading itself is done by calling loadIcon .
     *
     * @param name The name of the icon, without extension.
     * @param group The icon group. This will specify the size of and effects to
     * be applied to the icon.
     * @param size If nonzero, this overrides the size specified by @p group.
     *             See KIconLoader::StdSizes.
     * @param canReturnNull Can return a null iconset? If false, iconset
     * containing the "unknown" pixmap is returned when no appropriate icon has
     * been found.
     * @return the icon set. Can be null when not found, depending on
     *          @p canReturnNull.
     *
     * @deprecated use QIcon::fromTheme instead, which uses the iconloader internally
     */
#ifndef KICONTHEMES_NO_DEPRECATED
    KICONTHEMES_DEPRECATED QIcon loadIconSet(const QString &name, KIconLoader::Group group, int size = 0,
            bool canReturnNull = false);
#endif

    /**
     * Returns the path of an icon.
     * @param name The name of the icon, without extension. If an absolute
     * path is supplied for this parameter, iconPath will return it
     * directly.
     * @param group_or_size If positive, search icons whose size is
     * specified by the icon group @p group_or_size. If negative, search
     * icons whose size is - @p group_or_size.
     *             See KIconLoader::Group and KIconLoader::StdSizes
     * @param canReturnNull Can return a null string? If not, a path to the
     *                      "unknown" icon will be returned.
     * @return the path of an icon, can be null or the "unknown" icon when
     *         not found, depending on @p canReturnNull.
     */
    QString iconPath(const QString &name, int group_or_size,
                     bool canReturnNull = false) const;

    /**
     * Loads an animated icon.
     * @param name The name of the icon.
     * @param group The icon group. See loadIcon().
     * @param size Override the default size for @p group.
     *             See KIconLoader::StdSizes.
     * @param parent The parent object of the returned QMovie.
     * @return A QMovie object. Can be null if not found or not valid.
     *         Ownership is passed to the caller.
     */
    QMovie *loadMovie(const QString &name, KIconLoader::Group group, int size = 0, QObject *parent = 0) const;

    /**
     * Returns the path to an animated icon.
     * @param name The name of the icon.
     * @param group The icon group. See loadIcon().
     * @param size Override the default size for @p group.
     *             See KIconLoader::StdSizes.
     * @return the full path to the movie, ready to be passed to QMovie's constructor.
     * Empty string if not found.
     */
    QString moviePath(const QString &name, KIconLoader::Group group, int size = 0) const;

    /**
     * Loads an animated icon as a series of still frames. If you want to load
     * a .mng animation as QMovie instead, please use loadMovie() instead.
     * @param name The name of the icon.
     * @param group The icon group. See loadIcon().
     * @param size Override the default size for @p group.
     *             See KIconLoader::StdSizes.
     * @return A QStringList containing the absolute path of all the frames
     * making up the animation.
     */
    QStringList loadAnimated(const QString &name, KIconLoader::Group group, int size = 0) const;

    /**
     * Queries all available icons for a specific group, having a specific
     * context.
     * @param group_or_size If positive, search icons whose size is
     * specified by the icon group @p group_or_size. If negative, search
     * icons whose size is - @p group_or_size.
     *             See KIconLoader::Group and KIconLoader::StdSizes
     * @param context The icon context.
     * @return a list of all icons
     */
    QStringList queryIcons(int group_or_size, KIconLoader::Context context = KIconLoader::Any) const;

    /**
     * Queries all available icons for a specific context.
     * @param group_or_size The icon preferred group or size. If available
     * at this group or size, those icons will be returned, in other case,
     * icons of undefined size will be returned. Positive numbers are groups,
     * negative numbers are negated sizes. See KIconLoader::Group and
     * KIconLoader::StdSizes
     * @param context The icon context.
     * @return A QStringList containing the icon names
     * available for that context
     */
    QStringList queryIconsByContext(int group_or_size,
                                    KIconLoader::Context context = KIconLoader::Any) const;

    /**
     * @internal
     */
    bool hasContext(KIconLoader::Context context) const;

    /**
     * Returns a list of all icons (*.png or *.xpm extension) in the
     * given directory.
     * @param iconsDir the directory to search in
     * @return A QStringList containing the icon paths
     */
    QStringList queryIconsByDir(const QString &iconsDir) const;

    /**
     * Returns all the search paths for this icon loader, either absolute or
     * relative to GenericDataLocation.
     * Mostly internal (for KIconDialog).
     * \since 5.0
     */
    QStringList searchPaths() const;

    /**
     * Returns the current size of the icon group.
     * Using e.g. KIconLoader::SmallIcon will retrieve the icon size
     * that is currently set from System Settings->Appearance->Icon
     * sizes. SmallIcon for instance, would typically be 16x16, but
     * the user could increase it and this setting would change as well.
     * @param group the group to check.
     * @return the current size for an icon group.
     */
    int currentSize(KIconLoader::Group group) const;

    /**
     * Returns a pointer to the current theme. Can be used to query
     * available and default sizes for groups.
     * @note The KIconTheme will change if reconfigure() is called and
     * therefore it's not recommended to store the pointer anywhere.
     * @return a pointer to the current theme. 0 if no theme set.
     */
    KIconTheme *theme() const;

    /**
     * Returns a pointer to the KIconEffect object used by the icon loader.
     * @return the KIconEffect.
     */
    KIconEffect *iconEffect() const;

    /**
     * Reconfigure the icon loader, for instance to change the associated app name or extra search paths.
     * This also clears the in-memory pixmap cache (even if the appname didn't change, which is useful for unittests)
     * @param appname the application name (empty for the global iconloader)
     * @param extraSearchPaths additional search paths, either absolute or relative to GenericDataLocation
     */
    void reconfigure(const QString &appname, const QStringList &extraSearchPaths = QStringList());

    /**
     * Returns the unknown icon. An icon that is used when no other icon
     * can be found.
     * @return the unknown pixmap
     */
    static QPixmap unknown();

    /**
     * Checks whether the user wants to blend the icons with the background
     *  using the alpha channel information for a given group.
     * @param group the group to check
     * @return true if alpha blending is desired
     * @obsolete
     */
    bool alphaBlending(KIconLoader::Group group) const;

    /**
     * Draws overlays on the specified pixmap, it takes the width and height
     * of the pixmap into consideration
     * @param overlays List of up to 4 overlays to blend over the pixmap. The first overlay
     *                 will be in the bottom left corner, followed by bottom right, top right
     *                 and top left. An empty QString can be used to leave the specific position
     *                 blank.
     * @param pixmap to draw on
     * @since 4.7
     */
    void drawOverlays(const QStringList &overlays, QPixmap &pixmap, KIconLoader::Group group, int state = KIconLoader::DefaultState) const;

    bool hasIcon(const QString &iconName) const;
public Q_SLOTS:
    /**
     * Re-initialize the global icon loader
     *
     * @deprecated since 5.0, use emitChange(Group)
     */
#ifndef KICONTHEMES_NO_DEPRECATED
    KICONTHEMES_DEPRECATED void newIconLoader();
#endif

    /**
     * Emits an iconChanged() signal on all the KIconLoader instances in the system
     * indicating that a system's icon group has changed in some way. It will also trigger
     * a reload in all of them to update to the new theme.
     *
     * @p group indicates the group that has changed
     *
     * @since 5.0
     */
    static void emitChange(Group group);

Q_SIGNALS:
    /**
     * Emitted by newIconLoader once the new settings have been loaded
     */
    void iconLoaderSettingsChanged();

    /**
     * Emitted when the system icon theme changes
     *
     * @since 5.0
     */
    void iconChanged(int group);

private:
    // @internal the data object
    KIconLoaderPrivate *d;

    Q_PRIVATE_SLOT(d, void _k_refreshIcons(int group))
};

/**
 * \relates KIconLoader
 * Load a desktop icon.
 */
KICONTHEMES_EXPORT QPixmap DesktopIcon(const QString &name, int size = 0,
                                       int state = KIconLoader::DefaultState, const QStringList &overlays = QStringList());

/**
 * \relates KIconLoader
 * Load a desktop icon, and apply the necessary effects to get an IconSet.
 * @deprecated use QIcon::fromTheme instead
 */
#ifndef KICONTHEMES_NO_DEPRECATED
KICONTHEMES_DEPRECATED_EXPORT QIcon DesktopIconSet(const QString &name, int size = 0);
#endif

/**
 * \relates KIconLoader
 * Load a toolbar icon.
 */
KICONTHEMES_EXPORT QPixmap BarIcon(const QString &name, int size = 0, int state = KIconLoader::DefaultState,
                                   const QStringList &overlays = QStringList());

/**
 * \relates KIconLoader
 * Load a toolbar icon, and apply the necessary effects to get an IconSet.
 * @deprecated use QIcon::fromTheme instead
 */
#ifndef KICONTHEMES_NO_DEPRECATED
KICONTHEMES_DEPRECATED_EXPORT QIcon BarIconSet(const QString &name, int size = 0);
#endif

/**
 * \relates KIconLoader
 * Load a small icon.
 */
KICONTHEMES_EXPORT QPixmap SmallIcon(const QString &name, int size = 0,
                                     int state = KIconLoader::DefaultState, const QStringList &overlays = QStringList());

/**
 * \relates KIconLoader
 * Load a small icon, and apply the necessary effects to get an IconSet.
 * @deprecated use QIcon::fromTheme instead
 */
#ifndef KICONTHEMES_NO_DEPRECATED
KICONTHEMES_DEPRECATED_EXPORT QIcon SmallIconSet(const QString &name, int size = 0);
#endif

/**
 * \relates KIconLoader
 * Load a main toolbar icon.
 */
KICONTHEMES_EXPORT QPixmap MainBarIcon(const QString &name, int size = 0,
                                       int state = KIconLoader::DefaultState, const QStringList &overlays = QStringList());

/**
 * \relates KIconLoader
 * Load a main toolbar icon, and apply the effects to get an IconSet.
 * @deprecated use QIcon::fromTheme instead
 */
#ifndef KICONTHEMES_NO_DEPRECATED
KICONTHEMES_DEPRECATED_EXPORT QIcon MainBarIconSet(const QString &name, int size = 0);
#endif

/**
 * \relates KIconLoader
 * Load a user icon. User icons are searched in $appdir/pics.
 */
KICONTHEMES_EXPORT QPixmap UserIcon(const QString &name, int state = KIconLoader::DefaultState, const QStringList &overlays = QStringList());

/**
 * \relates KIconLoader
 * Load a user icon, and apply the effects to get an IconSet.
 * @deprecated use QIcon::fromTheme instead
 */
#ifndef KICONTHEMES_NO_DEPRECATED
KICONTHEMES_DEPRECATED_EXPORT QIcon UserIconSet(const QString &name);
#endif

/**
 * \relates KIconLoader
 * Returns the current icon size for a specific group.
 */
KICONTHEMES_EXPORT int IconSize(KIconLoader::Group group);

namespace KDE
{

/**
 * \relates KIconLoader
 * Returns a QIcon with an appropriate
 * KIconEngine to perform loading and rendering.  KIcons thus adhere to
 * KDE style and effect standards.
 * @since 5.0
 */
KICONTHEMES_EXPORT QIcon icon(const QString &iconName, KIconLoader *iconLoader = 0);

/**
 * \relates KIconLoader
 * Returns a QIcon for the given icon, with additional overlays.
 * @since 5.0
 */
KICONTHEMES_EXPORT QIcon icon(const QString &iconName, const QStringList &overlays, KIconLoader *iconLoader = 0);

}

inline KIconLoader::Group &operator++(KIconLoader::Group &group)
{
    group = static_cast<KIconLoader::Group>(group + 1);
    return group;
}
inline KIconLoader::Group operator++(KIconLoader::Group &group, int)
{
    KIconLoader::Group ret = group;
    ++group;
    return ret;
}

#endif // KICONLOADER_H
