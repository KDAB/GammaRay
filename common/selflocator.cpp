/*
  selflocator.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
    if (dladdr(reinterpret_cast<void*>(&SelfLocator::findMe), &info) == 0)
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
        return QDir::fromNativeSeparators(QString::fromUtf16(reinterpret_cast<const ushort*>(path)));
    }
    return QString();
}

#endif

QString SelfLocator::findMe()
{
    const QFileInfo fi(findMeInternal());
    return fi.canonicalFilePath();
}
