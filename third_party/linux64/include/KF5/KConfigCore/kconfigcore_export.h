
#ifndef KCONFIGCORE_EXPORT_H
#define KCONFIGCORE_EXPORT_H

#ifdef KCONFIGCORE_STATIC_DEFINE
#  define KCONFIGCORE_EXPORT
#  define KCONFIGCORE_NO_EXPORT
#else
#  ifndef KCONFIGCORE_EXPORT
#    ifdef KF5ConfigCore_EXPORTS
        /* We are building this library */
#      define KCONFIGCORE_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define KCONFIGCORE_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef KCONFIGCORE_NO_EXPORT
#    define KCONFIGCORE_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef KCONFIGCORE_DEPRECATED
#  define KCONFIGCORE_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef KCONFIGCORE_DEPRECATED_EXPORT
#  define KCONFIGCORE_DEPRECATED_EXPORT KCONFIGCORE_EXPORT KCONFIGCORE_DEPRECATED
#endif

#ifndef KCONFIGCORE_DEPRECATED_NO_EXPORT
#  define KCONFIGCORE_DEPRECATED_NO_EXPORT KCONFIGCORE_NO_EXPORT KCONFIGCORE_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef KCONFIGCORE_NO_DEPRECATED
#    define KCONFIGCORE_NO_DEPRECATED
#  endif
#endif

#endif
