
#ifndef KSERVICE_EXPORT_H
#define KSERVICE_EXPORT_H

#ifdef KSERVICE_STATIC_DEFINE
#  define KSERVICE_EXPORT
#  define KSERVICE_NO_EXPORT
#else
#  ifndef KSERVICE_EXPORT
#    ifdef KF5Service_EXPORTS
        /* We are building this library */
#      define KSERVICE_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define KSERVICE_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef KSERVICE_NO_EXPORT
#    define KSERVICE_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef KSERVICE_DEPRECATED
#  define KSERVICE_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef KSERVICE_DEPRECATED_EXPORT
#  define KSERVICE_DEPRECATED_EXPORT KSERVICE_EXPORT KSERVICE_DEPRECATED
#endif

#ifndef KSERVICE_DEPRECATED_NO_EXPORT
#  define KSERVICE_DEPRECATED_NO_EXPORT KSERVICE_NO_EXPORT KSERVICE_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef KSERVICE_NO_DEPRECATED
#    define KSERVICE_NO_DEPRECATED
#  endif
#endif

#endif
