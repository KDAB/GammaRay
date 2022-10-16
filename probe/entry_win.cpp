/*
  entry_win.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
    case DLL_PROCESS_ATTACH: {
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
        gammaray_probe_inject inject = ( gammaray_probe_inject )GetProcAddress(probe, "gammaray_probe_inject");
        if (!inject) {
            OutputDebugStringW(L"GammaRay: Failed to resolve gammaray_probe_inject");
            break;
        }
        inject();
    } break;
    }
    // return false to get unloaded
    return FALSE; // krazy:exclude=captruefalse
}
