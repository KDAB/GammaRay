
#include "config-endoscope.h"

#include "probefinder.h"

#include <qglobal.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QStringBuilder>

namespace Endoscope {

namespace ProbeFinder {

QString findProbe( const QString& baseName )
{
#ifndef Q_OS_WIN
  QStringList pldirs;
  pldirs << ENDOSCOPE_LIB_INSTALL_DIR
         << "/usr/local/lib64" << "/usr/local/lib"
         << "/opt/lib64" << "/opt/lib"
         << "/usr/lib64" << "/usr/lib";
  QDir::setSearchPaths( "preloads", pldirs );
#ifdef Q_OS_MAC
  QFile plfile( QLatin1Literal("preloads:") % baseName % QLatin1Literal(".dylib"));
#else
  QFile plfile( QLatin1Literal("preloads:") % baseName % QLatin1Literal(".so"));
#endif
  if ( plfile.exists() ) {
    return plfile.fileName();
  } else {
    qWarning() << "Cannot locate" << baseName << "in the typical places.\n"
              "Try setting the $LD_PRELOAD environment variable to the fullpath,\n"
              "For example:\n"
              "  export LD_PRELOAD=/opt/lib64/libendoscope_probe.so\n"
              "Continuing nevertheless, some systems can also preload from just the library name...";
    return baseName;
  }

#else
  return QCoreApplication::applicationDirPath() % baseName % QLatin1Literal( ".dll" );
#endif

  Q_ASSERT( false );
  return QString();
}

}
}
