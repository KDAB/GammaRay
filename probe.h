#ifndef ENDOSCOPE_PROBE_H
#define ENDOSCOPE_PROBE_H

#include <qobject.h>


namespace Endoscope {

class ConnectionModel;
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
    static void connectionAdded( QObject* sender, const char* signal, QObject* receiver, const char* method, Qt::ConnectionType type );
    static void connectionRemoved( QObject *sender, const char *signal, QObject *receiver, const char *method );

    ObjectListModel *objectListModel() const;
    ObjectTreeModel *objectTreeModel() const;
    ConnectionModel *connectionModel() const;

  protected:
    bool eventFilter(QObject *receiver, QEvent *event );

  private slots:
    void delayedInit();

  private:
    explicit Probe( QObject* parent = 0 );
    static Probe* s_instance;

    ObjectListModel *m_objectListModel;
    ObjectTreeModel *m_objectTreeModel;
    ConnectionModel *m_connectionModel;
};

}

#endif // ENDOSCOPE_PROBE_H
