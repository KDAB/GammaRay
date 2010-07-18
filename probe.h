#ifndef ENDOSCOPE_PROBE_H
#define ENDOSCOPE_PROBE_H

#include <qobject.h>


namespace Endoscope {

class ObjectListModel;


class Probe : public QObject
{
  Q_OBJECT
  public:
    static Probe* instance();
    static bool isInitialized();

    ObjectListModel *objectListModel() const;

  private slots:
    void delayedInit();

  private:
    Probe( QObject* parent = 0 );
    static Probe* s_instance;

    ObjectListModel *m_objectListModel;
};

}

#endif // ENDOSCOPE_PROBE_H
