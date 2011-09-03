/*
  preloadinjector.cpp

  This file is part of Endoscope, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krauss@kdab.com>

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

#include "preloadinjector.h"

#ifndef Q_OS_WIN

#include <QProcess>

using namespace Endoscope;

int PreloadInjector::launch(const QStringList& programAndArgs, const QString& probeDll, const QString& probeFunc)
{
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
#ifdef Q_OS_MAC
  env.insert( "DYLD_INSERT_LIBRARIES", probeDll );
#else
  env.insert( "LD_PRELOAD", probeDll );
#endif
  QProcess proc;
  proc.setProcessEnvironment( env );
  proc.setProcessChannelMode( QProcess::ForwardedChannels );

  QStringList args = programAndArgs;
  const QString program = args.takeFirst();
  proc.start( program, args );
  proc.waitForFinished( -1 );

  return proc.exitCode();
}

#endif
