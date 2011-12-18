#include "metaobject.h"

using namespace GammaRay;

MetaObject::MetaObject()
{
}

MetaObject::~MetaObject()
{
  qDeleteAll(m_properties);
}

int MetaObject::propertyCount() const
{
  int count = 0;
  foreach (MetaObject *mo, m_baseClasses)
    count += mo->propertyCount();
  return count + m_properties.size();
}

MetaProperty* MetaObject::propertyAt(int index) const
{
  foreach (MetaObject *mo, m_baseClasses) {
    if (index >= mo->propertyCount()) {
      index -= mo->propertyCount();
    } else {
      return mo->propertyAt(index);
    }
  }
  Q_ASSERT(index >= 0 && index < m_properties.size());
  return m_properties.at(index);
}

void MetaObject::addBaseClass(MetaObject* baseClass)
{
  Q_ASSERT(baseClass);
  m_baseClasses.push_back(baseClass);
}

void MetaObject::addProperty(MetaProperty* property)
{
  Q_ASSERT(property);
  // TODO: sort
  property->setMetaObject(this);
  m_properties.push_back(property);
}

QString MetaObject::className() const
{
  return m_className;
}

void MetaObject::setClassName(const QString& className)
{
  m_className = className;
}

void* MetaObject::castForPropertyAt(void* object, int index) const
{
  for (int i = 0; i < m_baseClasses.size(); ++i) {
    const MetaObject *base = m_baseClasses.at(i);
    if (index >= base->propertyCount()) {
      index -= base->propertyCount();
    } else {
      return base->castForPropertyAt( castToBaseClass(object, i), index );
    }
  }
  return object; // our own property
}
