#ifndef ABSTRACTINJECTOR_H
#define ABSTRACTINJECTOR_H
#include <QtCore/qsharedpointer.h>

class QString;
class QStringList;

namespace Endoscope {

class AbstractInjector
{
  public:
    typedef QSharedPointer<AbstractInjector> Ptr;
    virtual ~AbstractInjector();

    /**
     * Launch the application @p program and inject @p probeDll and call @p probeFunc on it.
     */
    virtual int launch( const QStringList &programAndArgs, const QString &probeDll, const QString &probeFunc );

    /**
     * Attach to the running application with process id @p pid and inject @p probeDll and call @p probeFunc on it.
     */
    virtual bool attach( int pid, const QString &probeDll, const QString &probeFunc );
};

}

#endif // ABSTRACTINJECTOR_H
