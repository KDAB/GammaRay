/*
  timertop.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Thomas McGuire <thomas.mcguire@kdab.com>

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

#include <core/toolfactory.h>

#include <QTimer>

namespace GammaRay {

namespace Ui {
  class TimerTop;
}

class TimerTop : public QObject
{
  Q_OBJECT
  public:
    explicit TimerTop(ProbeInterface *probe, QObject *parent = 0);

  private:
    QTimer *m_updateTimer;
};

class TimerTopFactory : public QObject,
                        public StandardToolFactory<QTimer, TimerTop>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)
  Q_PLUGIN_METADATA(IID "com.kdab.gammaray.TimerTop")

  public:
    explicit TimerTopFactory(QObject *parent = 0) : QObject(parent)
    {
    }

    inline QString name() const
    {
      return tr("Timers");
    }

    /* reimp */
    QStringList supportedTypes() const;
};

}

#endif // GAMMARAY_SCENEINSPECTOR_H
