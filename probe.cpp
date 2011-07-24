#include "probe.h"
#include "mainwindow.h"
#include "objectlistmodel.h"
#include "objecttreemodel.h"
#include "connectionmodel.h"
#include "modeltester.h"
#include "modelmodel.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <qevent.h>
#include <qgraphicsview.h>

#ifndef Q_OS_WIN
#include <dlfcn.h>
#else
#include <windows.h>
#ifdef USE_DETOURS
#include <detours.h>
#endif
#endif

#if defined(Q_OS_WIN) && defined(USE_DETOURS)
typedef void (* VoidFunc_t)();
typedef void (* QObjectFunc_t)( QObject *obj );
static VoidFunc_t true_qt_startup_hook_Func;
static QObjectFunc_t true_qt_addObject_Func;
static QObjectFunc_t true_qt_removeObject_Func;
#endif

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

struct Listener
{
  Listener()
    : active(true)
  { }

  bool active;
};

Q_GLOBAL_STATIC( Listener, s_listener )
Q_GLOBAL_STATIC( QVector<QObject*>, s_addedBeforeProbeInsertion )

Probe::Probe(QObject* parent):
  QObject(parent),
  m_objectListModel( new ObjectListModel( this ) ),
  m_objectTreeModel( new ObjectTreeModel( this ) ),
  m_connectionModel( new ConnectionModel( this ) ),
  m_modelTester( new ModelTester( this ) ),
  m_modelModel( new ModelModel( this ) ),
  m_window(0)
{
  qDebug() << Q_FUNC_INFO;

  qRegisterMetaType<QPointer<QObject> >();

  QInternal::registerCallback( QInternal::ConnectCallback, &Endoscope::probeConnectCallback );
  QInternal::registerCallback( QInternal::DisconnectCallback, &Endoscope::probeDisconnectCallback );
}

void Probe::setWindow(Endoscope::MainWindow* window)
{
  m_window = window;
}

Endoscope::MainWindow* Probe::window() const
{
  return m_window;
}

Probe* Endoscope::Probe::instance()
{
  if ( !s_instance ) {
    s_listener()->active = false;
    s_instance = new Probe;
    void* ptr = QCoreApplication::instance();

    QMetaObject::invokeMethod( s_instance, "delayedInit", Qt::QueuedConnection );
    s_listener()->active = true;
  }
  return s_instance;
}

bool Probe::isInitialized()
{
  return s_instance != 0;
}

void Probe::delayedInit()
{
  QCoreApplication::instance()->installEventFilter( s_instance );
  foreach ( QObject *obj, *(s_addedBeforeProbeInsertion()) )
    objectAdded( obj );
  s_addedBeforeProbeInsertion()->clear();

  s_listener()->active = false;
  Endoscope::MainWindow *window = new Endoscope::MainWindow;
  instance()->setWindow(window);
  window->show();
  s_listener()->active = true;
}

static bool descendantOf(QObject *ascendant, QObject *obj)
{
  QObject *parent = obj->parent();
  if (!parent)
    return false;
  if (parent == ascendant)
    return true;
  return descendantOf(ascendant, parent);
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
  if ( !s_listener()->active && obj->thread() == QThread::currentThread() ) {
    // Ignore
      return;
  } else if ( isInitialized() ) {
    if (obj == instance()->window() || descendantOf(instance()->window(), obj))
      return;
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
  if ( !isInitialized() || !s_listener()->active
      || descendantOf(instance()->window(), sender)
      || descendantOf(instance()->window(), receiver)
      || descendantOf(instance(), sender)
      || descendantOf(instance(), receiver))
    return;
  instance()->m_connectionModel->connectionAdded( sender, signal, receiver, method, type );
}

void Probe::connectionRemoved(QObject* sender, const char* signal, QObject* receiver, const char* method)
{
  if ( !isInitialized() || !s_listener()->active
      || (sender && descendantOf(instance()->window(), sender))
      || (receiver && descendantOf(instance()->window(), receiver))
      || (sender && descendantOf(instance(), sender))
      || (receiver && descendantOf(instance(), receiver)))
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
  if ( event->type() == QEvent::MouseButtonRelease ) {
    QMouseEvent *mouseEv = static_cast<QMouseEvent*>( event );
    if ( mouseEv->button() == Qt::LeftButton && mouseEv->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier) ) {
      QWidget *widget = QApplication::widgetAt( mouseEv->globalPos() );
      if ( widget ) {
        emit widgetSelected( widget );
        QGraphicsView *qgv = Util::findParentOfType<QGraphicsView>( widget );
        if ( qgv ) {
          QGraphicsItem* item = qgv->itemAt( qgv->mapFromGlobal( mouseEv->globalPos() ) );
          if ( item )
            emit graphicsItemSelected( item );
        }
      }
    }
  }
  return QObject::eventFilter(receiver, event);
}

void Probe::findExistingObjects()
{
  addObjectRecursive( QCoreApplication::instance() );
  foreach ( QObject *obj, QApplication::topLevelWidgets() )
    addObjectRecursive( obj );
}

void Probe::addObjectRecursive(QObject* obj)
{
  if ( !obj )
    return;
  objectRemoved( obj ); // in case we find it twice
  objectAdded( obj );
  foreach ( QObject *child, obj->children() )
    addObjectRecursive( child );
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

#ifdef Q_OS_WIN
typedef void (*qt_addObject_ptr)(QObject *obj);
typedef void (*qt_removeObject_ptr)(QObject *obj);
typedef void (*qt_startup_hook_ptr)();

qt_startup_hook_ptr next_qt_startup_hook = 0;
qt_addObject_ptr next_qt_addObject = 0;
qt_removeObject_ptr next_qt_removeObject = 0;
#endif

#ifndef USE_DETOURS
extern "C" Q_DECL_EXPORT void qt_startup_hook()
{
#ifndef Q_OS_WIN
  static void (*next_qt_startup_hook)() = (void (*)()) dlsym( RTLD_NEXT, "qt_startup_hook" );
#endif
  qDebug() << Q_FUNC_INFO;
  Probe::instance();
  next_qt_startup_hook();
}
#else
void fake_qt_startup_hook()
{
  Probe::instance();
  true_qt_startup_hook_Func();
}
#endif

#ifndef USE_DETOURS
extern "C" Q_DECL_EXPORT void qt_addObject( QObject *obj )
{
#ifndef Q_OS_WIN
  static void (*next_qt_addObject)(QObject* obj) = (void (*)(QObject *obj)) dlsym( RTLD_NEXT, "qt_addObject" );
#endif
  Probe::objectAdded( obj );
  next_qt_addObject( obj );
}
#else
void fake_qt_addObject( QObject *obj )
{
  Probe::objectAdded( obj );
  true_qt_addObject_Func( obj );
}
#endif

#ifndef USE_DETOURS
extern "C" Q_DECL_EXPORT void qt_removeObject( QObject *obj )
{
#ifndef Q_OS_WIN
  static void (*next_qt_removeObject)(QObject* obj) = (void (*)(QObject *obj)) dlsym( RTLD_NEXT, "qt_removeObject" );
#endif
  Probe::objectRemoved( obj );
  next_qt_removeObject( obj );
}
#else
void fake_qt_removeObject( QObject *obj )
{
  Probe::objectRemoved( obj );
  true_qt_removeObject_Func( obj );
}
#endif

#ifdef Q_OS_WIN
// IMPORTANT NOTE :
// In QtCored4.dll, qtstartuphookaddr et. al. actually point to a JMP instruction to the real qt_startup_hook code
// (the indirection is added by the linker as of the /INCREMENTAL link option), so it's easy to change the offset
// to redirect to our qt_startup_hook instead.
// this might not work in release builds though.

template<typename T>
T rewriteJmp(FARPROC func, T replacement) {
  MEMORY_BASIC_INFORMATION mbi;

  if(!VirtualQuery(func, &mbi, sizeof(MEMORY_BASIC_INFORMATION))) {
    qDebug() << "failed to query memory";
    return 0;
  }
  if(!VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_READWRITE, &mbi.Protect)) {
    qDebug() << "failed to protect memory";
    return 0;
  }

  unsigned char* pjmpbyte_add = reinterpret_cast<unsigned char*>(func);

  union {
    PBYTE pB;
    PINT pI;
  } ip;

  ip.pB = pjmpbyte_add;
  // make sure that the first instruction is a jump instruction
  *ip.pB++ = 0xE9;

  // read in the old offset
  size_t old_offset = *(unsigned long*)(pjmpbyte_add + 1);
  // make sure that we count the old_offset in bytes, and not in dwords!
  T ret = (T)((unsigned char*)(ip.pI + 1) + old_offset);
  // save the original value into next, addresses are calculated in bytes

  // make our memory the new jmpbyte_add destination
  *ip.pI++ = (unsigned long)replacement - (unsigned long)(ip.pI + 1);

  DWORD dummy;
  VirtualProtect(mbi.BaseAddress, mbi.RegionSize, mbi.Protect, &dummy);
  return ret;
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /* lpvReserved */ ) {
#ifdef USE_DETOURS
    switch(dwReason) {
        case DLL_PROCESS_ATTACH:
        {
            qt_startup_hook();
            break;
        }
        case DLL_PROCESS_DETACH:
        {
            break;
        }
    };
    return TRUE;
#else
  // First retrieve the right module, if Qt is linked in release or debug
  HMODULE qtCoreDllHandle = GetModuleHandle(L"QtCore4");
  if ( qtCoreDllHandle == NULL )
    qtCoreDllHandle = GetModuleHandle(L"QtCored4");

  if ( qtCoreDllHandle == NULL ) {
    qDebug() << "no handle for QtCore found!";
    return FALSE;
  }

  // Look up the address of qt_startup_hook
  FARPROC qtstartuphookaddr = GetProcAddress( qtCoreDllHandle, "qt_startup_hook" );
  FARPROC qtaddobjectaddr = GetProcAddress( qtCoreDllHandle, "qt_addObject" );
  FARPROC qtremobjectaddr = GetProcAddress( qtCoreDllHandle, "qt_removeObject" );

  if ( qtstartuphookaddr == NULL ) {
    qDebug() << "no address for qt_startup_hook found!";
    return FALSE;
  }
  if ( qtaddobjectaddr == NULL ) {
    qDebug() << "no address for qt_addObject found!";
    return FALSE;
  }
  if ( qtremobjectaddr == NULL ) {
    qDebug() << "no address for qt_removeObject found!";
    return FALSE;
  }

  switch(dwReason) {
    case DLL_PROCESS_ATTACH:
    {
      // write ourself into the hook chain
      next_qt_startup_hook = rewriteJmp<qt_startup_hook_ptr>(qtstartuphookaddr, qt_startup_hook);
      next_qt_addObject = rewriteJmp<qt_addObject_ptr>(qtaddobjectaddr, qt_addObject);
      next_qt_removeObject = rewriteJmp<qt_removeObject_ptr>(qtremobjectaddr, qt_removeObject);
      break;
    }
    case DLL_PROCESS_DETACH:
    {
      // in case the probe dll gets unloaded, lets remove ourselves from the hook chain
      rewriteJmp<qt_startup_hook_ptr>(qtstartuphookaddr, next_qt_startup_hook);
      rewriteJmp<qt_addObject_ptr>(qtaddobjectaddr, next_qt_addObject);
      rewriteJmp<qt_removeObject_ptr>(qtremobjectaddr, next_qt_removeObject);
      break;
    }
  };
  return TRUE;
#endif
}
#endif

#ifndef ENDOSCOPE_UKNOWN_CXX_MANGLED_NAMES
#ifndef Q_OS_WIN
Q_DECL_EXPORT const char* qFlagLocation( const char* method )
#else
Q_DECL_EXPORT const char* myFlagLocation( const char* method )
#endif
{
  static int endoscope_idx = 0;
  endoscope_flagged_locations[endoscope_idx] = method;
  endoscope_idx = (endoscope_idx+1) % endoscope_flagged_locations_count;

#ifndef Q_OS_WIN
  static const char* (*next_qFlagLocation)(const char* method) = (const char* (*)(const char* method)) dlsym( RTLD_NEXT, "_Z13qFlagLocationPKc" );
#else
  static const char* (*next_qFlagLocation)(const char* method);
#endif
  Q_ASSERT_X( next_qFlagLocation, "", "Recompile with ENDOSCOPE_UKNOWN_CXX_MANGLED_NAMES enabled, your compiler uses an unsupported C++ name mangling scheme" );
  return next_qFlagLocation( method );
}
#endif

extern "C" Q_DECL_EXPORT void endoscope_probe_inject()
{
  printf( "endoscope_probe_inject()\n" );
  Endoscope::Probe::instance();
  Endoscope::Probe::findExistingObjects();
  if ( Endoscope::Probe::instance()->window() )
    Endoscope::Probe::instance()->window()->show();
}

#include "probe.moc"
