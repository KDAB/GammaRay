#include "probecontroller.h"
#include "probe.h"

#include <QCoreApplication>

using namespace GammaRay;

ProbeController::ProbeController(QObject* parent): QObject(parent)
{
}

void ProbeController::detachProbe()
{
  Probe::instance()->deleteLater();
}

void ProbeController::quitHost()
{
  QCoreApplication::instance()->quit();
}

#include "probecontroller.moc"
