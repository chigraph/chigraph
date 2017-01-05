
#ifndef KCONFIGGUI_EXPORT_H
#define KCONFIGGUI_EXPORT_H

#ifdef KCONFIGGUI_STATIC_DEFINE
#  define KCONFIGGUI_EXPORT
#  define KCONFIGGUI_NO_EXPORT
#else
#  ifndef KCONFIGGUI_EXPORT
#    ifdef KF5ConfigGui_EXPORTS
        /* We are building this library */
#      define KCONFIGGUI_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define KCONFIGGUI_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef KCONFIGGUI_NO_EXPORT
#    define KCONFIGGUI_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef KCONFIGGUI_DEPRECATED
#  define KCONFIGGUI_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef KCONFIGGUI_DEPRECATED_EXPORT
#  define KCONFIGGUI_DEPRECATED_EXPORT KCONFIGGUI_EXPORT KCONFIGGUI_DEPRECATED
#endif

#ifndef KCONFIGGUI_DEPRECATED_NO_EXPORT
#  define KCONFIGGUI_DEPRECATED_NO_EXPORT KCONFIGGUI_NO_EXPORT KCONFIGGUI_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef KCONFIGGUI_NO_DEPRECATED
#    define KCONFIGGUI_NO_DEPRECATED
#  endif
#endif

#endif
