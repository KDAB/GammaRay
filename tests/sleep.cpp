/*
  sleep.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include <cstdlib>

#include <QThread>

class QThreadAccessViolator : public QThread
{
public:
    using QThread::sleep; // protected in Qt4
};

int main(int argc, char *argv[])
{
    if (argc <= 1) {
        return 1;
    }

    const auto secs = strtoul(argv[1], nullptr, 0);
    QThreadAccessViolator::sleep(secs);

    return 0;
}
