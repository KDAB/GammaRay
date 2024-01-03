/*
  wininjector-cli.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Hannah von Reth <hannah.vonreth@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "../core/injector/basicwindllinjector.h"

int main()
{
    int argc;
    wchar_t **argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argc != 4) {
        return -1;
    }
    wchar_t *pidString(argv[1]);
    wchar_t *path(argv[2]);
    wchar_t *probePath(argv[3]);

    return BasicWinDllInjector::injectProcess(pidString, path, probePath) ? 0 : -1;
}
