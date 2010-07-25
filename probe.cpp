#include "probe.h"
#include "mainwindow.h"
#include "objectlistmodel.h"
#include "objecttreemodel.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

#include <dlfcn.h>

using namespace Endoscope;

Probe* Probe::s_instance = 0;

Q_GLOBAL_STATIC( QVector<QObject*>, s_addedBeforeProbeInsertion )

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
    QCoreApplication::instance()->installEventFilter( s_instance );
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
  foreach ( QObject *obj, *(s_addedBeforeProbeInsertion()) )
    objectAdded( obj );
  s_addedBeforeProbeInsertion()->clear();

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

void Probe::objectAdded(QObject* obj)
{
  if ( isInitialized() ) {
    instance()->objectListModel()->objectAdded( obj );
    instance()->objectTreeModel()->objectAdded( obj );
  } else {
    s_addedBeforeProbeInsertion()->push_back( obj );
  }
}

void Probe::objectRemoved(QObject* obj)
{
  if ( isInitialized() ) {
    instance()->objectListModel()->objectRemoved( obj );
    instance()->objectTreeModel()->objectRemoved( obj );
  } else {
    for ( QVector<QObject*>::iterator it = s_addedBeforeProbeInsertion()->begin();
         it != s_addedBeforeProbeInsertion()->end(); )
    {
      if ( *it == obj )
        it = s_addedBeforeProbeInsertion()->erase( it );
      else
        ++it;
    }
  }
}

bool Probe::eventFilter(QObject *receiver, QEvent *event )
{
  if ( event->type() == QEvent::ChildAdded || event->type() == QEvent::ChildRemoved ) {
    QChildEvent *childEvent = static_cast<QChildEvent*>( event );
    objectTreeModel()->objectRemoved( childEvent->child() );
    if ( event->type() == QEvent::ChildAdded )
      objectTreeModel()->objectAdded( childEvent->child() );
  }
  return QObject::eventFilter(receiver, event);
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
  Probe::objectAdded( obj );
  next_qt_addObject( obj );
}

extern "C" Q_DECL_EXPORT void qt_removeObject( QObject *obj )
{
  static void (*next_qt_removeObject)(QObject* obj) = (void (*)(QObject *obj)) dlsym( RTLD_NEXT, "qt_removeObject" );
  Probe::objectRemoved( obj );
  next_qt_removeObject( obj );
}

#include "probe.moc"
