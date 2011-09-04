/*
  gdbinjector.cpp

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

#include "gdbinjector.h"

#include <QDebug>
#include <QProcess>
#include <QStringList>

#include <dlfcn.h>

using namespace Endoscope;

int GdbInjector::launch(const QStringList& programAndArgs, const QString& probeDll, const QString& probeFunc)
{
  QStringList gdbArgs;
  gdbArgs.push_back( QLatin1String("--args") );
  gdbArgs.append( programAndArgs );

  if ( !startGdb( gdbArgs ) )
    return -1;

  m_process->write( "break main\n" );
  m_process->write( "run\n" );
  m_process->write( "sha QtCore\n" );
  m_process->write( "break QCoreApplication::exec\n" );
  m_process->write( "continue\n" );

  return injectAndDetach( probeDll, probeFunc );
}

bool GdbInjector::attach(int pid, const QString& probeDll, const QString& probeFunc)
{
  Q_ASSERT( pid > 0 );
  if ( !startGdb( QStringList() << QLatin1String("-pid") << QString::number(pid) ) )
    return -1;
  return injectAndDetach( probeDll, probeFunc );
}

bool GdbInjector::startGdb(const QStringList& args)
{
  m_process.reset( new QProcess );
//   m_process->setProcessChannelMode( QProcess::ForwardedChannels );
  m_process->start( QLatin1String("gdb"), args );
  if ( !m_process->waitForStarted( -1 ) ) {
    qWarning() << m_process->errorString();
    return false;
  }
  return true;
}

int GdbInjector::injectAndDetach(const QString& probeDll, const QString& probeFunc)
{
  Q_ASSERT( m_process );
  m_process->write( "sha dl\n" );
  m_process->write( qPrintable( QString::fromLatin1("call dlopen(\"%1\", %2)\n").arg( probeDll ).arg( RTLD_NOW ) ) );
  m_process->write( qPrintable( QString::fromLatin1("sha %1\n").arg( probeDll ) ) );
  m_process->write( qPrintable( QString::fromLatin1("call %1()\n").arg( probeFunc ) ) );
  m_process->write( "detach\n" );
  m_process->write( "quit\n" );
  m_process->waitForBytesWritten( -1 );

  m_process->waitForFinished( -1 );
  return m_process->exitCode();
}
