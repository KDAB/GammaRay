#include "propertycontrollerextension.h"

using namespace GammaRay;

PropertyControllerExtension::PropertyControllerExtension(const QString& name)
  : m_name(name)
{
}

const QString &PropertyControllerExtension::name() const {
  return m_name;
}
