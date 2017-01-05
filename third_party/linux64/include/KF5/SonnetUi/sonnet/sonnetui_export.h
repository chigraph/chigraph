
#ifndef SONNETUI_EXPORT_H
#define SONNETUI_EXPORT_H

#ifdef SONNETUI_STATIC_DEFINE
#  define SONNETUI_EXPORT
#  define SONNETUI_NO_EXPORT
#else
#  ifndef SONNETUI_EXPORT
#    ifdef KF5SonnetUi_EXPORTS
        /* We are building this library */
#      define SONNETUI_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define SONNETUI_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef SONNETUI_NO_EXPORT
#    define SONNETUI_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef SONNETUI_DEPRECATED
#  define SONNETUI_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef SONNETUI_DEPRECATED_EXPORT
#  define SONNETUI_DEPRECATED_EXPORT SONNETUI_EXPORT SONNETUI_DEPRECATED
#endif

#ifndef SONNETUI_DEPRECATED_NO_EXPORT
#  define SONNETUI_DEPRECATED_NO_EXPORT SONNETUI_NO_EXPORT SONNETUI_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef SONNETUI_NO_DEPRECATED
#    define SONNETUI_NO_DEPRECATED
#  endif
#endif

#endif
