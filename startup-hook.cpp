#include <stdio.h>
#include <Qt/qglobal.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtGui/QGraphicsView>

#include <dlfcn.h>
#include "mainwindow.h"

class FilterObject : public QObject
{
  Q_OBJECT
  protected:
    bool eventFilter(QObject *obj, QEvent *ev) {
      if ( ev->type() == QEvent::Show && qobject_cast<QGraphicsView*>( obj ) )
        qDebug() << obj << ev->type();
      return QObject::eventFilter( obj, ev );
    }

  public slots:
    void delayedInit()
    {
      Endoscope::MainWindow *window = new Endoscope::MainWindow;
      window->show();
    }
};

extern "C" Q_DECL_EXPORT void qt_startup_hook()
{
  printf( "hello world!\n" );
  qDebug() << qApp;
  FilterObject *filter = new FilterObject;
  qApp->installEventFilter( filter );
  QMetaObject::invokeMethod( filter, "delayedInit", Qt::QueuedConnection );
}

extern "C" Q_DECL_EXPORT void qt_addObject( QObject *obj )
{
  static void (*qt_addObject_hook)(QObject* obj) = (void (*)(QObject *obj)) dlsym( RTLD_NEXT, "qt_addObject" );
//   printf( "qt_addObject: %i\n", obj );
  qt_addObject_hook( obj );
}

#include "startup-hook.moc"
