#include <QtGui/QApplication>
#include <QDebug>
#include "mainwindow.h"

#include <dlfcn.h>
#include <inttypes.h>

void writeJmp(void *func, void *replacement)
{
  short *cur = (short *) func;

  *cur = 0xff;
  *(++cur) = 0x25;

  *((uintptr_t *) ++cur) = 0;
  cur += sizeof (uintptr_t);
  *((uintptr_t *)cur) = (uintptr_t)replacement;
}


void test()
{
    qWarning() << "hook test!";
}

int main(int argc, char *argv[])
{

    void *qt_startup_hook_addr = dlsym(RTLD_NEXT, "qt_startup_hook");
    writeJmp(qt_startup_hook_addr, (void *)test);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
