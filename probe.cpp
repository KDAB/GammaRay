#include "probe.h"
#include "mainwindow.h"
#include "objectlistmodel.h"
#include "objecttreemodel.h"

#include <QtCore/QDebug>

#include <dlfcn.h>

using namespace Endoscope;

Probe* Probe::s_instance = 0;

Probe::Probe(QObject* parent):
  QObject(parent),
  m_objectListModel( new ObjectListModel( this ) ),
  m_objectTreeModel( new ObjectTreeModel( this ) )
{
  qDebug() << Q_FUNC_INFO;
}

Probe* Endoscope::Probe::instance()
{
  if ( !s_instance ) {
    s_instance = new Probe;
    QMetaObject::invokeMethod( s_instance, "delayedInit", Qt::QueuedConnection );
  }
  return s_instance;
}

bool Probe::isInitialized()
{
  return s_instance != 0;
}

void Probe::delayedInit()
{
  Endoscope::MainWindow *window = new Endoscope::MainWindow;
  window->show();
}

ObjectListModel* Probe::objectListModel() const
{
  return m_objectListModel;
}

ObjectTreeModel* Probe::objectTreeModel() const
{
  return m_objectTreeModel;
}



extern "C" Q_DECL_EXPORT void qt_startup_hook()
{
  static void (*next_qt_startup_hook)() = (void (*)()) dlsym( RTLD_NEXT, "qt_startup_hook" );
  qDebug() << Q_FUNC_INFO;
  Probe::instance();
  next_qt_startup_hook();
}

extern "C" Q_DECL_EXPORT void qt_addObject( QObject *obj )
{
  static void (*next_qt_addObject)(QObject* obj) = (void (*)(QObject *obj)) dlsym( RTLD_NEXT, "qt_addObject" );
  if ( Probe::isInitialized() ) {
    Probe::instance()->objectListModel()->objectAdded( obj );
    Probe::instance()->objectTreeModel()->objectAdded( obj );
  }
  next_qt_addObject( obj );
}

extern "C" Q_DECL_EXPORT void qt_removeObject( QObject *obj )
{
  static void (*next_qt_removeObject)(QObject* obj) = (void (*)(QObject *obj)) dlsym( RTLD_NEXT, "qt_removeObject" );
  if ( Probe::isInitialized() ) {
    Probe::instance()->objectListModel()->objectRemoved( obj );
    Probe::instance()->objectTreeModel()->objectRemoved( obj );
  }
  next_qt_removeObject( obj );
}

#include "probe.moc"
