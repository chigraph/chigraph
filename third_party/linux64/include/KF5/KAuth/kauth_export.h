
#ifndef KAUTH_EXPORT_H
#define KAUTH_EXPORT_H

#ifdef KAUTH_STATIC_DEFINE
#  define KAUTH_EXPORT
#  define KAUTH_NO_EXPORT
#else
#  ifndef KAUTH_EXPORT
#    ifdef KF5Auth_EXPORTS
        /* We are building this library */
#      define KAUTH_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define KAUTH_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef KAUTH_NO_EXPORT
#    define KAUTH_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef KAUTH_DEPRECATED
#  define KAUTH_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef KAUTH_DEPRECATED_EXPORT
#  define KAUTH_DEPRECATED_EXPORT KAUTH_EXPORT KAUTH_DEPRECATED
#endif

#ifndef KAUTH_DEPRECATED_NO_EXPORT
#  define KAUTH_DEPRECATED_NO_EXPORT KAUTH_NO_EXPORT KAUTH_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef KAUTH_NO_DEPRECATED
#    define KAUTH_NO_DEPRECATED
#  endif
#endif

#endif
