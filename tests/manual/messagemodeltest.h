/*
  messagemodeltest.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_MESSAGEMODELTEST_H
#define GAMMARAY_MESSAGEMODELTEST_H

#include <QWidget>

namespace GammaRay {
class MessageGenerator : public QWidget
{
    Q_OBJECT

public:
    MessageGenerator();

private:
    static void generateDebug();
    static void generateWarning();
    static void generateCritical();
    [[noreturn]] static void generateFatal();
};
}

#endif // GAMMARAY_MESSAGEMODELTEST_H
