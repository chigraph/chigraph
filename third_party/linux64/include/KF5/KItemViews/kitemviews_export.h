
#ifndef KITEMVIEWS_EXPORT_H
#define KITEMVIEWS_EXPORT_H

#ifdef KITEMVIEWS_STATIC_DEFINE
#  define KITEMVIEWS_EXPORT
#  define KITEMVIEWS_NO_EXPORT
#else
#  ifndef KITEMVIEWS_EXPORT
#    ifdef KF5ItemViews_EXPORTS
        /* We are building this library */
#      define KITEMVIEWS_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define KITEMVIEWS_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef KITEMVIEWS_NO_EXPORT
#    define KITEMVIEWS_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef KITEMVIEWS_DEPRECATED
#  define KITEMVIEWS_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef KITEMVIEWS_DEPRECATED_EXPORT
#  define KITEMVIEWS_DEPRECATED_EXPORT KITEMVIEWS_EXPORT KITEMVIEWS_DEPRECATED
#endif

#ifndef KITEMVIEWS_DEPRECATED_NO_EXPORT
#  define KITEMVIEWS_DEPRECATED_NO_EXPORT KITEMVIEWS_NO_EXPORT KITEMVIEWS_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef KITEMVIEWS_NO_DEPRECATED
#    define KITEMVIEWS_NO_DEPRECATED
#  endif
#endif

#endif
