/*
  gdbinjector.h

  This file is part of Endoscope, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef ENDOSCOPE_GDBINJECTOR_H
#define ENDOSCOPE_GDBINJECTOR_H

#include <injector/abstractinjector.h>
#include <qprocess.h>

namespace Endoscope {

class GdbInjector : public AbstractInjector
{
  public:
    virtual int launch(const QStringList& programAndArgs, const QString& probeDll, const QString& probeFunc);
    virtual bool attach(int pid, const QString& probeDll, const QString& probeFunc);

  private:
    bool startGdb( const QStringList &args );
    int injectAndDetach( const QString &probeDll, const QString &probeFunc );

  private:
    QScopedPointer<QProcess> m_process;
};

}

#endif // ENDOSCOPE_GDBINJECTOR_H
