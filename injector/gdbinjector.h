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
