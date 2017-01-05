
#ifndef KCOMPLETION_EXPORT_H
#define KCOMPLETION_EXPORT_H

#ifdef KCOMPLETION_STATIC_DEFINE
#  define KCOMPLETION_EXPORT
#  define KCOMPLETION_NO_EXPORT
#else
#  ifndef KCOMPLETION_EXPORT
#    ifdef KF5Completion_EXPORTS
        /* We are building this library */
#      define KCOMPLETION_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define KCOMPLETION_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef KCOMPLETION_NO_EXPORT
#    define KCOMPLETION_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef KCOMPLETION_DEPRECATED
#  define KCOMPLETION_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef KCOMPLETION_DEPRECATED_EXPORT
#  define KCOMPLETION_DEPRECATED_EXPORT KCOMPLETION_EXPORT KCOMPLETION_DEPRECATED
#endif

#ifndef KCOMPLETION_DEPRECATED_NO_EXPORT
#  define KCOMPLETION_DEPRECATED_NO_EXPORT KCOMPLETION_NO_EXPORT KCOMPLETION_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef KCOMPLETION_NO_DEPRECATED
#    define KCOMPLETION_NO_DEPRECATED
#  endif
#endif

#endif
