
#ifndef KCODECS_EXPORT_H
#define KCODECS_EXPORT_H

#ifdef KCODECS_STATIC_DEFINE
#  define KCODECS_EXPORT
#  define KCODECS_NO_EXPORT
#else
#  ifndef KCODECS_EXPORT
#    ifdef KF5Codecs_EXPORTS
        /* We are building this library */
#      define KCODECS_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define KCODECS_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef KCODECS_NO_EXPORT
#    define KCODECS_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef KCODECS_DEPRECATED
#  define KCODECS_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef KCODECS_DEPRECATED_EXPORT
#  define KCODECS_DEPRECATED_EXPORT KCODECS_EXPORT KCODECS_DEPRECATED
#endif

#ifndef KCODECS_DEPRECATED_NO_EXPORT
#  define KCODECS_DEPRECATED_NO_EXPORT KCODECS_NO_EXPORT KCODECS_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef KCODECS_NO_DEPRECATED
#    define KCODECS_NO_DEPRECATED
#  endif
#endif

#endif
