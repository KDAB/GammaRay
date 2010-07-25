#ifndef ENDOSCOPE_PROBE_H
#define ENDOSCOPE_PROBE_H

#include <qobject.h>


namespace Endoscope {

class ObjectListModel;
class ObjectTreeModel;

class Probe : public QObject
{
  Q_OBJECT
  public:
    static Probe* instance();
    static bool isInitialized();

    static void objectAdded( QObject *obj );
    static void objectRemoved( QObject *obj );

    ObjectListModel *objectListModel() const;
    ObjectTreeModel *objectTreeModel() const;

  private slots:
    void delayedInit();

  private:
    Probe( QObject* parent = 0 );
    static Probe* s_instance;

    ObjectListModel *m_objectListModel;
    ObjectTreeModel *m_objectTreeModel;
};

}

#endif // ENDOSCOPE_PROBE_H
