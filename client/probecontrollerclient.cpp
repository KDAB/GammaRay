#include "probecontrollerclient.h"

#include <common/network/endpoint.h>

using namespace GammaRay;

ProbeControllerClient::ProbeControllerClient(QObject* parent): QObject(parent)
{
}

void ProbeControllerClient::detachProbe()
{
  Endpoint::instance()->invokeObject(objectName(), "detachProbe");
}

void ProbeControllerClient::quitHost()
{
  Endpoint::instance()->invokeObject(objectName(), "quitHost");
}

#include "probecontrollerclient.moc"
