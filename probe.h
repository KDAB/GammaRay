#ifndef ENDOSCOPE_PROBE_H
#define ENDOSCOPE_PROBE_H

#include <qobject.h>
#include "probeinterface.h"

class QGraphicsItem;

namespace Endoscope {

class ModelModel;
class ModelTester;
class ConnectionModel;
class ObjectListModel;
class ObjectTreeModel;
class ToolModel;

class MainWindow;

class Q_DECL_EXPORT Probe : public QObject, public ProbeInterface
{
  Q_OBJECT
  public:
    static Probe* instance();
    static bool isInitialized();

    static void objectAdded( QObject *obj );
    static void objectRemoved( QObject *obj );
    static void connectionAdded( QObject* sender, const char* signal, QObject* receiver, const char* method, Qt::ConnectionType type );
    static void connectionRemoved( QObject *sender, const char *signal, QObject *receiver, const char *method );

    static void findExistingObjects();

    QAbstractItemModel *objectListModel() const;
    QAbstractItemModel *objectTreeModel() const;
    ConnectionModel *connectionModel() const;
    ModelTester *modelTester() const;
    ModelModel *modelModel() const;
    ToolModel *toolModel() const;

    static const char* connectLocation( const char *member );

    Endoscope::MainWindow *window() const;
    void setWindow(Endoscope::MainWindow *window);

  signals:
    void widgetSelected( QWidget* widget );
    void graphicsItemSelected( QGraphicsItem* item );

  protected:
    bool eventFilter(QObject *receiver, QEvent *event );

  private slots:
    void delayedInit();

  private:
    explicit Probe( QObject* parent = 0 );
    static void addObjectRecursive( QObject *obj );
    static Probe* s_instance;

    ObjectListModel *m_objectListModel;
    ObjectTreeModel *m_objectTreeModel;
    ConnectionModel *m_connectionModel;
    ModelTester *m_modelTester;
    ModelModel *m_modelModel;
    ToolModel *m_toolModel;
    Endoscope::MainWindow *m_window;
};

}

#endif // ENDOSCOPE_PROBE_H
