/*
  messagemodeltest.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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

private slots:
    static void generateDebug();
    static void generateWarning();
    static void generateCritical();
    static void generateFatal(); // FIXME: Q_RETURN fails on some configurations
};
}

#endif // GAMMARAY_MESSAGEMODELTEST_H
