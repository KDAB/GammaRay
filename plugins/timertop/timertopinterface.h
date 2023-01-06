/*
  timertopinterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_TIMERTOP_TIMERTOPINTERFACE_H
#define GAMMARAY_TIMERTOP_TIMERTOPINTERFACE_H

#include <QObject>

namespace GammaRay {
class TimerTopInterface : public QObject
{
    Q_OBJECT

public:
    explicit TimerTopInterface(QObject *parent = nullptr);
    ~TimerTopInterface() override;

public slots:
    virtual void clearHistory() = 0;
};
}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::TimerTopInterface,
                    "com.kdab.GammaRay.TimerTopInterface/1.0")
QT_END_NAMESPACE

#endif // GAMMARAY_TIMERTOP_TIMERTOPINTERFACE_H
