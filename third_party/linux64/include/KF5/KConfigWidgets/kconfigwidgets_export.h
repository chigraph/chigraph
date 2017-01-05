
#ifndef KCONFIGWIDGETS_EXPORT_H
#define KCONFIGWIDGETS_EXPORT_H

#ifdef KCONFIGWIDGETS_STATIC_DEFINE
#  define KCONFIGWIDGETS_EXPORT
#  define KCONFIGWIDGETS_NO_EXPORT
#else
#  ifndef KCONFIGWIDGETS_EXPORT
#    ifdef KF5ConfigWidgets_EXPORTS
        /* We are building this library */
#      define KCONFIGWIDGETS_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define KCONFIGWIDGETS_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef KCONFIGWIDGETS_NO_EXPORT
#    define KCONFIGWIDGETS_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef KCONFIGWIDGETS_DEPRECATED
#  define KCONFIGWIDGETS_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef KCONFIGWIDGETS_DEPRECATED_EXPORT
#  define KCONFIGWIDGETS_DEPRECATED_EXPORT KCONFIGWIDGETS_EXPORT KCONFIGWIDGETS_DEPRECATED
#endif

#ifndef KCONFIGWIDGETS_DEPRECATED_NO_EXPORT
#  define KCONFIGWIDGETS_DEPRECATED_NO_EXPORT KCONFIGWIDGETS_NO_EXPORT KCONFIGWIDGETS_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef KCONFIGWIDGETS_NO_DEPRECATED
#    define KCONFIGWIDGETS_NO_DEPRECATED
#  endif
#endif

#endif
