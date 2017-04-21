/*
  execution.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "execution.h"

#include <QtGlobal>

using namespace GammaRay;

#ifndef Q_OS_WIN

#include <dlfcn.h>

bool Execution::isReadOnlyData(const void* data)
{
    Dl_info info;
    // ### technically we would also need to check if we are in a read-only section, but this close enough for our purpose
    return dladdr(data, &info) != 0;
}

#else

#include <qt_windows.h>

bool Execution::isReadOnlyData(const void* data)
{
    HMODULE handle;
    // ### technically we would also need to check if we are in a read-only section, but this close enough for our purpose
    return GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCWSTR>(data), &handle);
}

#endif
