
#ifndef KWINDOWSYSTEM_EXPORT_H
#define KWINDOWSYSTEM_EXPORT_H

#ifdef KWINDOWSYSTEM_STATIC_DEFINE
#  define KWINDOWSYSTEM_EXPORT
#  define KWINDOWSYSTEM_NO_EXPORT
#else
#  ifndef KWINDOWSYSTEM_EXPORT
#    ifdef KF5WindowSystem_EXPORTS
        /* We are building this library */
#      define KWINDOWSYSTEM_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define KWINDOWSYSTEM_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef KWINDOWSYSTEM_NO_EXPORT
#    define KWINDOWSYSTEM_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef KWINDOWSYSTEM_DEPRECATED
#  define KWINDOWSYSTEM_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef KWINDOWSYSTEM_DEPRECATED_EXPORT
#  define KWINDOWSYSTEM_DEPRECATED_EXPORT KWINDOWSYSTEM_EXPORT KWINDOWSYSTEM_DEPRECATED
#endif

#ifndef KWINDOWSYSTEM_DEPRECATED_NO_EXPORT
#  define KWINDOWSYSTEM_DEPRECATED_NO_EXPORT KWINDOWSYSTEM_NO_EXPORT KWINDOWSYSTEM_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef KWINDOWSYSTEM_NO_DEPRECATED
#    define KWINDOWSYSTEM_NO_DEPRECATED
#  endif
#endif

#endif
