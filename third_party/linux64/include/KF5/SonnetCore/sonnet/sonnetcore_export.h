
#ifndef SONNETCORE_EXPORT_H
#define SONNETCORE_EXPORT_H

#ifdef SONNETCORE_STATIC_DEFINE
#  define SONNETCORE_EXPORT
#  define SONNETCORE_NO_EXPORT
#else
#  ifndef SONNETCORE_EXPORT
#    ifdef KF5SonnetCore_EXPORTS
        /* We are building this library */
#      define SONNETCORE_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define SONNETCORE_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef SONNETCORE_NO_EXPORT
#    define SONNETCORE_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef SONNETCORE_DEPRECATED
#  define SONNETCORE_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef SONNETCORE_DEPRECATED_EXPORT
#  define SONNETCORE_DEPRECATED_EXPORT SONNETCORE_EXPORT SONNETCORE_DEPRECATED
#endif

#ifndef SONNETCORE_DEPRECATED_NO_EXPORT
#  define SONNETCORE_DEPRECATED_NO_EXPORT SONNETCORE_NO_EXPORT SONNETCORE_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef SONNETCORE_NO_DEPRECATED
#    define SONNETCORE_NO_DEPRECATED
#  endif
#endif

#endif
