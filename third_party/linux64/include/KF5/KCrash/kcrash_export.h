
#ifndef KCRASH_EXPORT_H
#define KCRASH_EXPORT_H

#ifdef KCRASH_STATIC_DEFINE
#  define KCRASH_EXPORT
#  define KCRASH_NO_EXPORT
#else
#  ifndef KCRASH_EXPORT
#    ifdef KF5Crash_EXPORTS
        /* We are building this library */
#      define KCRASH_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define KCRASH_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef KCRASH_NO_EXPORT
#    define KCRASH_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef KCRASH_DEPRECATED
#  define KCRASH_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef KCRASH_DEPRECATED_EXPORT
#  define KCRASH_DEPRECATED_EXPORT KCRASH_EXPORT KCRASH_DEPRECATED
#endif

#ifndef KCRASH_DEPRECATED_NO_EXPORT
#  define KCRASH_DEPRECATED_NO_EXPORT KCRASH_NO_EXPORT KCRASH_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef KCRASH_NO_DEPRECATED
#    define KCRASH_NO_DEPRECATED
#  endif
#endif

#endif
