/*
  sleep.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
