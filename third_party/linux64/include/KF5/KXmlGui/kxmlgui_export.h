
#ifndef KXMLGUI_EXPORT_H
#define KXMLGUI_EXPORT_H

#ifdef KXMLGUI_STATIC_DEFINE
#  define KXMLGUI_EXPORT
#  define KXMLGUI_NO_EXPORT
#else
#  ifndef KXMLGUI_EXPORT
#    ifdef KF5XmlGui_EXPORTS
        /* We are building this library */
#      define KXMLGUI_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define KXMLGUI_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef KXMLGUI_NO_EXPORT
#    define KXMLGUI_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef KXMLGUI_DEPRECATED
#  define KXMLGUI_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef KXMLGUI_DEPRECATED_EXPORT
#  define KXMLGUI_DEPRECATED_EXPORT KXMLGUI_EXPORT KXMLGUI_DEPRECATED
#endif

#ifndef KXMLGUI_DEPRECATED_NO_EXPORT
#  define KXMLGUI_DEPRECATED_NO_EXPORT KXMLGUI_NO_EXPORT KXMLGUI_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef KXMLGUI_NO_DEPRECATED
#    define KXMLGUI_NO_DEPRECATED
#  endif
#endif

#endif
