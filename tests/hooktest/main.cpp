/*
  main.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Andreas Holzammer <andreas.holzammer@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include <QApplication>
#include <QDebug>
#include "mainwindow.h"

#include <dlfcn.h>
#include <inttypes.h>
#include <cassert>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/errno.h>

static inline void *
page_align(void *addr)
{
    assert(addr != 0);
    return ( void * )(( size_t )addr & ~(0xFFFF));
}

void writeJmp(void *func, void *replacement)
{
    quint8 *cur = ( quint8 * )func;
    quint8 *aligned = ( quint8 * )page_align(cur);
    assert(mprotect(aligned, 0xFFFF, PROT_READ | PROT_WRITE | PROT_EXEC) == 0);

    *cur = 0xff;
    *(++cur) = 0x25;

    *(( quint32 * )++cur) = 0;
    cur += sizeof(quint32);
    *(( quint64 * )cur) = ( quint64 )replacement;

    assert(mprotect(aligned, 0xFFFF, PROT_READ | PROT_EXEC) == 0);
}

void test()
{
    qWarning() << "hook test!";
}

int main(int argc, char *argv[])
{
    void *qt_startup_hook_addr = dlsym(RTLD_NEXT, "qt_startup_hook");
    writeJmp(qt_startup_hook_addr, ( void * )test);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
