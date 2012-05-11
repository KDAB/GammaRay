#ifndef GAMMARAY_EXPORT_H
#define GAMMARAY_EXPORT_H

#include <qglobal.h>

#ifdef GAMMARAY_STATICLIB
#  undef GAMMARAY_SHAREDLIB
#  define GAMMARAY_EXPORT
#else
#  ifdef MAKE_GAMMARAY_LIB
#    define GAMMARAY_EXPORT Q_DECL_EXPORT
#  else
#    define GAMMARAY_EXPORT Q_DECL_IMPORT
#  endif
#endif

#endif /* GAMMARAY_EXPORT_H */
