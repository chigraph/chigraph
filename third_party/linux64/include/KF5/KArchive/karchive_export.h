
#ifndef KARCHIVE_EXPORT_H
#define KARCHIVE_EXPORT_H

#ifdef KARCHIVE_STATIC_DEFINE
#  define KARCHIVE_EXPORT
#  define KARCHIVE_NO_EXPORT
#else
#  ifndef KARCHIVE_EXPORT
#    ifdef KF5Archive_EXPORTS
        /* We are building this library */
#      define KARCHIVE_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define KARCHIVE_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef KARCHIVE_NO_EXPORT
#    define KARCHIVE_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef KARCHIVE_DEPRECATED
#  define KARCHIVE_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef KARCHIVE_DEPRECATED_EXPORT
#  define KARCHIVE_DEPRECATED_EXPORT KARCHIVE_EXPORT KARCHIVE_DEPRECATED
#endif

#ifndef KARCHIVE_DEPRECATED_NO_EXPORT
#  define KARCHIVE_DEPRECATED_NO_EXPORT KARCHIVE_NO_EXPORT KARCHIVE_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef KARCHIVE_NO_DEPRECATED
#    define KARCHIVE_NO_DEPRECATED
#  endif
#endif

#endif
