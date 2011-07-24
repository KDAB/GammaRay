#ifndef ENDOSCOPE_INJECTORSTYLEPLUGIN_H
#define ENDOSCOPE_INJECTORSTYLEPLUGIN_H

#include <QStylePlugin>

namespace Endoscope {

class InjectorStylePlugin : public QStylePlugin
{
  Q_OBJECT
  public:
    QStyle* create( const QString& );
    QStringList keys() const;

  private:
    void inject();
};

}

#endif // ENDOSCOPE_INJECTORSTYLEPLUGIN_H
