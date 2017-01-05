
#ifndef KWIDGETSADDONS_EXPORT_H
#define KWIDGETSADDONS_EXPORT_H

#ifdef KWIDGETSADDONS_STATIC_DEFINE
#  define KWIDGETSADDONS_EXPORT
#  define KWIDGETSADDONS_NO_EXPORT
#else
#  ifndef KWIDGETSADDONS_EXPORT
#    ifdef KF5WidgetsAddons_EXPORTS
        /* We are building this library */
#      define KWIDGETSADDONS_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define KWIDGETSADDONS_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef KWIDGETSADDONS_NO_EXPORT
#    define KWIDGETSADDONS_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef KWIDGETSADDONS_DEPRECATED
#  define KWIDGETSADDONS_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef KWIDGETSADDONS_DEPRECATED_EXPORT
#  define KWIDGETSADDONS_DEPRECATED_EXPORT KWIDGETSADDONS_EXPORT KWIDGETSADDONS_DEPRECATED
#endif

#ifndef KWIDGETSADDONS_DEPRECATED_NO_EXPORT
#  define KWIDGETSADDONS_DEPRECATED_NO_EXPORT KWIDGETSADDONS_NO_EXPORT KWIDGETSADDONS_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef KWIDGETSADDONS_NO_DEPRECATED
#    define KWIDGETSADDONS_NO_DEPRECATED
#  endif
#endif

#endif
