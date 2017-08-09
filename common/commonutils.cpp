#include "commonutils.h"

#if defined(Q_OS_WIN)
#include <qt_windows.h>
#endif

#if defined(Q_OS_WIN)
QString GammaRay::CommonUtils::windowsErrorString()
{
    wchar_t *string = 0;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL,
                  GetLastError(),
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPWSTR)&string,
                  0,
                  NULL);
    QString ret = QString::fromWCharArray(string);
    LocalFree((HLOCAL)string);
    return ret.trimmed();
}
#endif
