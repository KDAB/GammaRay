/*
  injectorfactory.cpp

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

#include "injectorfactory.h"

#include "detourinjector.h"
#include "gdbinjector.h"
#include "preloadinjector.h"
#include "styleinjector.h"
#include "windllinjector.h"

#include <QtCore/QString>

namespace Endoscope {

namespace InjectorFactory {

AbstractInjector::Ptr createInjector( const QString &name )
{
  if ( name == QLatin1String("gdb") )
    return AbstractInjector::Ptr( new GdbInjector );
  if ( name == QLatin1String("style") )
    return AbstractInjector::Ptr( new StyleInjector );
#ifndef Q_OS_WIN
  if ( name == QLatin1String("preload") )
    return AbstractInjector::Ptr( new PreloadInjector );
#else
  if ( name == QLatin1String("windll") )
    return AbstractInjector::Ptr( new WinDllInjector );
  if ( name == QLatin1String("detour") )
    return AbstractInjector::Ptr( new DetourInjector );
#endif
  return AbstractInjector::Ptr( 0 );
}

AbstractInjector::Ptr defaultInjectorForLaunch()
{
#ifndef Q_OS_WIN
  return createInjector( QLatin1String("preload") );
#else
  return createInjector( QLatin1String("windll") );
#endif
}

AbstractInjector::Ptr defaultInjectorForAttach()
{
  return createInjector( QLatin1String("gdb") );
}

}
}
