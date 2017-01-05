
#ifndef KICONTHEMES_EXPORT_H
#define KICONTHEMES_EXPORT_H

#ifdef KICONTHEMES_STATIC_DEFINE
#  define KICONTHEMES_EXPORT
#  define KICONTHEMES_NO_EXPORT
#else
#  ifndef KICONTHEMES_EXPORT
#    ifdef KF5IconThemes_EXPORTS
        /* We are building this library */
#      define KICONTHEMES_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define KICONTHEMES_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef KICONTHEMES_NO_EXPORT
#    define KICONTHEMES_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef KICONTHEMES_DEPRECATED
#  define KICONTHEMES_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef KICONTHEMES_DEPRECATED_EXPORT
#  define KICONTHEMES_DEPRECATED_EXPORT KICONTHEMES_EXPORT KICONTHEMES_DEPRECATED
#endif

#ifndef KICONTHEMES_DEPRECATED_NO_EXPORT
#  define KICONTHEMES_DEPRECATED_NO_EXPORT KICONTHEMES_NO_EXPORT KICONTHEMES_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef KICONTHEMES_NO_DEPRECATED
#    define KICONTHEMES_NO_DEPRECATED
#  endif
#endif

#endif
