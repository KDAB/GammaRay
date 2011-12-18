#include "metaproperty.h"
#include "metaobject.h"

#include <QString>
#include <QVariant>

using namespace GammaRay;

MetaProperty::MetaProperty() : m_class(0)
{
}

MetaProperty::~MetaProperty()
{
}

MetaObject* MetaProperty::metaObject() const
{
  Q_ASSERT(m_class);
  return m_class;
}

void MetaProperty::setMetaObject(MetaObject* om)
{
  m_class = om;
}
