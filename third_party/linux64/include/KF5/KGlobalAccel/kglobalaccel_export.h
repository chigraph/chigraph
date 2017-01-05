
#ifndef KGLOBALACCEL_EXPORT_H
#define KGLOBALACCEL_EXPORT_H

#ifdef KGLOBALACCEL_STATIC_DEFINE
#  define KGLOBALACCEL_EXPORT
#  define KGLOBALACCEL_NO_EXPORT
#else
#  ifndef KGLOBALACCEL_EXPORT
#    ifdef KF5GlobalAccel_EXPORTS
        /* We are building this library */
#      define KGLOBALACCEL_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define KGLOBALACCEL_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef KGLOBALACCEL_NO_EXPORT
#    define KGLOBALACCEL_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef KGLOBALACCEL_DEPRECATED
#  define KGLOBALACCEL_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef KGLOBALACCEL_DEPRECATED_EXPORT
#  define KGLOBALACCEL_DEPRECATED_EXPORT KGLOBALACCEL_EXPORT KGLOBALACCEL_DEPRECATED
#endif

#ifndef KGLOBALACCEL_DEPRECATED_NO_EXPORT
#  define KGLOBALACCEL_DEPRECATED_NO_EXPORT KGLOBALACCEL_NO_EXPORT KGLOBALACCEL_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef KGLOBALACCEL_NO_DEPRECATED
#    define KGLOBALACCEL_NO_DEPRECATED
#  endif
#endif

#endif
