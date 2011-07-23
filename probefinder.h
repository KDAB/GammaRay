#ifndef ENDOSCOPE_PROBEFINDER_H
#define ENDOSCOPE_PROBEFINDER_H

class QString;

namespace Endoscope {

namespace ProbeFinder
{
  /**
   * Attempts to find the full path of the probe DLL.
   */
  QString findProbe( const QString &baseName );
}

}

#endif // ENDOSCOPE_PROBEFINDER_H
