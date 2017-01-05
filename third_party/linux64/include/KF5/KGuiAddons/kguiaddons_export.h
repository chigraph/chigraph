
#ifndef KGUIADDONS_EXPORT_H
#define KGUIADDONS_EXPORT_H

#ifdef KGUIADDONS_STATIC_DEFINE
#  define KGUIADDONS_EXPORT
#  define KGUIADDONS_NO_EXPORT
#else
#  ifndef KGUIADDONS_EXPORT
#    ifdef KF5GuiAddons_EXPORTS
        /* We are building this library */
#      define KGUIADDONS_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define KGUIADDONS_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef KGUIADDONS_NO_EXPORT
#    define KGUIADDONS_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef KGUIADDONS_DEPRECATED
#  define KGUIADDONS_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef KGUIADDONS_DEPRECATED_EXPORT
#  define KGUIADDONS_DEPRECATED_EXPORT KGUIADDONS_EXPORT KGUIADDONS_DEPRECATED
#endif

#ifndef KGUIADDONS_DEPRECATED_NO_EXPORT
#  define KGUIADDONS_DEPRECATED_NO_EXPORT KGUIADDONS_NO_EXPORT KGUIADDONS_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef KGUIADDONS_NO_DEPRECATED
#    define KGUIADDONS_NO_DEPRECATED
#  endif
#endif

#endif
