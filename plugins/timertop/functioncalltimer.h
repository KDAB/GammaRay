/*
  functioncalltimer.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#ifndef GAMMARAY_TIMERTOP_FUNCTIONCALLTIMER_H
#define GAMMARAY_TIMERTOP_FUNCTIONCALLTIMER_H

#include <qglobal.h>

#include <ctime>

namespace GammaRay {

class FunctionCallTimer
{
  public:
    FunctionCallTimer();
    bool start();
    bool active() const;
    int stop();

  private:
#ifndef Q_OS_WIN
    timespec m_startTime;
#else
    qint64 m_startTime;
#endif
    bool m_active;
};

}
#endif // GAMMARAY_FUNCTIONCALLTIMER_H
