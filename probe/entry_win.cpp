/*
  entry_win.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <config-gammaray.h>

#include <windows.h>
#include <string>

typedef void (*gammaray_probe_inject)(void);

namespace {
static const std::wstring LOADER_NAME = L"gammaray_winloader";
static const std::string PROBE_NAME = GAMMARAY_PROBE_BASENAME;
}

extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpvReserved*/)
{
    switch (dwReason) {
    case DLL_PROCESS_ATTACH:
    {
        wchar_t buffer[MAX_PATH * 2];
        const int size = GetModuleFileNameW(hInstance, buffer, MAX_PATH * 2);
        if (!size) {
            OutputDebugStringW(L"GammaRay: GetModuleFileNameW failed");
            break;
        }
        const std::wstring probeName(PROBE_NAME.cbegin(), PROBE_NAME.cend());
        std::wstring path(buffer, size);
        path.replace(path.find(LOADER_NAME), LOADER_NAME.length(), probeName);

        HMODULE probe = GetModuleHandleW(path.c_str());
        if (!probe) {
            probe = LoadLibraryW(path.c_str());
            if (!probe) {
                OutputDebugStringW(L"GammaRay: Failed to load: ");
                OutputDebugStringW(path.c_str());
                break;
            }
        }
        gammaray_probe_inject inject = (gammaray_probe_inject)GetProcAddress(probe, "gammaray_probe_inject");
        if (!inject) {
            OutputDebugStringW(L"GammaRay: Failed to resolve gammaray_probe_inject");
            break;
        }
        inject();
    }
        break;
    }
    // return false to get unloaded
    return FALSE; // krazy:exclude=captruefalse
}
