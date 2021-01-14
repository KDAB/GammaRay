/*
  timertop.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Thomas McGuire <thomas.mcguire@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
