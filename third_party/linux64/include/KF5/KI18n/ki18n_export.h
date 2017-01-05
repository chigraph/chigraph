
#ifndef KI18N_EXPORT_H
#define KI18N_EXPORT_H

#ifdef KI18N_STATIC_DEFINE
#  define KI18N_EXPORT
#  define KI18N_NO_EXPORT
#else
#  ifndef KI18N_EXPORT
#    ifdef KF5I18n_EXPORTS
        /* We are building this library */
#      define KI18N_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define KI18N_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef KI18N_NO_EXPORT
#    define KI18N_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef KI18N_DEPRECATED
#  define KI18N_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef KI18N_DEPRECATED_EXPORT
#  define KI18N_DEPRECATED_EXPORT KI18N_EXPORT KI18N_DEPRECATED
#endif

#ifndef KI18N_DEPRECATED_NO_EXPORT
#  define KI18N_DEPRECATED_NO_EXPORT KI18N_NO_EXPORT KI18N_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef KI18N_NO_DEPRECATED
#    define KI18N_NO_DEPRECATED
#  endif
#endif

#endif
