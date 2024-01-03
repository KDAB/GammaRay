/*
  timertop.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Thomas McGuire <thomas.mcguire@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/
#ifndef GAMMARAY_TIMERTOP_TIMERTOP_H
#define GAMMARAY_TIMERTOP_TIMERTOP_H

#include "timertopinterface.h"

#include <core/toolfactory.h>

#include <QTimer>

QT_BEGIN_NAMESPACE
class QItemSelectionModel;
QT_END_NAMESPACE

namespace GammaRay {
namespace Ui {
class TimerTop;
}

class TimerTop : public TimerTopInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::TimerTopInterface)

public:
    explicit TimerTop(Probe *probe, QObject *parent = nullptr);

public slots:
    void clearHistory() override;

private slots:
    void objectSelected(QObject *obj);

private:
    QItemSelectionModel *m_selectionModel;
};

class TimerTopFactory : public QObject, public StandardToolFactory<QTimer, TimerTop>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_timertop.json")

public:
    explicit TimerTopFactory(QObject *parent = nullptr);
};
}

#endif // GAMMARAY_SCENEINSPECTOR_H
