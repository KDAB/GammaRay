#include "probe.h"
#include "mainwindow.h"
#include "objectlistmodel.h"
#include "objecttreemodel.h"
#include "connectionmodel.h"
#include "modeltester.h"
#include "modelmodel.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

#include <dlfcn.h>

using namespace Endoscope;

Probe* Probe::s_instance = 0;

Q_DECLARE_METATYPE( QPointer<QObject> )

namespace Endoscope
{

static bool probeConnectCallback( void ** args )
{
  QObject *sender = reinterpret_cast<QObject*>( args[0] );
  const char *signal = reinterpret_cast<const char*>( args[1] );
  QObject *receiver = reinterpret_cast<QObject*>( args[2] );
  const char *method = reinterpret_cast<const char*>( args[3] );
  const Qt::ConnectionType *type = reinterpret_cast<Qt::ConnectionType*>( args[4] );
  Probe::connectionAdded( sender, signal, receiver, method, *type );
  return false;
}

static bool probeDisconnectCallback( void ** args )
{
  QObject *sender = reinterpret_cast<QObject*>( args[0] );
  const char *signal = reinterpret_cast<const char*>( args[1] );
  QObject *receiver = reinterpret_cast<QObject*>( args[2] );
  const char *method = reinterpret_cast<const char*>( args[3] );
  Probe::connectionRemoved( sender, signal, receiver, method );
  return false;
}

}

Q_GLOBAL_STATIC( QVector<QObject*>, s_addedBeforeProbeInsertion )

Probe::Probe(QObject* parent):
  QObject(parent),
  m_objectListModel( new ObjectListModel( this ) ),
  m_objectTreeModel( new ObjectTreeModel( this ) ),
  m_connectionModel( new ConnectionModel( this ) ),
  m_modelTester( new ModelTester( this ) ),
  m_modelModel( new ModelModel( this ) )
{
  qDebug() << Q_FUNC_INFO;
  
  qRegisterMetaType<QPointer<QObject> >();

  QInternal::registerCallback( QInternal::ConnectCallback, &Endoscope::probeConnectCallback );
  QInternal::registerCallback( QInternal::DisconnectCallback, &Endoscope::probeDisconnectCallback );
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

ConnectionModel* Probe::connectionModel() const
{
  return m_connectionModel;
}

ModelTester* Probe::modelTester() const
{
  return m_modelTester;
}

ModelModel* Probe::modelModel() const
{
  return m_modelModel;
}

void Probe::objectAdded(QObject* obj)
{
  if ( isInitialized() ) {
    // use queued connection so object is fully constructed when we check if it's a model
    const QPointer<QObject> objPtr( obj );
    QMetaObject::invokeMethod( instance()->objectListModel(), "objectAdded", Qt::QueuedConnection, Q_ARG( QPointer<QObject>, objPtr ) );
    // ### queued connection here crashes the sort filter proxy, need to investigate that
    // might be due to children inserted before the parents
    instance()->objectTreeModel()->objectAdded( obj );
    QMetaObject::invokeMethod( instance()->modelTester(), "objectAdded", Qt::QueuedConnection, Q_ARG( QPointer<QObject>, objPtr ) );
    QMetaObject::invokeMethod( instance()->modelModel(), "objectAdded", Qt::QueuedConnection, Q_ARG( QPointer<QObject>, objPtr ) );
  } else {
    s_addedBeforeProbeInsertion()->push_back( obj );
  }
}

void Probe::objectRemoved(QObject* obj)
{
  if ( isInitialized() ) {
    instance()->objectListModel()->objectRemoved( obj );
    instance()->objectTreeModel()->objectRemoved( obj );
    instance()->connectionRemoved( obj, 0, 0, 0 );
    instance()->connectionRemoved( 0, 0, obj, 0 );
    instance()->modelModel()->objectRemoved( obj );
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

void Probe::connectionAdded(QObject* sender, const char* signal, QObject* receiver, const char* method, Qt::ConnectionType type)
{
  if ( !isInitialized() )
    return;
  instance()->m_connectionModel->connectionAdded( sender, signal, receiver, method, type );
}

void Probe::connectionRemoved(QObject* sender, const char* signal, QObject* receiver, const char* method)
{
  if ( !isInitialized() )
    return;
  instance()->m_connectionModel->connectionRemoved( sender, signal, receiver, method );
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

// taken from qobject.cpp
const int endoscope_flagged_locations_count = 2;
static const char* endoscope_flagged_locations[endoscope_flagged_locations_count] = {0};

const char* Probe::connectLocation(const char* member)
{
  for (int i = 0; i < endoscope_flagged_locations_count; ++i) {
    if (member == endoscope_flagged_locations[i]) {
      // signature includes location information after the first null-terminator
      const char *location = member + qstrlen(member) + 1;
      if (*location != '\0')
        return location;
      return 0;
    }
  }
  return 0;
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

#ifndef ENDOSCOPE_UKNOWN_CXX_MANGLED_NAMES
Q_DECL_EXPORT const char* qFlagLocation( const char* method )
{
  static int endoscope_idx = 0;
  endoscope_flagged_locations[endoscope_idx] = method;
  endoscope_idx = (endoscope_idx+1) % endoscope_flagged_locations_count;

  static const char* (*next_qFlagLocation)(const char* method) = (const char* (*)(const char* method)) dlsym( RTLD_NEXT, "_Z13qFlagLocationPKc" );
  Q_ASSERT_X( next_qFlagLocation, "", "Recompile with ENDOSCOPE_UKNOWN_CXX_MANGLED_NAMES enabled, your compiler uses an unsupported C++ name mangling scheme" );
  return next_qFlagLocation( method );
}
#endif

#include "probe.moc"
