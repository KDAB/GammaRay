/*
  selflocator.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "selflocator.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QString>

using namespace GammaRay;

#ifndef Q_OS_WIN

#include <dlfcn.h>

static QString findMeInternal()
{
    Dl_info info;
    if (dladdr(reinterpret_cast<void *>(&SelfLocator::findMe), &info) == 0)
        return QString();
    if (!info.dli_fname)
        return QString();
    return QString::fromLocal8Bit(info.dli_fname);
}

#else

#include <qt_windows.h>

static QString findMeInternal()
{
    WCHAR path[MAX_PATH];
    HMODULE handle;
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPWSTR>(&SelfLocator::findMe), &handle)) {
        GetModuleFileNameW(handle, path, sizeof(path));
        return QDir::fromNativeSeparators(QString::fromUtf16(reinterpret_cast<const char16_t *>(path)));
    }
    return QString();
}

#endif

QString SelfLocator::findMe()
{
    const QFileInfo fi(findMeInternal());
    return fi.canonicalFilePath();
}
