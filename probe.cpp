/*
  probe.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
//krazy:excludeall=null,captruefalse,staticobjects

#include "probe.h"
#include "mainwindow.h"
#include "objectlistmodel.h"
#include "objecttreemodel.h"
#include "connectionmodel.h"
#include "toolmodel.h"
#include "readorwritelocker.h"
#include "tools/modelinspector/modeltest.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtGui/QMouseEvent>
#include <QtGui/QDialog>
#include <QtCore/QTimer>

#include <iostream>

#ifndef Q_OS_WIN
#include <dlfcn.h>
#else
#include <windows.h>
#endif

#define IF_DEBUG(x)

using namespace GammaRay;
using namespace std;

Probe *Probe::s_instance = 0;

namespace GammaRay
{

static bool probeConnectCallback(void ** args)
{
  QObject *sender = reinterpret_cast<QObject*>(args[0]);
  const char *signal = reinterpret_cast<const char*>(args[1]);
  QObject *receiver = reinterpret_cast<QObject*>(args[2]);
  const char *method = reinterpret_cast<const char*>(args[3]);
  const Qt::ConnectionType *type = reinterpret_cast<Qt::ConnectionType*>(args[4]);
  Probe::connectionAdded(sender, signal, receiver, method, *type);
  return false;
}

static bool probeDisconnectCallback(void ** args)
{
  QObject *sender = reinterpret_cast<QObject*>(args[0]);
  const char *signal = reinterpret_cast<const char*>(args[1]);
  QObject *receiver = reinterpret_cast<QObject*>(args[2]);
  const char *method = reinterpret_cast<const char*>(args[3]);
  Probe::connectionRemoved(sender, signal, receiver, method);
  return false;
}

}

// useful for debugging, dumps the object and all it's parents
// also useable from GDB!
void dumpObject(QObject *obj)
{
  if (!obj) {
    cout << "QObject(0x0)" << endl;
    return;
  }

  do {
    cout << obj->metaObject()->className() << "(" << hex << obj << ")";
    obj = obj->parent();
    if (obj) {
      cout << " <- ";
    }
  } while(obj);
  cout << endl;
}

struct Listener
{
  Listener()
    : filterThread(0),
      trackDestroyed(true)
  {}

  QThread *filterThread;
  bool trackDestroyed;
};

Q_GLOBAL_STATIC(Listener, s_listener)
Q_GLOBAL_STATIC(QVector<QObject*>, s_addedBeforeProbeInsertion)

Probe::Probe(QObject *parent):
  QObject(parent),
  m_objectListModel(new ObjectListModel(this)),
  m_objectTreeModel(new ObjectTreeModel(this)),
  m_connectionModel(new ConnectionModel(this)),
  m_toolModel(new ToolModel(this)),
  m_window(0),
  m_lock(QReadWriteLock::Recursive),
  m_queueTimer(new QTimer(this))
{
  qDebug() << Q_FUNC_INFO;

  QInternal::registerCallback(QInternal::ConnectCallback, &GammaRay::probeConnectCallback);
  QInternal::registerCallback(QInternal::DisconnectCallback, &GammaRay::probeDisconnectCallback);

  m_queueTimer->setSingleShot(true);
  m_queueTimer->setInterval(0);
  connect(m_queueTimer, SIGNAL(timeout()),
          this, SLOT(queuedObjectsFullyConstructed()));
}

Probe::~Probe()
{
  qDebug() << Q_FUNC_INFO;
  s_instance = 0;
}

void Probe::setWindow(GammaRay::MainWindow *window)
{
  m_window = window;
}

GammaRay::MainWindow *Probe::window() const
{
  return m_window;
}

Probe *GammaRay::Probe::instance()
{
  if (!qApp) {
    return NULL;
  }

  if (!s_instance) {
    IF_DEBUG(cout << "setting up new probe instance" << endl;)
    s_listener()->filterThread = QThread::currentThread();
    s_instance = new Probe;

    if (qgetenv("GAMMARAY_MODELTEST") == "1") {
      new ModelTest(s_instance->m_objectListModel, s_instance->m_objectListModel);
      new ModelTest(s_instance->m_objectTreeModel, s_instance->m_objectTreeModel);
      new ModelTest(s_instance->m_connectionModel, s_instance->m_connectionModel);
      new ModelTest(s_instance->m_toolModel, s_instance->m_toolModel);
    }
    s_listener()->filterThread = 0;
    IF_DEBUG(cout << "done setting up new probe instance" << endl;)

    s_instance->moveToThread(QCoreApplication::instance()->thread());
    //void* ptr = QCoreApplication::instance();

    QMetaObject::invokeMethod(s_instance, "delayedInit", Qt::QueuedConnection);
    foreach (QObject *obj, *(s_addedBeforeProbeInsertion())) {
      objectAdded(obj);
    }
    s_addedBeforeProbeInsertion()->clear();
  }
  return s_instance;
}

bool Probe::isInitialized()
{
  return s_instance && qApp;
}

void Probe::delayedInit()
{
  if (qgetenv("GAMMARAY_UNSET_PRELOAD") == "1") {
    qputenv("LD_PRELOAD", "");
  }
  if (qgetenv("GAMMARAY_UNSET_DYLD") == "1") {
    qputenv("DYLD_INSERT_LIBRARIES", "");
    qputenv("DYLD_FORCE_FLAT_NAMESPACE", "");
  }

  QCoreApplication::instance()->installEventFilter(s_instance);

  IF_DEBUG(cout << "creating GammaRay::MainWindow" << endl;)
  s_listener()->filterThread = QThread::currentThread();
  GammaRay::MainWindow *window = new GammaRay::MainWindow;
  s_listener()->filterThread = 0;
  IF_DEBUG(cout << "creation done" << endl;)

  window->setAttribute(Qt::WA_DeleteOnClose);
  instance()->setWindow(window);
  instance()->setParent(window);
  window->show();
}

static bool descendantOf(QObject *ascendant, QObject *obj)
{
  QObject *parent = obj->parent();
  if (!parent) {
    return false;
  }
  if (parent == ascendant) {
    return true;
  }
  return descendantOf(ascendant, parent);
}

/**
 * Returns true if @p obj belongs to the GammaRay Probe or Window.
 *
 * These objects should not be tracked or shown to the user,
 * hence must be explictly filtered.
 */
static bool filterObject(QObject *obj)
{
  Probe *p = Probe::instance();
  Q_ASSERT(p);
  if (obj->thread() != p->thread()) {
    // shortcut, never filter objects from a different thread
    return false;
  }
  return obj == p || obj == p->window() ||
          descendantOf(p, obj) ||
          descendantOf(p->window(), obj);
}

QAbstractItemModel *Probe::objectListModel() const
{
  return m_objectListModel;
}

QAbstractItemModel *Probe::objectTreeModel() const
{
  return m_objectTreeModel;
}

QAbstractItemModel *Probe::connectionModel() const
{
  return m_connectionModel;
}

ToolModel *Probe::toolModel() const
{
  return m_toolModel;
}

QObject *Probe::probe() const
{
  return const_cast<GammaRay::Probe*>(this);
}

bool Probe::isValidObject(QObject *obj) const
{
  ///TODO: can we somehow assert(m_lock.isLocked()) ?!
  return m_validObjects.contains(obj);
}

QReadWriteLock *Probe::objectLock() const
{
  return &m_lock;
}

void Probe::objectAdded(QObject *obj, bool fromCtor)
{
  if (s_listener()->filterThread == obj->thread()) {
    // Ignore
    IF_DEBUG(cout
             << "objectAdded Ignore: "
             << hex << obj
             << (fromCtor ? " (from ctor)" : "") << endl;)
    return;
  } else if (isInitialized()) {
    QWriteLocker lock(&instance()->m_lock);

    if (filterObject(obj)) {
      IF_DEBUG(cout
               << "objectAdded Filter: "
               << hex << obj
               << (fromCtor ? " (from ctor)" : "") << endl;)
      return;
    } else if (instance()->m_validObjects.contains(obj)) {
      // this happens when we get a child event before the objectAdded call from the ctor
      IF_DEBUG(cout
               << "objectAdded Known: "
               << hex << obj
               << (fromCtor ? " (from ctor)" : "") << endl;)
      Q_ASSERT(fromCtor);
      return;
    }

    // make sure we already know the parent
    if (obj->parent() && !instance()->m_validObjects.contains(obj->parent())) {
      objectAdded(obj->parent(), fromCtor);
    }
    Q_ASSERT(!obj->parent() || instance()->m_validObjects.contains(obj->parent()));

    instance()->m_validObjects << obj;
    if (s_listener()->trackDestroyed) {
      // when we did not use a preload variant that
      // overwrites qt_removeObject we must track object
      // deletion manually
      connect(obj, SIGNAL(destroyed(QObject*)),
              instance(), SLOT(handleObjectDestroyed(QObject*)),
              Qt::DirectConnection);
    }

    if (!fromCtor && obj->parent() && instance()->m_queuedObjects.contains(obj->parent())) {
      // when a child event triggers a call to objectAdded while inside the ctor
      // the parent is already tracked but it's call to objectFullyConstructed
      // was delayed. hence we must do the same for the child for integrity
      fromCtor = true;
    }

    IF_DEBUG(cout << "objectAdded: " << hex << obj
                  << (fromCtor ? " (from ctor)" : "")
                  << ", p: " << obj->parent() << endl;)

    if (fromCtor) {
      Q_ASSERT(!instance()->m_queuedObjects.contains(obj));
      instance()->m_queuedObjects << obj;
      if (!instance()->m_queueTimer->isActive()) {
        // timers must not be started from a different thread
        QMetaObject::invokeMethod(instance()->m_queueTimer, "start", Qt::AutoConnection);
      }
    } else {
      instance()->objectFullyConstructed(obj);
    }
  } else {
    IF_DEBUG(cout
             << "objectAdded Before: "
             << hex << obj
             << (fromCtor ? " (from ctor)" : "") << endl;)
    s_addedBeforeProbeInsertion()->push_back(obj);
  }
}

void Probe::queuedObjectsFullyConstructed()
{
  QWriteLocker lock(&m_lock);

  IF_DEBUG(cout << Q_FUNC_INFO << " " << m_queuedObjects.size() << endl;)

  // must be called from the main thread via timeout
  Q_ASSERT(QThread::currentThread() == thread());

  // when this is called no object must be in the queue twice
  // otherwise the cleanup procedures failed
  Q_ASSERT(m_queuedObjects.size() == m_queuedObjects.toSet().size());

  foreach (QObject *obj, m_queuedObjects) {
    objectFullyConstructed(obj);
  }

  IF_DEBUG(cout << Q_FUNC_INFO << " done" << endl;)

  m_queuedObjects.clear();
}

void Probe::objectFullyConstructed(QObject *obj)
{
  // must be write locked
  Q_ASSERT(!m_lock.tryLockForRead());

  if (!m_validObjects.contains(obj)) {
    // deleted already
    IF_DEBUG(cout << "stale fully constructed: " << hex << obj << endl;)
    return;
  } else if (filterObject(obj)) {
    // when the call was delayed from the ctor construction,
    // the parent might not have been set properly yet. hence
    // apply the filter again
    m_validObjects.remove(obj);
    IF_DEBUG(cout << "now filtered fully constructed: " << hex << obj << endl;)
    return;
  }

  IF_DEBUG(cout << "fully constructed: " << hex << obj << endl;)

  // ensure we know the parent already
  if (obj->parent() && !m_validObjects.contains(obj->parent())) {
    objectAdded(obj->parent());
  }
  Q_ASSERT(!obj->parent() || m_validObjects.contains(obj->parent()));

  m_objectListModel->objectAdded(obj);
  m_objectTreeModel->objectAdded(obj);
  m_toolModel->objectAdded(obj);

  emit objectCreated(obj);
}

void Probe::objectRemoved(QObject *obj)
{
  if (isInitialized()) {
    QWriteLocker lock(&instance()->m_lock);
    IF_DEBUG(cout << "object removed:" << hex << obj << " " << obj->parent() << endl;)

    bool success = instance()->m_validObjects.remove(obj);
    if (!success) {
      // object was not tracked by the probe, probably a gammaray object
      return;
    }

    instance()->m_queuedObjects.removeOne(obj);

    instance()->m_objectListModel->objectRemoved(obj);
    instance()->m_objectTreeModel->objectRemoved(obj);
    instance()->m_connectionModel->objectRemoved(obj);

    instance()->connectionRemoved(obj, 0, 0, 0);
    instance()->connectionRemoved(0, 0, obj, 0);

    emit instance()->objectDestroyed(obj);
  } else if (s_addedBeforeProbeInsertion()) {
    for (QVector<QObject*>::iterator it = s_addedBeforeProbeInsertion()->begin();
         it != s_addedBeforeProbeInsertion()->end();) {
      if (*it == obj) {
        it = s_addedBeforeProbeInsertion()->erase(it);
      } else {
        ++it;
      }
    }
  }
}

void Probe::handleObjectDestroyed(QObject *obj)
{
  objectRemoved(obj);
}

void Probe::connectionAdded(QObject *sender, const char *signal, QObject *receiver,
                            const char *method, Qt::ConnectionType type)
{
  if (!isInitialized() || !sender || !receiver ||
      s_listener()->filterThread == QThread::currentThread())
  {
    return;
  }

  ReadOrWriteLocker lock(&instance()->m_lock);
  if (filterObject(sender) || filterObject(receiver)) {
    return;
  }

  instance()->m_connectionModel->connectionAdded(sender, signal, receiver, method, type);
}

void Probe::connectionRemoved(QObject *sender, const char *signal,
                              QObject *receiver, const char *method)
{
  if (!isInitialized() || !s_listener() ||
      s_listener()->filterThread == QThread::currentThread())
  {
    return;
  }

  ReadOrWriteLocker lock(&instance()->m_lock);
  if ((sender && filterObject(sender)) || (receiver && filterObject(receiver))) {
    return;
  }

  instance()->m_connectionModel->connectionRemoved(sender, signal, receiver, method);
}

bool Probe::eventFilter(QObject *receiver, QEvent *event)
{
  if (s_listener()->filterThread == receiver->thread()) {
    return QObject::eventFilter(receiver, event);
  }

  if (event->type() == QEvent::ChildAdded || event->type() == QEvent::ChildRemoved) {
    QChildEvent *childEvent = static_cast<QChildEvent*>(event);
    QObject *obj = childEvent->child();

    QWriteLocker lock(&m_lock);
    const bool tracked = m_validObjects.contains(obj);
    const bool filtered = filterObject(obj);

    IF_DEBUG(cout << "child event: " << hex << obj << ", p: " << obj->parent() << dec
         << ", tracked: " << tracked
         << ", filtered: " << filtered
         << ", type: " << (childEvent->added() ? "added" : "removed") << endl;)

    if (!filtered && childEvent->added()) {
      if (!tracked) {
        // was not tracked before, add to all models
        objectAdded(obj);
      } else if (!m_queuedObjects.contains(obj)) {
        // object is known already, just update the position in the tree
        // BUT: only when we did not queue this item before
        IF_DEBUG(cout << "update pos: " << hex << obj << endl;)
        m_objectTreeModel->objectRemoved(obj);
        m_objectTreeModel->objectAdded(obj);
      }
    } else if (tracked) {
      objectRemoved(obj);
    }
  }
  if (event->type() == QEvent::MouseButtonRelease) {
    QMouseEvent *mouseEv = static_cast<QMouseEvent*>(event);
    if (mouseEv->button() == Qt::LeftButton &&
        mouseEv->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier)) {
      QWidget *widget = QApplication::widgetAt(mouseEv->globalPos());
      if (widget) {
        emit widgetSelected(widget, widget->mapFromGlobal(mouseEv->globalPos()));
      }
    }
  }

  // make modal dialogs non-modal so that the gammaray window is still reachable
  if (event->type() == QEvent::Show) {
    QDialog *dlg = qobject_cast<QDialog*>(receiver);
    if (dlg) {
      dlg->setWindowModality(Qt::NonModal);
    }
  }

  // we have no preloading hooks, so recover all objects we see
  if (s_listener()->trackDestroyed && event->type() != QEvent::ChildAdded && event->type() != QEvent::ChildRemoved && !filterObject(receiver)) {
    QWriteLocker lock(&m_lock);
    const bool tracked = m_validObjects.contains(receiver);
    if (!tracked)
      objectAdded(receiver);
  }
  return QObject::eventFilter(receiver, event);
}

void Probe::findExistingObjects()
{
  addObjectRecursive(QCoreApplication::instance());
  foreach (QObject *obj, QApplication::topLevelWidgets()) {
    addObjectRecursive(obj);
  }
}

void Probe::addObjectRecursive(QObject *obj)
{
  if (!obj) {
    return;
  }
  objectRemoved(obj); // in case we find it twice
  objectAdded(obj);
  foreach (QObject *child, obj->children()) {
    addObjectRecursive(child);
  }
}

// taken from qobject.cpp
const int gammaray_flagged_locations_count = 2;
static const char *gammaray_flagged_locations[gammaray_flagged_locations_count] = {0};

const char *Probe::connectLocation(const char *member)
{
  for (int i = 0; i < gammaray_flagged_locations_count; ++i) {
    if (member == gammaray_flagged_locations[i]) {
      // signature includes location information after the first null-terminator
      const char *location = member + qstrlen(member) + 1;
      if (*location != '\0') {
        return location;
      }
      return 0;
    }
  }
  return 0;
}

extern "C" Q_DECL_EXPORT void qt_startup_hook()
{
  s_listener()->trackDestroyed = false;

  qDebug() << Q_FUNC_INFO;
  Probe::instance();
#ifndef Q_OS_WIN
  static void(*next_qt_startup_hook)() = (void (*)()) dlsym(RTLD_NEXT, "qt_startup_hook");
  next_qt_startup_hook();
#endif
}

extern "C" Q_DECL_EXPORT void qt_addObject(QObject *obj)
{
  Probe::objectAdded(obj, true);
#ifndef Q_OS_WIN
  static void (*next_qt_addObject)(QObject *obj) =
    (void (*)(QObject *obj)) dlsym(RTLD_NEXT, "qt_addObject");
  next_qt_addObject(obj);
#endif
}

extern "C" Q_DECL_EXPORT void qt_removeObject(QObject *obj)
{
  Probe::objectRemoved(obj);
#ifndef Q_OS_WIN
  static void (*next_qt_removeObject)(QObject *obj) =
    (void (*)(QObject *obj)) dlsym(RTLD_NEXT, "qt_removeObject");
  next_qt_removeObject(obj);
#endif
}

#ifndef GAMMARAY_UNKNOWN_CXX_MANGLED_NAMES
#ifndef Q_OS_WIN
Q_DECL_EXPORT const char *qFlagLocation(const char *method)
#else
Q_DECL_EXPORT const char *myFlagLocation(const char *method)
#endif
{
  static int gammaray_idx = 0;
  gammaray_flagged_locations[gammaray_idx] = method;
  gammaray_idx = (gammaray_idx+1) % gammaray_flagged_locations_count;

#ifndef Q_OS_WIN
  static const char *(*next_qFlagLocation)(const char *method) =
    (const char * (*)(const char *method)) dlsym(RTLD_NEXT, "_Z13qFlagLocationPKc");

  Q_ASSERT_X(next_qFlagLocation, "",
             "Recompile with GAMMARAY_UNKNOWN_CXX_MANGLED_NAMES enabled, "
             "your compiler uses an unsupported C++ name mangling scheme");
  return next_qFlagLocation(method);
#else
  return method;
#endif
}
#endif

#ifdef Q_OS_WIN
// IMPORTANT NOTE:
// We are writing a jmp instruction at the target address, if the function is not big,
// we have to trust that the alignment gives us enough place to this jmp. Jumps are
// 10 bytes long under 32 bit and 13 bytes long under x64. We will overwrite the
// existing function, which cannot be reverted. It would be possible to save the content
// and write it back when we unattach.

void writeJmp(FARPROC func, ULONG_PTR replacement)
{
    DWORD oldProtect = 0;

    //Jump takes 10 bytes under x86 and 14 bytes under x64
    int worstSize;
#ifdef _M_IX86
    worstSize = 10;
#else ifdef _M_X64
    worstSize = 14;
#endif

    VirtualProtect(func, worstSize, PAGE_EXECUTE_READWRITE, &oldProtect);

    BYTE *cur = (BYTE *) func;

    // If there is a short jump, its a jumptable and we dont have enough space after, so
    // follow the short jump and write the jmp there
    if (*cur == 0xE9) {
        size_t old_offset = *(unsigned long *)(cur + 1);
        FARPROC ret = (FARPROC)(((DWORD)(((ULONG_PTR) cur) + sizeof (DWORD))) + old_offset + 1);
        writeJmp(ret, replacement);
        return;
    }

    *cur = 0xff;
    *(++cur) = 0x25;

#ifdef _M_IX86
    *((DWORD *) ++cur) = (DWORD)(((ULONG_PTR) cur) + sizeof (DWORD));
    cur += sizeof (DWORD);
    *((ULONG_PTR *)cur) = replacement;
#else ifdef _M_X64
    *((DWORD *) ++cur) = 0;
    cur += sizeof (DWORD);
    *((ULONG_PTR *)cur) = replacement;
#endif

    VirtualProtect(func, worstSize, oldProtect, &oldProtect);
}

extern "C" Q_DECL_EXPORT void gammaray_probe_inject();

BOOL WINAPI DllMain(HINSTANCE /*hInstance*/, DWORD dwReason, LPVOID/* lpvReserved */)
{
  // First retrieve the right module, if Qt is linked in release or debug
  HMODULE qtCoreDllHandle = GetModuleHandle(L"QtCore4");
  if (qtCoreDllHandle == NULL) {
    qtCoreDllHandle = GetModuleHandle(L"QtCored4");
  }

  if (qtCoreDllHandle == NULL) {
    qDebug() << "no handle for QtCore found!";
    return FALSE;
  }

  // Look up the address of qt_startup_hook
  FARPROC qtstartuphookaddr = GetProcAddress(qtCoreDllHandle, "qt_startup_hook");
  FARPROC qtaddobjectaddr = GetProcAddress(qtCoreDllHandle, "qt_addObject");
  FARPROC qtremobjectaddr = GetProcAddress(qtCoreDllHandle, "qt_removeObject");
#ifdef _M_X64
  FARPROC qFlagLocationaddr = GetProcAddress(qtCoreDllHandle, "?qFlagLocation@@YAPEBDPEBD@Z");
#else
  FARPROC qFlagLocationaddr = GetProcAddress(qtCoreDllHandle, "?qFlagLocation@@YAPBDPBD@Z");
#endif

  if (qtstartuphookaddr == NULL) {
    qDebug() << "no address for qt_startup_hook found!";
    return FALSE;
  }
  if (qtaddobjectaddr == NULL) {
    qDebug() << "no address for qt_addObject found!";
    return FALSE;
  }
  if (qtremobjectaddr == NULL) {
    qDebug() << "no address for qt_removeObject found!";
    return FALSE;
  }
  if (qFlagLocationaddr == NULL) {
    qDebug() << "no address for qFlagLocation found!";
    return FALSE;
  }

  switch(dwReason) {
  case DLL_PROCESS_ATTACH:
  {
    // write ourself into the hook chain
    writeJmp(qtstartuphookaddr, (ULONG_PTR)qt_startup_hook);
    writeJmp(qtaddobjectaddr, (ULONG_PTR)qt_addObject);
    writeJmp(qtremobjectaddr, (ULONG_PTR)qt_removeObject);
    writeJmp(qFlagLocationaddr, (ULONG_PTR)myFlagLocation);
    gammaray_probe_inject();
    break;
  }
  case DLL_PROCESS_DETACH:
  {
      //Unloading does not work, because we overwrite existing code
      exit(-1);
      break;
  }
  };
  return TRUE;
}
#endif

extern "C" Q_DECL_EXPORT void gammaray_probe_inject()
{
  if (!qApp) {
    return;
  }
  printf("gammaray_probe_inject()\n");
  GammaRay::Probe::instance();
  GammaRay::Probe::findExistingObjects();
  if (GammaRay::Probe::instance()->window()) {
    GammaRay::Probe::instance()->window()->show();
  }
}

#include "probe.moc"
