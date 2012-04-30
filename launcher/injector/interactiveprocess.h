/*
  interactiveprocess.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2006 Bradley T. Hughes  //krazy:exclude=copyright
  http://labs.qt.nokia.com/2006/03/16/starting-interactive-processes-with-qprocess/

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
#ifndef GAMMARAY_INTERACTIVEPROCESS_H
#define GAMMARAY_INTERACTIVEPROCESS_H

#include <QProcess>

class InteractiveProcess : public QProcess
{
  Q_OBJECT
  public:
    InteractiveProcess(QObject *parent = 0);

  protected:
    void setupChildProcess();

  private:
    static int stdinClone;
};

#endif // INTERACTIVEPROCESS_H
