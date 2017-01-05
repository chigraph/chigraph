
#ifndef KDBUSADDONS_EXPORT_H
#define KDBUSADDONS_EXPORT_H

#ifdef KDBUSADDONS_STATIC_DEFINE
#  define KDBUSADDONS_EXPORT
#  define KDBUSADDONS_NO_EXPORT
#else
#  ifndef KDBUSADDONS_EXPORT
#    ifdef KF5DBusAddons_EXPORTS
        /* We are building this library */
#      define KDBUSADDONS_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define KDBUSADDONS_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef KDBUSADDONS_NO_EXPORT
#    define KDBUSADDONS_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef KDBUSADDONS_DEPRECATED
#  define KDBUSADDONS_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef KDBUSADDONS_DEPRECATED_EXPORT
#  define KDBUSADDONS_DEPRECATED_EXPORT KDBUSADDONS_EXPORT KDBUSADDONS_DEPRECATED
#endif

#ifndef KDBUSADDONS_DEPRECATED_NO_EXPORT
#  define KDBUSADDONS_DEPRECATED_NO_EXPORT KDBUSADDONS_NO_EXPORT KDBUSADDONS_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef KDBUSADDONS_NO_DEPRECATED
#    define KDBUSADDONS_NO_DEPRECATED
#  endif
#endif

#endif
