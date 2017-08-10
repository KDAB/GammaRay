#ifndef GAMMARAY_COMMON_UTILS_H
#define GAMMARAY_COMMON_UTILS_H

#include "gammaray_common_export.h"

#include <QtGlobal>


#define WIN_ERROR_ASSERT(condition, action) if (condition) {} else { qWarning("%s Error: %s failed: %s", Q_FUNC_INFO, #condition, qPrintable(qt_error_string())); action; } do {} while(false)
#define WIN_ERROR_CHECK(condition) WIN_ERROR_ASSERT(condition, qt_noop();)

namespace GammaRay {
namespace CommonUtils {

}
}

#endif // GAMMARAY_COMMON_UTILS_H
