#ifndef ENDOSCOPE_WINDLLINJECTOR_H
#define ENDOSCOPE_WINDLLINJECTOR_H

#include <injector/abstractinjector.h>
#include <qglobal.h>

#ifdef Q_OS_WIN

namespace Endoscope {

class WinDllInjector : public AbstractInjector
{
  public:
    WinDllInjector();
    virtual int launch(const QStringList& programAndArgs, const QString& probeDll, const QString& probeFunc);

  private:
    bool inject();
    HANDLE m_destProcess;
    HANDLE m_destThread;
    QString m_dllPath;
};

}

#endif

#endif // ENDOSCOPE_WINDLLINJECTOR_H
