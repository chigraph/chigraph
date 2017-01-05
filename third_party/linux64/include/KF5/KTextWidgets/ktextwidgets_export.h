
#ifndef KTEXTWIDGETS_EXPORT_H
#define KTEXTWIDGETS_EXPORT_H

#ifdef KTEXTWIDGETS_STATIC_DEFINE
#  define KTEXTWIDGETS_EXPORT
#  define KTEXTWIDGETS_NO_EXPORT
#else
#  ifndef KTEXTWIDGETS_EXPORT
#    ifdef KF5TextWidgets_EXPORTS
        /* We are building this library */
#      define KTEXTWIDGETS_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define KTEXTWIDGETS_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef KTEXTWIDGETS_NO_EXPORT
#    define KTEXTWIDGETS_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef KTEXTWIDGETS_DEPRECATED
#  define KTEXTWIDGETS_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef KTEXTWIDGETS_DEPRECATED_EXPORT
#  define KTEXTWIDGETS_DEPRECATED_EXPORT KTEXTWIDGETS_EXPORT KTEXTWIDGETS_DEPRECATED
#endif

#ifndef KTEXTWIDGETS_DEPRECATED_NO_EXPORT
#  define KTEXTWIDGETS_DEPRECATED_NO_EXPORT KTEXTWIDGETS_NO_EXPORT KTEXTWIDGETS_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef KTEXTWIDGETS_NO_DEPRECATED
#    define KTEXTWIDGETS_NO_DEPRECATED
#  endif
#endif

#endif
