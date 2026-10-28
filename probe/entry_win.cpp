/*
  entry_win.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <config-gammaray.h>

#include <windows.h>
#include <string>

typedef void (*gammaray_probe_inject)(void);

namespace {
static const std::wstring LOADER_NAME = L"gammaray_winloader";
static const std::string PROBE_NAME = GAMMARAY_PROBE_BASENAME;
}

// DO NOT MERGE: CI diagnostics, OutputDebugString doesn't reach the CI log
static void logStderr(const std::wstring &message)
{
    std::string narrow;
    for (wchar_t c : message)
        narrow.push_back(c < 128 ? static_cast<char>(c) : '?');
    narrow = "GammaRay loader: " + narrow + " (error " + std::to_string(GetLastError()) + ")\n";
    DWORD written = 0;
    WriteFile(GetStdHandle(STD_ERROR_HANDLE), narrow.data(), static_cast<DWORD>(narrow.size()), &written, nullptr);
}

extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpvReserved*/)
{
    switch (dwReason) {
    case DLL_PROCESS_ATTACH: {
        wchar_t buffer[MAX_PATH * 2];
        const int size = GetModuleFileNameW(hInstance, buffer, MAX_PATH * 2);
        if (!size) {
            OutputDebugStringW(L"GammaRay: GetModuleFileNameW failed");
            logStderr(L"GetModuleFileNameW failed");
            break;
        }
        const std::wstring probeName(PROBE_NAME.cbegin(), PROBE_NAME.cend());
        std::wstring path(buffer, size);
        path.replace(path.find(LOADER_NAME), LOADER_NAME.length(), probeName);

        logStderr(L"attached, loading " + path);
        HMODULE probe = GetModuleHandleW(path.c_str());
        if (!probe) {
            probe = LoadLibraryW(path.c_str());
            if (!probe) {
                OutputDebugStringW(L"GammaRay: Failed to load: ");
                OutputDebugStringW(path.c_str());
                logStderr(L"failed to load " + path);
                break;
            }
        }
        gammaray_probe_inject inject = ( gammaray_probe_inject )GetProcAddress(probe, "gammaray_probe_inject");
        if (!inject) {
            OutputDebugStringW(L"GammaRay: Failed to resolve gammaray_probe_inject");
            logStderr(L"failed to resolve gammaray_probe_inject");
            break;
        }
        logStderr(L"calling gammaray_probe_inject");
        inject();
        logStderr(L"gammaray_probe_inject returned");
    } break;
    }
    // return false to get unloaded
    return FALSE; // krazy:exclude=captruefalse
}
