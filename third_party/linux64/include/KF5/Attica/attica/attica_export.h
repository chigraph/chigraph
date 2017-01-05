
#ifndef ATTICA_EXPORT_H
#define ATTICA_EXPORT_H

#ifdef ATTICA_STATIC_DEFINE
#  define ATTICA_EXPORT
#  define ATTICA_NO_EXPORT
#else
#  ifndef ATTICA_EXPORT
#    ifdef KF5Attica_EXPORTS
        /* We are building this library */
#      define ATTICA_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define ATTICA_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef ATTICA_NO_EXPORT
#    define ATTICA_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef ATTICA_DEPRECATED
#  define ATTICA_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef ATTICA_DEPRECATED_EXPORT
#  define ATTICA_DEPRECATED_EXPORT ATTICA_EXPORT ATTICA_DEPRECATED
#endif

#ifndef ATTICA_DEPRECATED_NO_EXPORT
#  define ATTICA_DEPRECATED_NO_EXPORT ATTICA_NO_EXPORT ATTICA_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef ATTICA_NO_DEPRECATED
#    define ATTICA_NO_DEPRECATED
#  endif
#endif

#endif
