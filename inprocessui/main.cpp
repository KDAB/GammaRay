
#include <core/probe.h>
#include <ui/mainwindow.h>


extern "C" {

void Q_DECL_EXPORT gammaray_create_inprocess_mainwindow()
{
  GammaRay::MainWindow *window = new GammaRay::MainWindow;
  window->setAttribute(Qt::WA_DeleteOnClose);
  GammaRay::Probe::instance()->setWindow(window);
  GammaRay::Probe::instance()->setParent(window);
  window->show();
}

}
